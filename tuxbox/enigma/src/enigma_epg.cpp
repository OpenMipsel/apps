#define DIR_V
#include "enigma_epg.h"
#include <enigma_lcd.h>
#include <lib/dvb/epgcache.h>
#include <lib/dvb/service.h>
#include <lib/dvb/si.h>
#include <lib/dvb/serviceplaylist.h>
#include <lib/gui/eskin.h>
#include <lib/gui/elabel.h>
#include <lib/gui/guiactions.h>
#include <lib/gdi/font.h>
#include "epgactions.h"
#include "timer.h"
#include "enigma_event.h"

gPixmap *eZapEPG::entry::inTimer=0;
gPixmap *eZapEPG::entry::inTimerRec=0;

eZapEPG::eZapEPG(const std::list<eServiceReferenceDVB> &services): 
	eWindow(1), services(services)
{
	timeFont = eSkin::getActive()->queryFont("epg.time");
	titleFont = eSkin::getActive()->queryFont("epg.title");
	descrFont = eSkin::getActive()->queryFont("epg.description");
	entryColor = eSkin::getActive()->queryColor("epg.entry.background");
	entryColorSelected = eSkin::getActive()->queryColor("epg.entry.background.selected");
	entry::inTimer = eSkin::getActive()->queryImage("timer_symbol");
	entry::inTimerRec = eSkin::getActive()->queryImage("timer_rec_symbol");
	addActionMap( &i_epgSelectorActions->map );
	addActionMap( &i_focusActions->map );
	addActionMap( &i_cursorActions->map );
}

eZapEPG::entry::entry(eWidget *parent, gFont &timeFont, gFont &titleFont, 
	gFont &descrFont, gColor entryColor, gColor entryColorSelected): eWidget(parent), timeFont(timeFont),
	titleFont(titleFont), descrFont(descrFont), entryColor(entryColor), 
	entryColorSelected(entryColorSelected)
{
	setBackgroundColor(entryColor);
};

eZapEPG::entry::~entry()
{
	delete event;
}

void eZapEPG::entry::redrawWidget(gPainter *target, const eRect &area)
{
	eString time="";
	tm *begin=start!=-1?localtime(&start):0;
	if (begin)
		time.sprintf("%02d:%02d (%dmin)", begin->tm_hour, begin->tm_min, duration / 60);
	target->setFont(timeFont);
	target->renderText(eRect(0, 0, size.width(), 18), time);

	target->setFont(titleFont);
	target->renderText(eRect(0, 18, size.width(), size.height()-18), title, RS_WRAP);

	ePlaylistEntry* p=0;
	if ( (p = eTimerManager::getInstance()->findEvent( &service->service, (EITEvent*)event )) )
	{
		if ( p->type & ePlaylistEntry::SwitchTimerEntry )
			target->blit( *inTimer, ePoint( size.width()-inTimer->x-1, size.height()-inTimerRec->y-1 ), eRect(), gPixmap::blitAlphaTest);
		else if ( p->type & ePlaylistEntry::RecTimerEntry )
			target->blit( *inTimerRec, ePoint(size.width()-inTimerRec->x-1, size.height()-inTimerRec->y-1), eRect(), gPixmap::blitAlphaTest);
	}

	target->setForegroundColor(gColor(entryColorSelected));
	target->fill(eRect(0, size.height()-1, size.width(), 1));
	target->fill(eRect(size.width()-1, 0, 1, size.height()));
}

void eZapEPG::entry::gotFocus()
{
//	setForegroundColor(focusF,false);
	setBackgroundColor(entryColorSelected);
}

void eZapEPG::entry::lostFocus()
{
//	setForegroundColor(normalF,false);
	setBackgroundColor(entryColor);
}

