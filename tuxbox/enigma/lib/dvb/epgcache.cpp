#include <lib/dvb/epgcache.h>

#include <time.h>
#include <unistd.h>  // for usleep
#include <lib/system/init.h>
#include <lib/dvb/lowlevel/dvb.h>
#include <lib/dvb/si.h>
#include <lib/dvb/service.h>

int eventData::CacheSize=0;

eEPGCache *eEPGCache::instance;

eEPGCache::eEPGCache()
	:paused(0), CleanTimer(eApp), zapTimer(eApp)
{
	eDebug("[EPGC] Initialized EPGCache");
	isRunning=0;

	CONNECT(eDVB::getInstance()->switchedService, eEPGCache::enterService);
	CONNECT(eDVB::getInstance()->leaveService, eEPGCache::abortEPG);
	CONNECT(eDVB::getInstance()->timeUpdated, eEPGCache::timeUpdated);
	CONNECT(zapTimer.timeout, eEPGCache::startEPG);
	CONNECT(CleanTimer.timeout, eEPGCache::cleanLoop);
	instance=this;
}

void eEPGCache::timeUpdated()
{
	CleanTimer.start(CLEAN_INTERVAL);
}

void eEPGCache::SDTReady(int err)
{
	if (!err)
		otherSDTReady=1;
}

int eEPGCache::sectionRead(__u8 *data, int source)
{
	eit_t *eit = (eit_t*) data;
	uniqueEPGKey service( HILO(eit->service_id), HILO(eit->original_network_id) );
	int len=HILO(eit->section_length)-1;//+3-4;
	int ptr=EIT_SIZE;
	eit_event_struct* eit_event = (eit_event_struct*) (data+ptr);
	int eit_event_size;
	int duration;

	time_t TM = parseDVBtime( eit_event->start_time_1, eit_event->start_time_2,	eit_event->start_time_3, eit_event->start_time_4,	eit_event->start_time_5);

	temp[service] = std::pair< time_t, int> (time(0)+eDVB::getInstance()->time_difference, source);

	uniqueEvent event( TM, HILO(eit_event->event_id), service );

	if ( source == SCHEDULE )
	{
		if ( !firstScheduleEvent.valid() )
		{
			if ( (time(0)+eDVB::getInstance()->time_difference) < TM )
				firstScheduleEvent=event;
		}
		else if ( firstScheduleEvent == event )  // epg around
		{
			eDebug("[EPGC] schedule data ready");
			scheduleReader.abort();
			return -1;
		}
	}
	else // if ( source == NOWNEXT )
	{
		if ( !firstNowNextEvent.valid() )
		{
			if ( (time(0)+eDVB::getInstance()->time_difference) < TM )
				firstNowNextEvent = event;
		}
		else if ( firstNowNextEvent == event ) // now next ready
		{
			eDebug("[EPGC] nownext data ready");
			nownextReader.abort();
			return -1;
		}
	}
	while (ptr<len)
	{
		eit_event_size = HILO(eit_event->descriptors_loop_length)+EIT_LOOP_SIZE;

		duration = fromBCD(eit_event->duration_1)*3600+fromBCD(eit_event->duration_2)*60+fromBCD(eit_event->duration_3);

		TM = parseDVBtime( eit_event->start_time_1, eit_event->start_time_2,	eit_event->start_time_3, eit_event->start_time_4,	eit_event->start_time_5);

		if ( (time(0)+eDVB::getInstance()->time_difference) <= (TM+duration) || TM == 3599 /*NVOD Service*/ )  // old events should not be cached
		{
			// hier wird immer eine eventMap zurück gegeben.. entweder eine vorhandene..
			// oder eine durch [] erzeugte
			eventMap &servicemap = eventDB[service];

			// look in 1st descriptor tag.. i hope all time shifted events have the
			// time shifted event descriptor at the begin of the descriptors..
			if ( ((unsigned char*)eit_event)[12] == 0x4F ) // TIME_SHIFTED_EVENT_DESCR
			{
				int sid = ((unsigned char*)eit_event)[14] << 8 | ((unsigned char*)eit_event)[15];
				uniqueEPGKey parent( sid, HILO(eit->original_network_id) );
				std::list<NVODReferenceEntry>::iterator it( NVOD[parent].begin() );
				for ( ; it != NVOD[parent].end(); it++ )
					if ( it->service_id == HILO(eit->service_id) && it->original_network_id == HILO( eit->original_network_id ) )
						break;
				if ( it == NVOD[parent].end() )
/*				{
					eDebug( "add time shifted service" );
					eDebug( "parent sid = %04x, onid = %04x", sid, HILO( eit->original_network_id ) );
					eDebug( "reference sid = %04x, onid = %04x", HILO(eit->service_id), HILO( eit->original_network_id ) );*/
					NVOD[parent].push_back( NVODReferenceEntry( HILO(eit->transport_stream_id), HILO(eit->original_network_id), HILO(eit->service_id) ) );
//				}
			}

			if (TM == 3599)
			{
				TM += HILO(eit_event->event_id);
//				std::list<NVODReferenceEntry> &ref=NVOD[service];
//				eDebug("nvod cached");
			}
/* alt: vorhandene items haben vorrang vor neuen.
				eventMap::iterator It = servicemap.find(TM);

				if (It == servicemap.end())   // event still not cached */

// neu: erst alte events killen...
			eventMap::iterator it = servicemap.find(TM);
			if ( it != servicemap.end() )
				delete it->second;

// ... dann neuen einfuegen.				
			eventDB[service][TM] = new eventData(eit_event, eit_event_size, source);
		}

		ptr += eit_event_size;
		((__u8*)eit_event)+=eit_event_size;
	}
	return 0;
}

