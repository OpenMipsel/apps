#ifndef __enigma_setup_h
#define __enigma_setup_h

#include <lib/gui/listbox.h>

class eZapSetup: public eListBoxWindow<eListBoxEntryMenu>
{
private:
	void sel_close();
	void sel_channels();	
#ifndef DISABLE_NETWORK
	void sel_network();
#endif
	void sel_setup_av();
	void sel_language();
	void sel_timezone();
	void sel_timeCorrection();
	void sel_skin();
	void sel_osd();
#ifndef DISABLE_LCD
	void sel_lcd();
#endif
	void sel_rc();
#ifndef DISABLE_FILE
	void sel_harddisk();
#endif
	void sel_ci();
	void sel_upgrade();
	void sel_num(int n);
#ifndef DISABLE_NETWORK
	void sel_engrab();
#endif
	void sel_extra();
	void sel_parental();
	void sel_test();
protected:
	int eventHandler(const eWidgetEvent &event);
public:
	static Signal1<void,eZapSetup*> setupHook;
	eZapSetup();
	~eZapSetup();
};

#endif /* __enigma_setup_h */
