#ifndef __SRC_PARENTALLOCK_H_
#define __SRC_PARENTALLOCK_H_

#include <lib/gui/ewindow.h>
#include <lib/gui/statusbar.h>

class eButton;
class eCheckbox;
class eNumber;

bool checkPin( int pin, const char * text );

class eParentalSetup: public eWindow
{
	eButton *ok;
	eCheckbox *parentallock, *setuplock;
	eButton *changeParentalPin, *changeSetupPin, *lockunlock;
	eStatusBar *statusbar;

	int sparentallock;
	int ssetuplock;

	int parentalpin, setuppin;
private:
	void okPressed();
	void loadSettings();
	void saveSettings();
	void slockChecked(int);
	void plockChecked(int);
	void changePin( eButton* );
	void lockunlockPressed();
public:
	eParentalSetup();
	~eParentalSetup();
};

class ParentalLockWindow:public eWindow
{
	eLabel *lPin;
	eNumber *nPin;
public:
	ParentalLockWindow( const char *, int );
	void numEntered(int *i);
};

#endif // __SRC_PARENTALLOCK_H_