int eZapEPG::eventHandler(const eWidgetEvent &event)
{
	switch (event.type)
	{
	case eWidgetEvent::evtAction:
	{
		int addtype=-1;
		int servicevalid = current_service != serviceentries.end();
		int eventvalid = 0;
		if (servicevalid)
			if (current_service->current_entry != current_service->entries.end())
				eventvalid = 1;
		if (event.action == &i_epgSelectorActions->addDVRTimerEvent)
			addtype = ePlaylistEntry::RecTimerEntry |
								ePlaylistEntry::recDVR|
								ePlaylistEntry::stateWaiting;
		else if (event.action == &i_epgSelectorActions->addNGRABTimerEvent)
			addtype = ePlaylistEntry::RecTimerEntry|
								ePlaylistEntry::recNgrab|
								ePlaylistEntry::stateWaiting;
		else if (event.action == &i_epgSelectorActions->addSwitchTimerEvent)
			addtype = ePlaylistEntry::SwitchTimerEntry|
								ePlaylistEntry::stateWaiting;
		else if (event.action == &i_epgSelectorActions->removeTimerEvent)
		{
			if (eventvalid)
				if ( eTimerManager::getInstance()->removeEventFromTimerList( this, &current_service->service, current_service->current_entry->event ) )
            current_service->current_entry->invalidate();
		} else if (event.action == &i_focusActions->left)
			selService(-1);
		else if (event.action == &i_focusActions->right)
			selService(+1);
		else if (event.action == &i_focusActions->up)
			selEntry(-1);
		else if (event.action == &i_focusActions->down)
			selEntry(+1);
		else if ((event.action == &i_cursorActions->ok) && eventvalid)
		{
			eService *service=eDVB::getInstance()->settings->getTransponders()->searchService(current_service->service);
			eEventDisplay ei(service ? service->service_name.c_str() : "", current_service->service, 0, (EITEvent*)current_service->current_entry->event);

#ifndef DISABLE_LCD
			eZapLCD* pLCD = eZapLCD::getInstance();
			pLCD->lcdMain->hide();
			pLCD->lcdMenu->show();
			ei.setLCD(pLCD->lcdMenu->Title, pLCD->lcdMenu->Element);
#endif

			hide();
			ei.show();
			int ret;
			while((ret = ei.exec()))
			{
				if (ret == 1)
					selEntry(-1);
				else if (ret == 2)
					selEntry(+1);
				else
					break; // close EventDisplay
	
				ei.setEvent((EITEvent*)current_service->current_entry->event);
			}
			ei.hide();
			show();
		} else
			break;
		if (eventvalid && (addtype != -1))
		{
			if ( !eTimerManager::getInstance()->eventAlreadyInList( this, *(EITEvent*)current_service->current_entry->event, current_service->service) )
			{
				hide();
				eTimerEditView v( *(EITEvent*)current_service->current_entry->event, addtype, current_service->service);
				v.show();
				v.exec();
				v.hide();
				show();
			}
		}
		return 1;
	}
	default:
		break;
	}
	return eWindow::eventHandler(event);
}


void eZapEPG::buildService(serviceentry &service, time_t start, time_t end)
{
#ifdef DIR_V
	int height = service.pos.height();
#else
	int width = service.pos.width();
#endif
	service.entries.setAutoDelete(1);
	eEPGCache *epgcache=eEPGCache::getInstance();
	const timeMap *evmap = epgcache->getTimeMap(service.service);
	if (!evmap)
		return;
	timeMap::const_iterator ibegin = evmap->lower_bound(start);
	if ((ibegin != evmap->end()) && (ibegin != evmap->begin()))
		--ibegin;
		
	timeMap::const_iterator iend = evmap->upper_bound(end);
	if (iend != evmap->end())
		++iend;
	
	for (timeMap::const_iterator event(ibegin); event != iend; ++event)
	{
		const EITEvent *ev = epgcache->lookupEvent(service.service, event->first);
		if (!ev)
			continue;
		if (((ev->start_time+ev->duration)>= start) && (ev->start_time <= end))
		{
			entry *e = new entry(this, timeFont, titleFont, descrFont, entryColor, entryColorSelected);
			e->service = &service;
			e->start = ev->start_time;
			e->duration = ev->duration;
			e->event_id = ev->event_id;
#ifdef DIR_V
			int ypos = (e->start - start) * height / (end - start);
			int eheight = (e->start + e->duration - start) * height / (end - start);
			eheight -= ypos;
			
			if (ypos < 0)
			{
				eheight += ypos;
				ypos = 0;
			}
			
			if ((ypos+eheight) > height)
				eheight = height - ypos;
				
			e->move(ePoint(service.pos.x(), service.pos.y() + ypos));
			e->resize(eSize(service.pos.width(), eheight));
#else
			int xpos = (e->start - start) * width / (end - start);
			int ewidth = (e->start + e->duration - start) * width / (end - start);
			ewidth -= xpos;
			
			if (xpos < 0)
			{
				ewidth += xpos;
				xpos = 0;
			}
			
			if ((xpos+ewidth) > width)
				ewidth = width - xpos;
				
			e->move(ePoint(service.pos.x() + xpos, service.pos.y()));
			e->resize(eSize(ewidth, service.pos.height()));
#endif
			service.entries.push_back(e);
			
			for (ePtrList<Descriptor>::const_iterator d(ev->descriptor); d != ev->descriptor.end(); ++d)
				if (d->Tag()==DESCR_SHORT_EVENT)
				{
					const ShortEventDescriptor *s=(const ShortEventDescriptor*)*d;
					e->title=s->event_name;
					e->description=s->text;
				}
			e->event = ev;
		} else
			delete ev;
	}
}

