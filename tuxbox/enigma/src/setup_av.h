#ifndef __enigma_setup_av_h
#define __enigma_setup_av_h

#include <lib/gui/listbox.h>

class eZapAVSetup: public eListBoxWindow<eListBoxEntryMenu>
{
private:
	void sel_video();
	void sel_test();
#ifdef ENABLE_RFMOD
	void sel_rfmod();
#endif
	void sel_sound();
protected:
	int eventHandler(const eWidgetEvent &event);
	void sel_num(int n);
public:
	eZapAVSetup();
};

class eZapSetupSelectN
{
	int n;
public:
	eZapSetupSelectN(int n): n(n) { }
	bool operator()(eListBoxEntryMenu &e)
	{
		if (!n--)
		{
			e.selected();
			return 1;
		}
		return 0;
	}
};

#endif /* __enigma_setup_av_h */
