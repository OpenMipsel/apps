#ifndef __setup_extra_h
#define __setup_extra_h

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>

class eButton;
class eCheckbox;

class eZapExtraSetup: public eWindow
{
	eButton *ok, *abort;
	eCheckbox *profimode, *hideerror;
	eStatusBar *statusbar;

	int sprofimode;
	int shideerror;
private:
	void okPressed();
	void abortPressed();
	void loadSettings();
	void saveSettings();
public:
	eZapExtraSetup();
	~eZapExtraSetup();
};

#endif
