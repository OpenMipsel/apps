#ifndef __setup_extra_h
#define __setup_extra_h

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>

class eButton;
class eCheckbox;

class eZapExtraSetup: public eWindow
{
	eButton *ok;
	eCheckbox *profimode, *hideerror, *extZap, *showHelpButtons;
	eStatusBar *statusbar;

	int sprofimode;
	int shideerror;
	int sextzap;
	int shelpbuttons;
private:
	void okPressed();
	void loadSettings();
	void saveSettings();
public:
	eZapExtraSetup();
	~eZapExtraSetup();
};

#endif