void eZapEPG::selService(int dir)
{	
	if (serviceentries.begin() == serviceentries.end())
		return;
	int isok;
	ePtrList<entry>::iterator l = current_service->current_entry;
	isok = l != current_service->entries.end();
	if (dir == +1)
	{
		++current_service;
		if (current_service == serviceentries.end())
		{
			--current_service;
			return;
		}
	} else if (dir == -1)
	{
		if (current_service != serviceentries.begin())
			--current_service;
		else
			return;
	}

	time_t last_time=0;

	if (isok)
	{
		l->lostFocus();
		last_time = l->start;
	}
	
	if (current_service->current_entry != current_service->entries.end())
	{
		if (last_time)
		{
			int best_diff=0;
			ePtrList<entry>::iterator best=current_service->entries.end();
			for (ePtrList<entry>::iterator i(current_service->entries.begin()); 
					i != current_service->entries.end(); ++i)
			{
				if ((best == current_service->entries.end()) || abs(i->start-last_time) < best_diff)
				{
					best = i;
					best_diff = abs(i->start-last_time);
				}
			}
			
			if (best != current_service->entries.end())
				current_service->current_entry = best;
		}
		current_service->current_entry->gotFocus();
	}
}

void eZapEPG::selEntry(int dir)
{
	if (current_service == serviceentries.end())
	{
		eDebug("invalid service");
		return;
	}
	if (current_service->entries.begin() == current_service->entries.end())
	{
		eDebug("empty service");
		return;
	}
	ePtrList<entry>::iterator l = current_service->current_entry;
	if (dir == +1)
	{
		++current_service->current_entry;
		if (current_service->current_entry == current_service->entries.end())
		{
			--current_service->current_entry;
			return;
		}
	} else
	{
		if (current_service->current_entry == current_service->entries.begin())
			return;
		--current_service->current_entry;
	}
	if (l != current_service->entries.end())
		l->lostFocus();
	current_service->current_entry->gotFocus();
}

void eZapEPG::buildPage(time_t start, time_t end)
{
	if (start >= end)
		return;

#ifdef DIR_V	
	int height = clientrect.height();
#else
	int width = clientrect.width();
#endif
	int numservices = services.size();
	if (!numservices)
		return;
	
	if (numservices > 10)
		numservices = 10;
#ifdef DIR_V
	int servicewidth = clientrect.width() / numservices;
#else
	int serviceheight = clientrect.height() / numservices;
#endif

	int p = 0;
	
	for (std::list<eServiceReferenceDVB>::const_iterator i(services.begin());
			i != services.end(); ++i, ++p)
	{
		serviceentries.push_back(serviceentry());
		serviceentry &service = serviceentries.back();

		eLabel *header = new eLabel(this);
#ifdef DIR_V
		header->move(ePoint(p * servicewidth, 0));
		header->resize(eSize(servicewidth, 30));
		service.pos = eRect(p * servicewidth, 30, servicewidth, height - 30);
#else
		header->move(ePoint(0, p * serviceheight));
		header->resize(eSize(100, serviceheight));
		service.pos = eRect(100, p * serviceheight, width - 100, serviceheight);
#endif
		
		eService *sv=eServiceInterface::getInstance()->addRef(*i);
		if (sv)
			header->setText(sv->service_name);
		eServiceInterface::getInstance()->removeRef(*i);

		service.service = *i;
		
		buildService(service, start, end);
		service.current_entry = service.entries.begin();
	}
	if (!p)
		return;
	current_service = serviceentries.begin();
	if (current_service == serviceentries.end())
		return;
	if (current_service->current_entry != current_service->entries.end())
		current_service->current_entry->gotFocus();
}