bool eEPGCache::finishEPG()
{
	if (!isRunning)  // epg ready
	{
		eDebug("[EPGC] stop caching events");
		zapTimer.start(UPDATE_INTERVAL, 1);
		eDebug("[EPGC] next update in %i min", UPDATE_INTERVAL / 60000);

		tmpMap::iterator It = temp.begin();

		while (It != temp.end())
		{
			if ( It->second.second == SCHEDULE || ( It->second.second == NOWNEXT && !firstScheduleEvent.valid() ) )
				serviceLastUpdated[It->first]=It->second.first;
			if ( eventDB.find( It->first ) == eventDB.end() )
				temp.erase(It++->first);
			else
			{
/*				if ( NVOD.find(It->first) != NVOD.end() ) // look at SDT now
				{
					eServiceHandler *sapi=eServiceInterface::getInstance()->getService();
					if (!sapi)
						break;

					SDT *sdt=sapi->getSDT();
					if (sdt)
					{
						eDebug("SDT found");
						for (ePtrList<SDTEntry>::iterator sdtIt(sdt->entries); sdtIt != sdt->entries.end(); ++sdtIt)
						{
							if (eServiceID(sdtIt->service_id)==It->first.sid)
							{
								eDebug("service in SDT found");
								for (ePtrList<Descriptor>::iterator i(sdtIt->descriptors); i != sdtIt->descriptors.end(); ++i)
								{
									if (i->Tag()==DESCR_NVOD_REF)
									{
										eDebug("NVODRef descr found in SDT... clear existing nvodrefs");
										NVOD[It->first].clear();
										for (ePtrList<NVODReferenceEntry>::iterator e(((NVODReferenceDescriptor*)*i)->entries); e != ((NVODReferenceDescriptor*)*i)->entries.end(); ++e)
										{
											eDebug("add new nvodRef to Service");
											NVOD[It->first].push_back(**e);
										}
									}
								}
							}
						}
						sdt->unlock();
					}
				}*/
				It++;
			}
		}
		if (!eventDB[current_service].empty())
			/*emit*/ EPGAvail(1);

		/*emit*/ EPGUpdated( &temp );

		return true;
	}
	return false;
}

void eEPGCache::cleanLoop()
{
	if (!eventDB.empty() && !paused )
	{
		eDebug("[EPGC] start cleanloop");
		const eit_event_struct* cur_event;
		int duration;
		time_t TM;
		tmpMap temp;		

		for (eventCache::iterator DBIt = eventDB.begin(); DBIt != eventDB.end(); DBIt++)
			for (eventMap::iterator It = DBIt->second.begin(); It != DBIt->second.end();)
			{
				cur_event = (*It->second).get();

				duration = fromBCD( cur_event->duration_1)*3600 + fromBCD(cur_event->duration_2)*60 + fromBCD(cur_event->duration_3);
				TM = parseDVBtime( cur_event->start_time_1, cur_event->start_time_2,cur_event->start_time_3,cur_event->start_time_4,cur_event->start_time_5);
				time_t now = time(0)+eDVB::getInstance()->time_difference;
				if ( now > (TM+duration) )  // outdated entry ?
				{
					// check if NVODService is valid
					if ( It->first - HILO(cur_event->event_id) == 3599 )
					{
//						eDebug("do not delete nvod event");
						It++;
						continue;
					}

					eDebug("[EPGC] delete old event");
					delete It->second;				// release Heap Memory for this entry   (new ....)
					DBIt->second.erase(It);   // remove entry from map
					temp[DBIt->first]=std::pair<time_t, int>(now, NOWNEXT);
					It=DBIt->second.begin();  // start at begin
				}
				else  // valid entry in map
					It=DBIt->second.end();  // ends this clean loop
			}

		if (temp.size())
			/*emit*/ EPGUpdated( &temp );

		eDebug("[EPGC] stop cleanloop");
		eDebug("[EPGC] %i bytes for cache used", eventData::CacheSize);
	}
}

eEPGCache::~eEPGCache()
{
	for (eventCache::iterator evIt = eventDB.begin(); evIt != eventDB.end(); evIt++)
		for (eventMap::iterator It = evIt->second.begin(); It != evIt->second.end(); It++)
			delete It->second;
}

