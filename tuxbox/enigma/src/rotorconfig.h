#ifndef __rotorconfig_h
#define __rotorconfig_h

#include <list>

#include <scan.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/combobox.h>
#include <lib/gui/enumber.h>
#include <lib/gui/multipage.h>
#include <lib/gui/listbox.h>
#include <lib/dvb/dvb.h>

class eFEStatusWidget;
class eTransponderWidget;

class RotorConfig: public eWindow, public existNetworks
{
	friend class eLNBSetup;
	eLNB *lnb;
	eListBox<eListBoxEntryText> *positions;
	eLabel *lLatitude, *lLongitude, *lOrbitalPosition, *lStoredRotorNo, *lDirection, *lDegPerSec;
	eNumber *orbital_position, *number, *Latitude, *Longitude, *DegPerSec;
	eButton *add, *remove, *save, *cancel;
	eCheckbox *useGotoXX, *useRotorInPower;
	eComboBox *direction, *LaDirection, *LoDirection;
	eStatusBar* statusbar;
	void onAdd();
	void onRemove();
	void onSavePressed();
	void numSelected(int*);
	void lnbChanged( eListBoxEntryText* );
	void posChanged( eListBoxEntryText* );
	void gotoXXChanged( int );
	void useRotorInPowerChanged( int );
	void setLNBData( eLNB *lnb );
	int eventHandler( const eWidgetEvent& e);
public:
	RotorConfig( eLNB *lnb );
};

#endif
