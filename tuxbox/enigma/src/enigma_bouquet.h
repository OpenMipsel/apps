#ifndef __setup_bouquet_h
#define __setup_bouquet_h

#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>

class eZapBouquetSetup: public eListBoxWindow<eListBoxEntryMenu>
{
	void editSelected();
	void createNewEmptyBouquet();
	void editModeSelected();
public:
	eZapBouquetSetup();
};

#endif //__setup_bouquet_h
