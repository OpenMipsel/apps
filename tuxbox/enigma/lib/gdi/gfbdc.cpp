#include <lib/gdi/gfbdc.h>

#include <lib/dvb/edvb.h>
#include <lib/system/init.h>
#include <lib/system/econfig.h>

gFBDC *gFBDC::instance;

gFBDC::gFBDC()
{
	instance=this;
	fb = fbClass::getInstance();
	if (!fb->Available())
		eFatal("no framebuffer available");

	fb->SetMode(720, 576, 8);

	eDebug("grabbing primary...");
	pixmap=fb->grabPrimary();
	
	eDebug("is %x", (void*)pixmap);
	reloadSettings();
}

gFBDC::~gFBDC()
{
	delete fb;
	instance=0;
}

void gFBDC::calcRamp()
{
#if 0
	float fgamma=gamma ? gamma : 1;
	fgamma/=10.0;
	fgamma=1/log(fgamma);
	for (int i=0; i<256; i++)
	{
		float raw=i/255.0; // IIH, float.
		float corr=pow(raw, fgamma) * 256.0;

		int d=corr * (float)(256-brightness) / 256 + brightness;
		if (d < 0)
			d=0;
		if (d > 255)
			d=255;
		ramp[i]=d;
		
		rampalpha[i]=i*alpha/256;
	}
#endif
	for (int i=0; i<256; i++)
	{
		int d;
		d=i;
		d=(d-128)*(gamma+64)/(128+64)+128;
		d+=brightness-128; // brightness correction
		if (d<0)
			d=0;
		if (d>255)
			d=255;
		ramp[i]=d;

/*		if ( eDVB::getInstance()->getInfo("mID") == "01" )
			rampalpha[i]=i*alpha/65535;
		else*/
			rampalpha[i]=i*alpha/256;
	}

	rampalpha[255]=255; // transparent BLEIBT bitte so.
}

void gFBDC::exec(gOpcode *o)
{
	switch (o->opcode)
	{
	case gOpcode::setPalette:
	{
		gPixmapDC::exec(o);
		break;
	}
	case gOpcode::flush:
		eDebug("flush!");
		pixmap->Flip(pixmap, 0, DSFLIP_BLIT);
	default:		// (fall through...)
		gPixmapDC::exec(o);
		break;
	}
}

gFBDC *gFBDC::getInstance()
{
	return instance;
}

void gFBDC::setAlpha(int a)
{
	alpha=a;

	calcRamp();
//	setPalette();
}

void gFBDC::setBrightness(int b)
{
	brightness=b;

	calcRamp();
//	setPalette();
}

void gFBDC::setGamma(int g)
{
	gamma=g;

	calcRamp();
//	setPalette();
}

void gFBDC::saveSettings()
{
	eConfig::getInstance()->setKey("/ezap/osd/alpha", alpha);
	eConfig::getInstance()->setKey("/ezap/osd/gamma", gamma);
	eConfig::getInstance()->setKey("/ezap/osd/brightness", brightness);
}

void gFBDC::reloadSettings()
{
	if (eConfig::getInstance()->getKey("/ezap/osd/alpha", alpha))
		alpha=255;
	if (eConfig::getInstance()->getKey("/ezap/osd/gamma", gamma))
		gamma=128;
	if (eConfig::getInstance()->getKey("/ezap/osd/brightness", brightness))
		brightness=128;

	calcRamp();
//	setPalette();
}

eAutoInitP0<gFBDC> init_gFBDC(1, "GFBDC");
