#ifndef __setuposd_h
#define __setuposd_h

#include <lib/gui/ewindow.h>

class eCheckbox;
class eButton;
class eStatusBar;
class eSlider;

class eZapOsdSetup: public eWindow
{
	eSlider *sAlpha, *sBrightness, *sGamma;
	eCheckbox *export_mode, *hide_error_windows;
	eStatusBar *statusbar;

	eButton *pluginoffs, *menu_language, *ok;
	int alpha, brightness, gamma;
private:
	int eventHandler(const eWidgetEvent&);
	void menuLanguagePressed();
	void alphaChanged( int );
	void brightnessChanged( int );
	void gammaChanged( int );
	void pluginPositionPressed();
	void okPressed();
public:
	eZapOsdSetup();
	~eZapOsdSetup();
private:
};

#endif
