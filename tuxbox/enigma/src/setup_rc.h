#ifndef __setuprc_h
#define __setuprc_h

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>

class eLabel;
class eButton;
class eSlider;
class eComboBox;
class eListBoxEntryText;
class eCheckbox;

class eZapRCSetup: public eWindow
{
	eSlider *srrate, *srdelay;
	eLabel *lrrate, *lrdelay, *lrcStyle;
	eStatusBar* statusbar;
	eCheckbox* sselect_style;
	int sselect_style_val;
	eComboBox* rcStyle;
	eString curstyle;

	int rdelay;
	int rrate;
	                	
	eButton *ok, *abort;
	void okPressed();
	void abortPressed();
	void repeatChanged( int );
	void delayChanged( int );
	void styleChanged( eListBoxEntryText* );
	void update();
	void setStyle();
public:
	eZapRCSetup();
	~eZapRCSetup();
private:
};

#endif
