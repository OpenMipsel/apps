#ifndef __src_lib_dvb_dvbwidgets_h
#define __src_lib_dvb_dvbwidgets_h

#include <lib/gui/ewidget.h>
#include <lib/gui/listbox.h>
#include <lib/base/ebase.h>

class eNumber;
class eTransponder;
class eCheckbox;
class eProgress;
class eFrontend;
class eComboBox;

class eTransponderWidget: public eWidget
{
	eNumber *frequency, *symbolrate;
	eCheckbox *inversion;
	int type, edit;
	eListBoxEntryText *fecEntry[6], *polarityEntry[4];
	
	eListBox<eListBoxEntryText> *fec, *polarity;
	eComboBox *sat;
	void nextField0(int *);
	void updated1(eListBoxEntryText *);
	void updated2(int);
public:
	enum type
	{
		deliveryCable, deliverySatellite
	};
	Signal0<void> updated;
	eTransponderWidget(eWidget *parent, int edit, int type);
	int load();
	int setTransponder(const eTransponder *transponder);
	int getTransponder(eTransponder *transponder);
};

class eFEStatusWidget: public eWidget
{
	eProgress *p_snr, *p_agc;
	eCheckbox *c_sync, *c_lock;
	eFrontend *fe;
	eTimer updatetimer;
	void update();
	int eventHandler(const eWidgetEvent &);
public:
	eFEStatusWidget(eWidget *parent, eFrontend *fe);
};

#endif
