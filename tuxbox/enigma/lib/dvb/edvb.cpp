 #include "edvb.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dbox/info.h>
#include <algorithm>
#include <string.h>

#ifdef PROFILE
	#include <sys/time.h>
#endif

#include "config.h"

#include <core/driver/eavswitch.h>
#include <core/driver/streamwd.h>
#include <core/dvb/esection.h>
#include <core/dvb/si.h>
#include <core/dvb/frontend.h>
#include <core/dvb/dvb.h>
#include <core/dvb/decoder.h>
#include <core/dvb/record.h>
#include <core/system/init.h>
#include <core/system/econfig.h>

#include "dvbservice.h"
#include "dvbscan.h"

eDVBController::~eDVBController()
{
}

eDVB *eDVB::instance;

eString eDVB::getVersion()
{
	return "eDVB core 1.0, compiled " __DATE__;
}

void eDVB::event(const eDVBEvent &event)
{
	eventOccured(event);
	if (controller)
		controller->handleEvent(event);
}

void eDVB::tunedIn(eTransponder *trans, int err)
{
	event(eDVBEvent(eDVBEvent::eventTunedIn, err, trans));
}

eDVB::eDVB(): state(eDVBState::stateIdle)
{
	settings=0;
	time_difference=0;

		// singleton
	if (instance)
		eFatal("eDVB already initialized!");
	instance=this;

		// initialize frontend (koennte auch nochmal raus)
	eString frontend=getInfo("fe");
	int fe;
	if (!frontend.length())
	{
		eDebug("WARNING: couldn't determine frontend-type, assuming satellite...");
		fe=eFrontend::feSatellite;
	} else
	{
		switch(atoi(frontend.c_str()))
		{
		case DBOX_FE_CABLE:
			fe=eFrontend::feCable;
			break;
		case DBOX_FE_SAT:
			fe=eFrontend::feSatellite;
			break;
		default:
			eDebug("COOL: dvb-t is out. less cool: eDVB doesn't support it yet...");
			fe=eFrontend::feCable;
			break;
		}
	}
	if (eFrontend::open(fe)<0)
		eFatal("couldn't open frontend");

	settings = new eDVBSettings(*this);
		// tuned-in handling
	CONNECT(eFrontend::getInstance()->tunedIn, eDVB::tunedIn);

		// decoder init
	Decoder::Initialize();
	
		// set to idle state
	controller=0;
	setState(eDVBState(eDVBState::stateIdle));
	
		// intialize to service mode
	setMode(controllerService);

		// init AV switch
	int type=0;
	type=atoi( getInfo("mID").c_str() );
	
	switch (type)
	{
	case 1:
		new eAVSwitchNokia;
		break;
	case 2:
		new eAVSwitchPhilips;
		break;
	case 3:
		new eAVSwitchSagem;
		break;
	default:
		new eAVSwitchNokia;
		break;
	}
	
	eAVSwitch::getInstance()->setInput(0);
	eAVSwitch::getInstance()->setActive(1);
	
		// init stream watchdog
	eStreamWatchdog::getInstance()->reloadSettings();

		// initial volume settings
	int vol, m;
	if (eConfig::getInstance()->getKey("/elitedvb/audio/volume", vol))
		vol=10;
	if (eConfig::getInstance()->getKey("/elitedvb/audio/mute", m))
		m=0;
	changeVolume(1, vol);
	changeVolume(3, m);
	
//	tMHWEIT=0;

		// init dvb recorder
	recorder=0;
	
	eDebug("eDVB::eDVB done.");
}

eDVB::~eDVB()
{
	delete settings; 
	recEnd();

	eAVSwitch::getInstance()->setActive(0);
	delete eAVSwitch::getInstance();
	eConfig::getInstance()->setKey("/elitedvb/audio/volume", volume);
	eConfig::getInstance()->setKey("/elitedvb/audio/mute", mute);
	Decoder::Close();

	eFrontend::close();
	instance=0;
}

eString eDVB::getInfo(const char *info)
{
	FILE *f=fopen("/proc/bus/dbox", "rt");
	if (!f)
		return "";
	eString result;
	while (1)
	{
		char buffer[128];
		if (!fgets(buffer, 128, f))
			break;
		if (strlen(buffer))
			buffer[strlen(buffer)-1]=0;
		if ((!strncmp(buffer, info, strlen(info)) && (buffer[strlen(info)]=='=')))
		{
			int i = strlen(info)+1;
			result = eString(buffer).mid(i, strlen(buffer)-i);
			break;
		}
	}	
	fclose(f);
	return result;
}

		// for external access only
