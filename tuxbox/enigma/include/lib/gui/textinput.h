#ifndef __LIB_GUI_TEXTINPUT_H__
#define __LIB_GUI_TEXTINPUT_H__

#include <lib/gui/ebutton.h>

class eTextInputField: public eButton
{
	unsigned int curPos, maxChars;
	int eventHandler( const eWidgetEvent &);
	void redrawWidget( gPainter *target, const eRect &area );
	void drawCursor();
	void updated();
	void nextChar();
	int lastKey;
	bool editMode;
	eString oldText;
	eString oldHelpText;
	eTimer nextCharTimer;
	eString useableChars;
	unsigned int nextCharTimeout;
	bool capslock;
	eString isotext;
public:
	eTextInputField( eWidget* parent, eLabel *descr=0, const char *deco="eNumber" );
	void setMaxChars( int i ) { maxChars = i; }
	void setUseableChars( const char* );
	void setNextCharTimeout( unsigned int );
};

#endif
