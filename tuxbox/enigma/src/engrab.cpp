#include <engrab.h>
#include <plugin.h>
#include <stdio.h>
#include <fstream.h>
#include <iostream.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <lib/dvb/decoder.h>
#include <lib/dvb/dvb.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/epgcache.h>
#include <lib/base/estring.h>
//#define TMP_NgrabXML "/var/tmp/e-ngrab.xml"
#include <lib/gui/enumber.h>
#include <lib/gui/statusbar.h>


static void unpack(const struct in_addr &l, int *t)
{
	uint8_t *tc = (uint8_t *) &l.s_addr;
	for (int i = 0; i < 4; ++i)
		t[i] = tc[i];
}

static void pack(struct in_addr &l, const int *const t)
{
	uint8_t *tc = (uint8_t *) &l.s_addr;
	for (int i = 0; i < 4; ++i)
		tc[i] = t[i];
}

static eString getServiceName()
{
	eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();
	if(!sapi)
		return "not available";
	eService *current=eDVB::getInstance()->settings->getTransponders()->searchService(sapi->service);

	if(!current)
		return "no channel selected";

	return current->service_name.c_str();
}

static eString getEPGTitle()
{
	eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();

	if(!sapi)
		return "not available";

	const eventMap* evt=eEPGCache::getInstance()->getEventMap(sapi->service);

	if(!evt)
		return "not available";

	eventMap::const_iterator It;
	It=evt->begin();
	EITEvent event(*It->second);
	for(ePtrList<Descriptor>::iterator d(event.descriptor); d!= event.descriptor.end(); ++d)
	{
		Descriptor *descriptor=*d;
		if(descriptor->Tag()==DESCR_SHORT_EVENT)
		{
			return eString(((ShortEventDescriptor*)descriptor)->event_name);
		}
	}
	return "not available";
}


 ENgrab::ENgrab()
         :eWindow(1), lb(this)
 {
		 move( ePoint(100,100));  // Fenster verschieben
         cresize( eSize(400, 200 )); // Grösse festlegen
         lb.cmove( ePoint(0,0));
         lb.cresize( eSize( 400,200 ));
		 setText(_("ngrab plugin"));

	CONNECT((new eListBoxEntryMenu(&lb, _("[back]"), _("Zurück zum Pluginmenü") ))->selected, ENgrab::onBackSelected);
	CONNECT((new eListBoxEntryMenu(&lb, _("start recording"), _("Aufnahme manuell Starten") ))->selected, ENgrab::sendstart);
	CONNECT((new eListBoxEntryMenu(&lb, _("stopp recording"), _("Aufnahme manuell Stoppen") ))->selected, ENgrab::sendstopp);
#ifdef TMP_NgrabXML
	CONNECT((new eListBoxEntryMenu(&lb, _("send userXML"), _("User XML an port und IP senden") ))->selected, ENgrab::userxmls);//muß noch eingebaut werden
#endif
	//CONNECT((new eListBoxEntryMenu(&lb, _("config"), _("Hier könnt ihr die IP und den Port des Ngrab Servers einstellen") ))->selected, ENgrab::nsetup);// wurde nur im demo gebraucht, diese funktion ist jetzt im setupmenue
         lb.moveSelection(eListBox<eListBoxEntryMenu>::dirFirst);//keine ahnung warum das nicht funzt!?!

 }

 ENgrab::~ENgrab()
 {
 }
 

 void ENgrab::NgrabMenue()
{
	ENgrab fenster;
	fenster.show();
	fenster.exec();
	fenster.hide();
}

void ENgrab::onBackSelected()
{
	close(0);
}

void ENgrab::userxmls()
{
	// Hier muß ich noch eine DAtei in ein eString einlesen und dann ....
}

 eString ENgrab::startxml()
{
		eDebug("Start xml wird erstellt");

eString xmlstart;

		xmlstart+="<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		xmlstart+=" <neutrino commandversion=\"1\">\n";
		xmlstart+="   <record command=\"record\">\n";
		xmlstart+="    <channelname>"+getServiceName()+"</channelname>\n";//übernommen von trh
		xmlstart+="    <epgtitle>"+getEPGTitle()+"</epgtitle>\n"; //übernommen von trh
		xmlstart+="    <onidsid>123456</onidsid>\n"; // keine ahnung aber wies aussieht wird die sid und die onid nicht gebraucht von ngrab
		xmlstart+="    <epgid>123456</epgid>\n"; // und die epgid auch nicht
		xmlstart+="    <videopid>"+eString().sprintf("%d", Decoder::parms.vpid)+"</videopid>\n";
		xmlstart+="    <audiopids selected=\""+eString().sprintf("%d", Decoder::parms.apid)+"\">\n";
		xmlstart+="       <audio pid=\""+eString().sprintf("%d", Decoder::parms.apid)+"\" name=\"standard\"/>\n";
		xmlstart+="    </audiopids>\n";
		xmlstart+="  </record>\n";
       	xmlstart+=" </neutrino>\n";


#ifdef TMP_NgrabXML
	fstream xmlstartf;
    char* Name0 =TMP_NgrabXML;
    xmlstartf.open(Name0,ios::out);
	xmlstartf<<xmlstart<<endl;
	xmlstartf.close();
		eDebug("Start xml wird abgespeichert");

#endif
return xmlstart;
}

 eString ENgrab::stoppxml()
{
		eDebug("Stop xml wird erstellt");

eString xmlstop;

		xmlstop+="<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		xmlstop+=" <neutrino commandversion=\"2\">\n";
		xmlstop+="   <record command=\"stop\">\n";
		xmlstop+="    <channelname></channelname>\n";
		xmlstop+="    <epgtitle></epgtitle>\n";
		xmlstop+="    <onidsid></onidsid>\n";
		xmlstop+="    <epgid></epgid>\n";
		xmlstop+="    <videopid></videopid>\n";
		xmlstop+="    <audiopids selected=\"\">\n";
		xmlstop+="       <audio pid=\"\" name=\"\"/>\n";
		xmlstop+="    </audiopids>\n";
		xmlstop+="  </record>\n";
        xmlstop+=" </neutrino>\n";


#ifdef TMP_NgrabXML   
	fstream xmlstopf;
    char* Name0 =TMP_NgrabXML;
    xmlstopf.open(Name0,ios::out);
	xmlstopf<<xmlstop<<endl;
	xmlstopf.close();
			eDebug("Stop xml wird abgespeichert");

#endif
	return xmlstop;
}

// es war ein mal ein plugin .... :)
/* void ENgrab::nsetup()
{
hide();
    ENgrabSetup setupN;
	setupN.show();
	setupN.exec();
	setupN.hide();
show();
}
*/
void ENgrab::sendstart()
{
	sending(startxml());
}
void ENgrab::sendstopp()
{
	sending(stoppxml());
}


void ENgrab::sending(eString sxml)
{

		struct in_addr sinet_address;
		eString hostname;
		int port;
		int de[4];

        //hostname="192.168.0.102";// zum testen mein pc!
        //port=4123;// und dessen ngrpa port
	eConfig::getInstance()->getKey("/elitedvb/network/nserver", sinet_address.s_addr);
	eConfig::getInstance()->getKey("/elitedvb/network/nservport", port);
	unpack(sinet_address, de);
	hostname=eString().sprintf("%d.%d.%d.%d", de[0], de[1], de[2], de[3]);
//Hier werden die daten gesendet, es muß nur noch eine überprüfung rein ob es erfolgreich war(tmbinc)
	sd=new eSocket(eApp);
	sd->connectToHost(hostname, port);
	sd->writeBlock(sxml.c_str(), sxml.length());
	sd->close();

}
