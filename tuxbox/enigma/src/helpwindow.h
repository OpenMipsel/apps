#ifndef __helpwindow_h
#define __helpwindow_h

#include <lib/gui/ewindow.h>
#include <lib/gui/ewidget.h>
#include <lib/gui/elabel.h>
#include <lib/gui/statusbar.h>

//class eButton;
//class eCheckbox;

class eHelpWindow: public eWindow
{
		eLabel *label;
		eWidget *scrollbox;
		int scrollypos;
		int scrollmax;
		bool doscroll;
private:
	eString loadHelpText(int helpIDtoLoad);
	int eventHandler(const eWidgetEvent &event);
public:
	eHelpWindow(ePtrList<eAction> &parseActionHelpList, int helpID);
	~eHelpWindow();
};

#endif


