#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <memory.h>
#include <linux/kd.h>
#include <argp.h>

#include <lib/system/init.h>

#include <directfb.h>

#include <lib/system/econfig.h>
#include <lib/gdi/fb.h>

fbClass *fbClass::instance;

IDirectFB *DirectFB;

fbClass *fbClass::getInstance()
{
	return instance;
}

extern char **__argv;
extern int __argc;

fbClass::fbClass()
{
	instance=this;
	locked=0;
	available=0;
	
	if (DirectFBInit(&__argc, &__argv) != DFB_OK) // todo: argc, argv
	{
		eWarning("DirectFBInit failed!");
		goto nolfb;
	}
	
	if (DirectFBCreate(&DirectFB) != DFB_OK)
	{
		eWarning("DirectFBCreate failed!");
		goto nolfb;
	}
	
	DirectFB->SetCooperativeLevel (DirectFB, DFSCL_FULLSCREEN);
	
#if 0	
	int state=0;
	eConfig::getInstance()->getKey("/ezap/osd/showConsoleOnFB", state);

	fd=open(fb, O_RDWR);
	if (fd<0)
	{
		perror(fb);
		goto nolfb;
	}
	if (ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo)<0)
	{
		perror("FBIOGET_VSCREENINFO");
		goto nolfb;
	}
	
	showConsole(state);
#endif

	available=1;
	
	eDebug("successfully setup directfb mode..");

	return;
nolfb:
	printf("framebuffer not available.\n");
}

#if 0
int fbClass::showConsole(int state)
{
	int fd=open("/dev/vc/0", O_RDWR);
	if(fd>=0)
	{
		if(ioctl(fd, KDSETMODE, state?KD_TEXT:KD_GRAPHICS)<0)
		{
			eDebug("setting /dev/vc/0 status failed.");
		}
		close(fd);
	}
	return 0;
}
#endif

int fbClass::SetMode(unsigned int nxRes, unsigned int nyRes, unsigned int nbpp)
{
	if (DirectFB->SetVideoMode(DirectFB, nxRes, nyRes, nbpp))
	{
		eWarning("DirectFB: couldn't set video mode!");
		return -1;
	}
	
	DFBSurfaceDescription dsc;
	
	if (primary)
		primary->Release(primary);

	dsc.flags = DSDESC_CAPS;
	dsc.caps  = DFBSurfaceCapabilities(DSCAPS_PRIMARY | DSCAPS_FLIPPING );

	if (DirectFB->CreateSurface( DirectFB, &dsc, &primary ) != DFB_OK)
	{
		eWarning("dfb->CreateSurface failed!");
		return -1;
	}
	
	eDebug("CreateSurface success!");
	eDebug("get size .. %x", primary->GetSize);

	return 0;
}

fbClass::~fbClass()
{
	if (primary)
		primary->Release(primary);
}

int fbClass::lock()
{
	eDebug("FRAMEBUFFER LOCK .. nye");
	return -1;
}

void fbClass::unlock()
{
	eDebug("FRAMEBUFFER UNLOCK .. nye");
}

eAutoInitP0<fbClass> init_fbClass(0, "DirectFB");
