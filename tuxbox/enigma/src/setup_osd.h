#ifndef __setuposd_h
#define __setuposd_h

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/slider.h>

class eCheckbox;
class eButton;

class eZapOsdSetup: public eWindow
{
	eSlider *sAlpha, *sBrightness, *sGamma;
	eCheckbox* showOSDOnEITUpdate;
	eCheckbox* showConsoleOnFB;
	eCheckbox* showCurrentRemaining;
	eCheckbox* anableAutohideOSDOn;
	eStatusBar* statusbar;

	eButton *ok, *pluginoffs;
	int alpha, brightness, gamma;
private:
	int eventHandler(const eWidgetEvent&);
	void consoleStateChanged( int );
	void alphaChanged( int );
	void brightnessChanged( int );
	void gammaChanged( int );
	void fieldSelected(int *number);
	void okPressed();
	void PluginOffsetPressed();
public:
	eZapOsdSetup();
	~eZapOsdSetup();
private:
};

#endif
