#ifndef __setuprfmod_h
#define __setuprfmod_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>

class eNumber;
class eButton;
class eCheckbox;

class eZapRFmodSetup: public eWindow
{
	eButton *abort, *ok;
	eStatusBar *status;

private:
	void okPressed();
	void abortPressed();

public:
	eZapRFmodSetup();
	~eZapRFmodSetup();
};

#endif