EITEvent *eEPGCache::lookupEvent(const eServiceReferenceDVB &service, int event_id)
{
	uniqueEPGKey key( service.getServiceID().get(), service.getOriginalNetworkID().get() );

	eventCache::iterator It =	eventDB.find( key );
	if ( It

	!= eventDB.end() && !It->second.empty() ) // entry in cache found ?
	{
		for ( eventMap::iterator i( It->second.begin() ); i != It->second.end(); i++)
		{
			if ( i->second->getEventID() == event_id )
				return new EITEvent( *i->second );
		}
	}
	else
		eDebug("no entry in cache found");
	return 0;
}

EITEvent *eEPGCache::lookupEvent(const eServiceReferenceDVB &service, time_t t)
// if t == 0 we search the current event...
{
	EITEvent* e=0;
	uniqueEPGKey key( service.getServiceID().get(), service.getOriginalNetworkID().get() );

	// check if EPG for this service is ready...
	eventCache::iterator It =	eventDB.find( key );
	if ( It != eventDB.end() && !It->second.empty() ) // entry in cache found ?
	{
		if (!t)
			t = time(0)+eDVB::getInstance()->time_difference;

		for ( eventMap::iterator i( It->second.begin() ); i != It->second.end(); i++)
		{
			const eit_event_struct* eit_event = i->second->get();
			int duration = fromBCD(eit_event->duration_1)*3600+fromBCD(eit_event->duration_2)*60+fromBCD(eit_event->duration_3);
			time_t begTime = parseDVBtime( eit_event->start_time_1, eit_event->start_time_2,	eit_event->start_time_3, eit_event->start_time_4,	eit_event->start_time_5);					
			if ( t >= begTime && t <= begTime+duration || service.getServiceType() == 4 ) // the we have found
				return new EITEvent( *i->second );
		}
	}
	return e;
}

void eEPGCache::enterService(const eServiceReferenceDVB &service, int err)
{
	current_service.sid = service.getServiceID().get();
	current_service.onid = service.getOriginalNetworkID().get();

	updateMap::iterator It = serviceLastUpdated.find( current_service );

	int update;

	if (!err || err == -ENOCASYS || err == -ENVOD)
	{
		otherSDTReady=0;
		if (SDTOtherTS)
			delete SDTOtherTS;
		SDTOtherTS = new SDT(SDT::typeOther);
			CONNECT( SDTOtherTS->tableReady, eEPGCache::SDTReady );

		update = ( It != serviceLastUpdated.end() ? ( UPDATE_INTERVAL - ( (time(0)+eDVB::getInstance()->time_difference-It->second) * 1000 ) ) : ZAP_DELAY );

		if (update < ZAP_DELAY)
			update = ZAP_DELAY;

		zapTimer.start(update, 1);
		if (update >= 60000)
			eDebug("[EPGC] next update in %i min", update/60000);
		else if (update >= 1000)
			eDebug("[EPGC] next update in %i sec", update/1000);
	}

	if (It != serviceLastUpdated.end() && !eventDB[current_service].empty())
	{
		eDebug("[EPGC] service has EPG");
		/*emit*/ EPGAvail(1);
	}
	else
	{
		eDebug("[EPGC] service has no EPG");
		/*emit*/ EPGAvail(0);
	}
}

void eEPGCache::pauseEPG()
{
	if (!paused)
	{
		abortEPG();
		eDebug("[EPGC] paused]");
		paused=1;
	}
}

void eEPGCache::restartEPG()
{
	if (paused)
	{
		isRunning=0;
		eDebug("[EPGC] restarted");
		paused--;
		if (paused)
		{
			paused = 0;
			startEPG();   // updateEPG
		}
		cleanLoop();
	}
}

void eEPGCache::startEPG()
{
	if (paused)  // called from the updateTimer during pause...
		paused++;

	if (eDVB::getInstance()->time_difference)	
	{
		SDTOtherTS->start();
		temp.clear();
		eDebug("[EPGC] start caching events");
		firstScheduleEvent.invalidate();
		firstNowNextEvent.invalidate();
		scheduleReader.start();
		isRunning |= 1;
		nownextReader.start();
		isRunning |= 2;
	}
	else
	{
		eDebug("[EPGC] wait for clock update");
		zapTimer.start(1000, 1); // restart Timer
	}
}

void eEPGCache::abortEPG(const eServiceReferenceDVB&)
{
	zapTimer.stop();
	if (isRunning)
	{
	 	if (isRunning & 1)
		{
			isRunning &= ~1;
			scheduleReader.abort();
		}
		if (isRunning & 2)
		{
			isRunning &= ~2;
			nownextReader.abort();
		}
		eDebug("[EPGC] abort caching events !!");
	}
}


eAutoInitP0<eEPGCache> init_eEPGCacheInit(5, "EPG cache");

