#ifndef __src_enigma_epg
#define __src_enigma_epg

#include <lib/gui/ewidget.h>
#include <lib/gui/ewindow.h>
#include <lib/dvb/dvb.h>

class EITEvent;

class eZapEPG: public eWindow
{
	gFont timeFont, titleFont, descrFont;
	gColor entryColor, entryColorSelected;
	struct serviceentry;
	struct entry: public eWidget
	{
		void redrawWidget(gPainter *target, const eRect &area);
		gFont &timeFont, &titleFont, &descrFont;
		gColor entryColor, entryColorSelected;
		void gotFocus();
		void lostFocus();
	public:
		struct serviceentry *service;
		static gPixmap *inTimer;
		static gPixmap *inTimerRec;
		time_t start;
		int duration;
		int event_id;
		eString title, description;
		void setActive(int active);
		const EITEvent *event;
		entry(eWidget *parent, gFont &timeFont, gFont &titleFont, gFont &descrFont, gColor entryColor, gColor entryColorSelected);
		~entry();
	};
	
	struct serviceentry
	{
		eRect pos;
		eServiceReferenceDVB service;
		ePtrList<entry> entries;
		ePtrList<entry>::iterator current_entry;
		serviceentry() : current_entry(entries.end()) { }
	};
	std::list<eServiceReferenceDVB> services;
	std::list<serviceentry> serviceentries;
	std::list<serviceentry>::iterator current_service;
	int eventHandler(const eWidgetEvent &event);
	void buildService(serviceentry &service, time_t start, time_t end);
	void selService(int dir);
	void selEntry(int dir);
public:
	eZapEPG(const std::list<eServiceReferenceDVB> &services);
	void buildPage(time_t start, time_t end);
};

#endif
