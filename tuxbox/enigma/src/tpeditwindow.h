#ifndef __SRC_TPEDITWINDOW_H_
#define __SRC_TPEDITWINDOW_H_

#include <lib/gui/listbox.h>

class eButton;
class eTransponder;
class eTransponderWidget;

class eListBoxEntryTransponder: public eListBoxEntry
{
	friend class eListBox<eListBoxEntryTransponder>;
	eTransponder *tp;
	const eString &redraw(gPainter *, const eRect&, gColor, gColor, gColor, gColor, int);
	static gFont font;
public:
	bool operator < ( const eListBoxEntry& e )const;
	static int getEntryHeight();
	eListBoxEntryTransponder( eListBox<eListBoxEntryTransponder>*, eTransponder* );
	eTransponder *getTransponder() { return tp; }
};

class eTransponderEditWindow: public eWindow
{
	eListBox<eListBoxEntryText> *satellites;
	eListBox<eListBoxEntryTransponder> *transponders;
	eButton *sat, *add, *edit, *remove;
	int changed;
public:
	eTransponderEditWindow();
	~eTransponderEditWindow();
	void satSelChanged( eListBoxEntryText* );
	void addPressed();
	void editPressed();
	void removePressed();
	void focusChanged( const eWidget* w );
	int eventHandler( const eWidgetEvent & e );
};

class eTPEditDialog: public eWindow
{
	eTransponderWidget *tpWidget;
	eButton *save, *cancel;
	eTransponder *tp;
	void savePressed();
public:
	eTPEditDialog( eTransponder *tp );
};

#endif // __SRC_TPEDITWINDOW_H_
