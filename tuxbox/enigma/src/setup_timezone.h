#ifndef __setuptimezone_h
#define __setuptimezone_h

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>

class eLabel;
class eButton;
class eComboBox;
class eListBoxEntryText;
class eCheckbox;

class eZapTimeZoneSetup: public eWindow
{
	int errLoadTimeZone;
	
	eLabel *ltimeZone;
	eStatusBar* statusbar;
	eComboBox* timeZone;
	eCheckbox *useDst;
	eButton *ok, *abort;
private:
	void okPressed();
	void abortPressed();
	
	int loadTimeZones();
	const char *cmdTimeZones();
public:
	eZapTimeZoneSetup();
	~eZapTimeZoneSetup();
	void setTimeZone();
};

#endif