PMT *eDVB::getPMT()
{
	return tPMT.ready()?tPMT.getCurrent():0;
}

EIT *eDVB::getEIT()
{
	return tEIT.ready()?tEIT.getCurrent():0;
}

SDT *eDVB::getSDT()
{
	return tSDT.ready()?tSDT.getCurrent():0;
}

void eDVB::changeVolume(int abs, int vol)
{
	switch (abs)
	{
		case 0:
			volume+=vol;
			mute=0;
		break;
		case 1:
			volume=vol;
			mute=0;
		break;
		case 2:
			if (vol)
			mute=!mute;
		break;
		case 3:
			mute=vol;
		break;
	}
	
	if (volume<0)
		volume=0;
	if (volume>63)
		volume=63;

	eAVSwitch::getInstance()->setVolume(mute?0:((63-volume)*65536/64));

	/*emit*/ volumeChanged(mute?63:volume);
}

static void unpack(__u32 l, int *t)
{
	for (int i=0; i<4; i++)
		*t++=(l>>((3-i)*8))&0xFF;
}

void eDVB::configureNetwork()
{
	__u32 sip=0, snetmask=0, sdns=0, sgateway=0;
	int ip[4], netmask[4], dns[4], gateway[4];
	int sdosetup=0;

	eConfig::getInstance()->getKey("/elitedvb/network/ip", sip);
	eConfig::getInstance()->getKey("/elitedvb/network/netmask", snetmask);
	eConfig::getInstance()->getKey("/elitedvb/network/dns", sdns);
	eConfig::getInstance()->getKey("/elitedvb/network/gateway", sgateway);
	eConfig::getInstance()->getKey("/elitedvb/network/dosetup", sdosetup);

	unpack(sip, ip);
	unpack(snetmask, netmask);
	unpack(sdns, dns);
	unpack(sgateway, gateway);
	
	if (sdosetup)
	{
		FILE *f=fopen("/etc/resolv.conf", "wt");
		if (!f)
			eDebug("couldn't write resolv.conf");
		else
		{
			fprintf(f, "# Generated by enigma\nnameserver %d.%d.%d.%d\n", dns[0], dns[1], dns[2], dns[3]);
			fclose(f);
		}
		eString buffer;
		buffer.sprintf("/sbin/ifconfig eth0 %d.%d.%d.%d up netmask %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3], netmask[0], netmask[1], netmask[2], netmask[3]);
		if (system(buffer.c_str())>>8)
			eDebug("'%s' failed.", buffer.c_str());
		else
		{
			system("/sbin/route del default 2> /dev/null");
			buffer.sprintf("/sbin/route add default gw %d.%d.%d.%d", gateway[0], gateway[1], gateway[2], gateway[3]);
			if (system(buffer.c_str())>>8)
				eDebug("'%s' failed", buffer.c_str());
		}
	}
}

void eDVB::recBegin(const char *filename)
{
	if (recorder)
		recEnd();
	recorder=new eDVBRecorder();
	if (Decoder::parms.apid != -1)		// todo! get pids from PMT
		recorder->addPID(Decoder::parms.apid);
	if (Decoder::parms.vpid != -1)
		recorder->addPID(Decoder::parms.vpid);
	if (Decoder::parms.tpid != -1)
		recorder->addPID(Decoder::parms.tpid);
	if (Decoder::parms.pcrpid != -1)
		recorder->addPID(Decoder::parms.pcrpid);
	recorder->addPID(0);
	if (Decoder::parms.pmtpid != -1)
		recorder->addPID(Decoder::parms.pmtpid);
	recResume();
}

void eDVB::recPause()
{
	recorder->stop();
}

void eDVB::recResume()
{
	recorder->start();
}

void eDVB::recEnd()
{
	if (!recorder)
		return;
	recPause();
	recorder->close();
	delete recorder;
	recorder=0;
}

void eDVB::setMode(int mode)
{
	if (controllertype == mode)
		return;
	controllertype = mode;
	if (controller)
		delete controller;
	switch (mode)
	{
	case controllerScan:
		controller = new eDVBScanController(*this);
		break;
	case controllerService:
		controller = new eDVBServiceController(*this);
		break;
	}
}

eDVBServiceController *eDVB::getServiceAPI()
{
	if (controllertype != controllerService)
		return 0;
	return (eDVBServiceController*)controller;
}

eDVBScanController *eDVB::getScanAPI()
{
	if (controllertype != controllerScan)
		return 0;
	return (eDVBScanController*)controller;
}

eAutoInitP0<eDVB> init_dvb(4, "eDVB core");
