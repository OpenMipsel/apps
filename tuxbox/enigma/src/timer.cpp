#include <timer.h>
#include <engrab.h>
#include <enigma_main.h>
#include <lib/system/init.h>
#include <lib/system/init_num.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/servicestructure.h>
#include <lib/gui/emessage.h>
#include <lib/gdi/font.h>
#include <lib/gui/textinput.h>
#include <lib/gui/enumber.h>
#include <lib/gui/combobox.h>
#include <lib/gui/echeckbox.h>
#include <engrab.h>

static const unsigned char monthdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const char *monthStr[12] = { _("January"), _("February"), _("March"),
													_("April"), _("May"), _("June"),	_("July"),
													_("August"), _("September"), _("October"),
													_("November"), _("December") };
static const char *dayStr[7] = { _("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"),
											 _("Thursday"), _("Friday"), _("Saturday") };
const char *dayStrShort[7] = { _("Sun"), _("Mon"), _("Tue"), _("Wed"),
											 _("Thu"), _("Fri"), _("Sat") };

eTimerManager* eTimerManager::instance=0;

void normalize( struct tm & );

time_t getNextEventStartTime( time_t t, int type, bool notToday )
{
	if ( type < ePlaylistEntry::typeMultiple )
		return 0;

	time_t now = time(0)+eDVB::getInstance()->time_difference;
	tm tmp = *localtime( &now ),  // now
		tmp2 = *localtime( &t );    // activation hour:min

	if ( notToday )  // add one day.. and normalize struct tm
	{
		tmp.tm_mday++;
		normalize(tmp);
		time_t b=mktime(&tmp);
		tmp = *localtime(&b);
	}

	bool found=false;

	int i = ePlaylistEntry::Su;
	for ( int x = 0; x < tmp.tm_wday; x++ )
		i*=2;
	
	for (; i <= ePlaylistEntry::Sa; i*=2 )
	{
		if ( type & i ) // next day found for this event
		{
			found=true;
			break;
		}
		tmp.tm_mday++;
	}

	if ( !found )
	{
		tmp.tm_mday--;
		for ( int i = ePlaylistEntry::Su; i <= ePlaylistEntry::Sa; i*=2 )
		{
			if ( type & i ) // next day found for this event
			{
				found=true;
				break;
			}
			tmp.tm_mday++;
		}
	}
	if ( !found )  // No day(s) selected
		return 0;
	tmp.tm_hour = tmp2.tm_hour;
	tmp.tm_min = tmp2.tm_min;
	tmp.tm_sec = tmp2.tm_sec;
	normalize( tmp );
	return mktime(&tmp);
}

static eString getRight( const eString& str, char c )
{
	unsigned int found = str.find(c);
	unsigned int beg = ( found != eString::npos ? found : 0 );
	unsigned int len = str.length();
	if ( found != eString::npos )
		beg++;
	return str.mid( beg, len-beg );
}

time_t getDate()
{
	static time_t tmp = time(0)+eDVB::getInstance()->time_difference;
	tm *now = localtime(&tmp);
	now->tm_min=0;
	now->tm_hour=0;
	now->tm_sec=0;
	return mktime(now);
}

static eString getLeft( const eString& str, char c )
{
	unsigned int found = str.find(c);
	return found != eString::npos ? str.left(found):str;
}

const eString& getEventDescrFromEPGCache( const eServiceReference &_ref, time_t time )
{
	static eString descr;
	descr="";
	const eServiceReferenceDVB &ref = (eServiceReferenceDVB&)_ref;
	// parse EPGCache to get event informations
	EITEvent *tmp = eEPGCache::getInstance()->lookupEvent( ref, time );
	if (tmp)
	{
		for (ePtrList<Descriptor>::const_iterator d(tmp->descriptor); d != tmp->descriptor.end(); ++d)
		{
			if ( d->Tag() == DESCR_SHORT_EVENT)
			{
				ShortEventDescriptor *s=(ShortEventDescriptor*)*d;
				descr=s->event_name;
				if ((s->text.length() > 0) && (s->text!=descr))
					descr+=" - "+s->text;
				break;
			}
		}
		delete tmp;
	}
	return descr;
}

eTimerManager::eTimerManager()
	:actionTimer(eApp), timer(eApp)
{
	if (!instance)
		instance = this;

	eServicePlaylistHandler::getInstance()->addNum( 5 );
	timerlistref=eServiceReference( eServicePlaylistHandler::ID, eServiceReference::flagDirectory, 0, 5);
	timerlist=(ePlaylist*)eServiceInterface::getInstance()->addRef(timerlistref);
	ASSERT(timerlist);
	timerlist->service_name=_("Timerlist");
	timerlist->load(CONFIGDIR "/enigma/timer.epl");
	CONNECT( actionTimer.timeout, eTimerManager::actionHandler );
	conn = CONNECT( timer.timeout, eTimerManager::waitClock );
	waitClock();
}

// called only once... at start of eTimerManager
void eTimerManager::waitClock()
{
	if (eDVB::getInstance()->time_difference)	
	{
		eDebug("[eTimerManager] timeUpdated");	
		nextAction = setNextEvent;
		actionTimer.start(0, true);
	}
	else
	{
		eDebug("[eTimerManager] wait for clock update");
		timer.start(1000, true);  // next check in 1 sec
	}
}

void eTimerManager::loadTimerList()
{
	timerlist->load(CONFIGDIR "/enigma/timer.epl");
	if ( !(nextStartingEvent->type & ePlaylistEntry::stateRunning) )
	{
		nextAction = setNextEvent;
		actionTimer.start(0,true);
	}
}

void eTimerManager::saveTimerList()
{
	timerlist->save();
}

void eTimerManager::actionHandler()
{
	switch( nextAction )
	{
		case zap:
			eDebug("[eTimerManager] zapToChannel");
			if ( eServiceInterface::getInstance()->service != nextStartingEvent->service )
			{
				eDebug("[eTimerManager] change to the right service");
				conn = CONNECT( eDVB::getInstance()->switchedService, eTimerManager::switchedService );
				eString save = nextStartingEvent->service.descr;
				nextStartingEvent->service.descr = getLeft( nextStartingEvent->service.descr, '/' );
				eZapMain::getInstance()->playService( nextStartingEvent->service, 0 );
				nextStartingEvent->service.descr=save;
			}
			else
			{
				eDebug("[eTimerManager] we are always on the right service... do not change service");
				nextAction=startCountdown;
				actionTimer.start(0, true);
			}
			break;

		case showMessage:
			eDebug("[eTimerManager] showMessage");
			long t;
			if ( (t = getSecondsToBegin()) ) // event is not running
			{
				nextAction=zap;
				actionTimer.start(60000, true ); // restart timer
				eDebug("[eTimerManager] event starts in 6 min");
				// here we can show a messagebox... event begin in bla minutes... ok.. abort...
				// messagebox timout 1 min..
			}
		/*	else
			{
				conn = CONNECT( timer.timeout, eTimerManager::stopEvent );
				int t = getTimeout();
			}*/
			break;

		case startCountdown:
			eDebug("[eTimerManager] startCountdown");
			eZapMain::getInstance()->toggleTimerMode();
			// now in eZapMain the RemoteControl should be handled for TimerMode...
			// an service change now stop the Running Event and set it to userAborted
			if ( conn.connected() )
				conn.disconnect();
			conn = CONNECT( eDVB::getInstance()->leaveService, eTimerManager::leaveService );
			if ( nextStartingEvent->type & ePlaylistEntry::typeSmartTimer )
			{
				conn2 = CONNECT( eDVB::getInstance()->tEIT.tableReady, eTimerManager::EITready );
				EITready(0);  // check current eit now !
			}
			else
			{
				long t = getSecondsToBegin();
				nextAction=startEvent;
				if ( nextStartingEvent->type & ePlaylistEntry::RecTimerEntry )
					t-=10;  // 10 seconds for HDD Wakeup
				actionTimer.start( t*1000 , true );
			}
			break;

		case startEvent:
			eDebug("[eTimerManager] startEvent");
			if ( nextStartingEvent->type & ePlaylistEntry::typeMultiple )
			{
				nextStartingEvent->type &= ~(
					ePlaylistEntry::stateError |
					ePlaylistEntry::stateFinished |
					ePlaylistEntry::errorNoSpaceLeft |
					ePlaylistEntry::errorUserAborted |
					ePlaylistEntry::errorZapFailed );
				nextStartingEvent->last_activation = getDate();
			}
			else
				nextStartingEvent->type &= ~ePlaylistEntry::stateWaiting;

			nextStartingEvent->type |= ePlaylistEntry::stateRunning;

			if (nextStartingEvent->type & ePlaylistEntry::typeShutOffTimer)
			{
				eDebug("event has already began");
			}
			else if (nextStartingEvent->type & ePlaylistEntry::RecTimerEntry)
			{
				nextAction = startRecording;
				actionHandler();
			}
			else if (nextStartingEvent->type & ePlaylistEntry::SwitchTimerEntry)
			{
				eZapMain::getInstance()->handleStandby();
			}

			if ( !(nextStartingEvent->type & ePlaylistEntry::typeSmartTimer) )
			{
				nextAction = stopEvent;
				actionTimer.start( getSecondsToEnd() * 1000, true );
			}
			break;

		case pauseEvent:
			eDebug("[eTimerManager] pauseEvent");
			if ( nextStartingEvent->type & ePlaylistEntry::RecTimerEntry )
			{
				nextStartingEvent->type &= ~ePlaylistEntry::stateRunning;
				nextStartingEvent->type |= ePlaylistEntry::statePaused;
				nextAction = pauseRecording;
				actionHandler();
			}
			break;

		case restartEvent:
			eDebug("[eTimerManager] restartEvent");
			if ( nextStartingEvent->type & ePlaylistEntry::RecTimerEntry )
			{
				nextStartingEvent->type &= ~ePlaylistEntry::statePaused; // we delete current state...
				nextStartingEvent->type |= ePlaylistEntry::stateRunning;
				nextAction=restartRecording;
				actionHandler();
			}
			break;

		case stopEvent:
			eDebug("[eTimerManager] stopEvent");
			if( nextStartingEvent->type & ePlaylistEntry::stateRunning )
			{
				nextStartingEvent->type &= ~ePlaylistEntry::stateRunning;
				// when no ErrorCode is set the we set the state to finished
				if ( !(nextStartingEvent->type & ePlaylistEntry::stateError) )
				{
					nextStartingEvent->type |= ePlaylistEntry::stateFinished;
				}
				if ( nextStartingEvent->type & ePlaylistEntry::RecTimerEntry )
				{
					nextAction=stopRecording;
					actionHandler();
				}
				else // SwitchTimer
				{
					eZapMain::getInstance()->handleStandby();
				}
				eZapMain::getInstance()->toggleTimerMode();
			}
			nextAction=setNextEvent;	// we set the Next Event... a new beginning *g*
			actionTimer.start(0, true);
			break;

		case setNextEvent:
		{
			eDebug("[eTimerManager] setNextEvent");
			if (conn.connected() )
				conn.disconnect();
			if (conn2.connected() )
				conn2.disconnect();
			nextStartingEvent=timerlist->getList().end();
			int timeToNextEvent=INT_MAX, count=0;
			// parse events... invalidate old, set nextEvent Timer
			for ( std::list< ePlaylistEntry >::iterator i(timerlist->getList().begin()); i != timerlist->getList().end(); )
			{
				time_t nowTime=time(0)+eDVB::getInstance()->time_difference;
				if ( i->type & ePlaylistEntry::typeMultiple )
				{
					time_t tmp = getNextEventStartTime( i->time_begin, i->type, getDate() == i->last_activation );
					if ( tmp-nowTime < timeToNextEvent )
					{
						nextStartingEvent=i;
						timeToNextEvent = tmp-nowTime;
						count++;
					}
				}
				else if ( i->type & ePlaylistEntry::stateWaiting )
				{
					if ( i->type & ePlaylistEntry::typeShutOffTimer)
					{
						nextStartingEvent=i;
						timeToNextEvent = i->time_begin - nowTime;
						break;
					}
					else if ( i->type & ePlaylistEntry::stateError )
					{
						i->type &= ~ePlaylistEntry::stateWaiting;
					}
					else if ( i->time_begin+i->duration < nowTime ) // old event found
					{
						i->type &= ~ePlaylistEntry::stateWaiting;
						i->type |= (ePlaylistEntry::stateError|ePlaylistEntry::errorOutdated);
					}
					else if( (i->time_begin - nowTime) < timeToNextEvent )
					{
						nextStartingEvent=i;
						timeToNextEvent = i->time_begin - nowTime;
						count++;
					}
					else
						count++;
				}
				else if ( i->type & ePlaylistEntry::typeShutOffTimer )
				{
					i = timerlist->getList().erase(i);  // alten ShutOffTimer aus liste entfernen...
					continue;
				}
				i++;
			}
			eDebug("[eTimerManager] updated ( %d waiting events in list )", count );
			timerlist->save();
			if ( nextStartingEvent != timerlist->getList().end() )
			{
				tm* evtTime=0;
				if ( nextStartingEvent->type & ePlaylistEntry::typeMultiple )
				{
					time_t tmp = getNextEventStartTime( nextStartingEvent->time_begin, nextStartingEvent->type, getDate() == nextStartingEvent->last_activation );
					if ( tmp )
						evtTime = localtime( &tmp );
				}
				else
					evtTime = localtime( &nextStartingEvent->time_begin );
				eDebug("[eTimerManager] next event starts at %02d.%02d, %02d:%02d", evtTime->tm_mday, evtTime->tm_mon+1, evtTime->tm_hour, evtTime->tm_min );
				long t = getSecondsToBegin();
				if ( nextStartingEvent->type & ePlaylistEntry::RecTimerEntry )
					t -= 40; // we zap 40 sec before... ( for EPGCache... )
				int prepareTime = 0;
				if ( nextStartingEvent->type & ePlaylistEntry::typeSmartTimer )
				{  // EIT related zapping ....
					if ( t > prepareTime )
					{
						nextAction=showMessage;
						actionTimer.start( (t - 360) * 1000, true ); // set the Timer to eventBegin - 6 min
					}
					else  // time to begin is under 6 min or the event is currently running
					{
						nextAction=zap;
						actionHandler();
					}
				}
				else
				{
					nextAction=zap;
					// set the Timer to eventBegin
					actionTimer.start( t * 1000, true );
				}
			}
		}
		break;

		case startRecording:
			eDebug("[eTimerManager] start recording");
			if (nextStartingEvent->type & ePlaylistEntry::recDVR)
			{
//				eDebug("nextStartingEvent->service.data[0] = %d", nextStartingEvent->service.data[0] );
//				eDebug("nextStartingEvent->service.descr = %s", nextStartingEvent->service.descr.c_str() );
				eString recordDescr;
				if ( nextStartingEvent->type & ePlaylistEntry::typeMultiple )
				{
					time_t tmp = getNextEventStartTime( nextStartingEvent->time_begin, nextStartingEvent->type, false );
					const eString &descr=getEventDescrFromEPGCache( nextStartingEvent->service, tmp+nextStartingEvent->duration/2 );
					if ( descr ) // build Episode Information
					{
						unsigned int npos = descr.find(" - ");
						if ( npos != eString::npos )
							recordDescr = descr.mid( npos );
					}
					if ( !recordDescr )
					{  // build date instead of epoisode information
						tm *evtTime = localtime(&tmp);
						recordDescr.sprintf(" - %02d.%02d.%02d,", evtTime->tm_mday, evtTime->tm_mon+1, evtTime->tm_year%100 );
					}
				}
				eString descr = getRight( nextStartingEvent->service.descr, '/');
				if ( recordDescr )
					descr += recordDescr;
				eZapMain::getInstance()->recordDVR(1, 0, descr.length()?descr.c_str():0 );
			}
			else if (nextStartingEvent->type & ePlaylistEntry::recNgrab)
			{
				eDebug("Starte Ngrab aufnahme");
				ENgrab::getNew()->sendstart();
			}
			else
			{
				// insert lirc ( VCR start ) here
			}
			break;

		case stopRecording:
			eDebug("[eTimerManager] stop recording");
			if (nextStartingEvent->type & ePlaylistEntry::recDVR)
			{
				eZapMain::getInstance()->recordDVR(0, 0);
			}
			else if (nextStartingEvent->type & ePlaylistEntry::recNgrab)
			{
				eDebug("Stope Ngrab aufnahme");
				ENgrab::getNew()->sendstop();
			}
			else  // insert lirc ( VCR stop ) here
			{
			}
			break;

		case restartRecording:
			eDebug("[eTimerManager] restart recording");
			if (nextStartingEvent->type & ePlaylistEntry::recDVR)
			{
				eServiceHandler *handler=eServiceInterface::getInstance()->getService();
				if (!handler)
					eFatal("no service Handler");
				handler->serviceCommand(eServiceCommand(eServiceCommand::cmdRecordStart));
			}
			else // insert lirc ( VCR START )
			{

			}
			eDebug("ok, recording...");
			break;

		case pauseRecording:
			if (nextStartingEvent->type & ePlaylistEntry::recDVR)
			{
				eServiceHandler *handler=eServiceInterface::getInstance()->getService();
				if (!handler)
					eFatal("no service Handler");
				handler->serviceCommand(eServiceCommand(eServiceCommand::cmdRecordStop));
			}
			else // insert lirc ( VCR PAUSE )
			{

			}
			break;

		default:
			eDebug("unhandled timer action");
	}
}

void eTimerManager::switchedService( const eServiceReferenceDVB &ref, int err)
{
	if ( err == -ENOSTREAM || nextStartingEvent->service != (eServiceReference&)ref )
	{
		abortEvent( ePlaylistEntry::errorZapFailed );
	}
	else  // zap okay
	{
		nextAction=startCountdown;
		actionTimer.start(0,true);
	}
}

void eTimerManager::abortEvent( int err )
{
	eDebug("[eTimerManager] abortEvent");
	nextAction=stopEvent;
	nextStartingEvent->type |= (ePlaylistEntry::stateError|err);
	actionHandler();
}

void eTimerManager::leaveService( const eServiceReferenceDVB& ref )
{
	eDebug("[eTimerManager] leaveService");
	abortEvent( ePlaylistEntry::errorUserAborted );
}

void eTimerManager::EITready( int error )
{
	eDebug("[eTimerManager] EITready %s", strerror(-error));
	if (!error)
	{
		EIT *eit = eDVB::getInstance()->getEIT();
		if ( eit )
		{
			for (ePtrList<EITEvent>::const_iterator event(eit->events); event != eit->events.end(); ++event)		// always take the first one
			{
				if ( nextStartingEvent != timerlist->getList().end() && event->event_id == nextStartingEvent->event_id )
				{
					eDebugNoNewLine("running_status(%d) = ", event->running_status );
					switch( event->running_status )
					{
						case 0:
							eDebug("undefined");
							// premiere world sends either undefined or running
						case 1:
							eDebug("not running");
							if ( nextStartingEvent->type & ePlaylistEntry::stateRunning )
							{
								nextAction=stopEvent;
								actionHandler();
							}
							break;

						case 2:
							eDebug("starts in few seconds");
							break;

						case 3:
							eDebug("pausing");
							if ( nextStartingEvent->type & ePlaylistEntry::stateRunning )
							{
								nextAction=pauseEvent;
								actionHandler();
							}
							break;

						case 4:
							eDebug("running");
							if ( nextStartingEvent->type & ePlaylistEntry::stateWaiting )
								nextAction=startEvent;
							else if ( nextStartingEvent->type & ePlaylistEntry::statePaused )
								nextAction=restartEvent;
							else
								break;
							actionHandler();
							break;
						case 5 ... 7:
							eDebug("reserved for future use");
							break;
					}
					break;
				}
			}
			eit->unlock();
		}
	}
}

long eTimerManager::getSecondsToBegin()
{
	time_t nowTime = time(0)+eDVB::getInstance()->time_difference;
	time_t tmp=0;
	if ( (tmp = getNextEventStartTime( nextStartingEvent->time_begin, nextStartingEvent->type, getDate() == nextStartingEvent->last_activation ) ) )
		return tmp - nowTime;
	return nextStartingEvent->time_begin - nowTime;
}

long eTimerManager::getSecondsToEnd()
{
	time_t nowTime = time(0)+eDVB::getInstance()->time_difference;
	time_t tmp=0;
	if ( (tmp = getNextEventStartTime( nextStartingEvent->time_begin, nextStartingEvent->type, false ) ) )
		return (tmp + nextStartingEvent->duration) - nowTime;
	return (nextStartingEvent->time_begin + nextStartingEvent->duration) - nowTime;
}

eTimerManager::~eTimerManager()
{
	eDebug("[eTimerManager] down ( %d events in list )", timerlist->getList().size() );
	if (this == instance)
		instance = 0;
	timerlist->save();
	eServiceInterface::getInstance()->removeRef(timerlistref);
}

ePlaylistEntry* eTimerManager::findEvent( eServiceReference *service, EITEvent *evt )
{
 for ( std::list<ePlaylistEntry>::iterator i( timerlist->getList().begin() ); i != timerlist->getList().end(); i++)
		if ( ( evt->event_id != -1 && i->event_id == evt->event_id ) ||
				 ( *service == i->service && evt->start_time == i->time_begin ) )
				return &*i;

	return 0;
}

bool Overlap( time_t beginTime1, int duration1, time_t beginTime2, int duration2 )
{
	eRect movie1( ePoint(beginTime1, 0), eSize( duration1, 10) );
	eRect movie2( ePoint(beginTime2, 0), eSize( duration2, 10) );

	return movie1.intersects(movie2);
}

bool eTimerManager::removeEventFromTimerList( eWidget *sel, const ePlaylistEntry& entry, int type )
{
	for ( std::list<ePlaylistEntry>::iterator i( timerlist->getList().begin() ); i != timerlist->getList().end(); i++)
		if ( *i == entry )
		{
			sel->hide();
			eString str1, str2, str3;
			if (type == erase)
			{
				str1 = _("You would to delete the running event..\nthis stops the timer mode (recording)!");
				str2 = _("Delete the event from the timerlist");
				str3 = _("Really delete this event?");
			}
			else if (type == update)
			{
				str1 = _("You would to update the running event.. \nthis stops the timer mode (recording)!");
				str2 = _("Update event in timerlist");
				str3 = _("Really update this event?");
			}
			if ( &(*nextStartingEvent) == &entry && entry.type & ePlaylistEntry::stateRunning  )
			{
				eMessageBox box(str1, str2, eMessageBox::btOK|eMessageBox::iconWarning );
				box.show();
				box.exec();
				box.hide();
			}
			eMessageBox box(str3, str2, eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btNo);
			box.show();
			int r=box.exec();
			box.hide();
			if (r == eMessageBox::btYes)
			{
				timerlist->getList().erase(i);
				if ( &(*nextStartingEvent) == &entry )
				{
					nextAction=stopEvent;
					nextStartingEvent->type |= (ePlaylistEntry::stateError | ePlaylistEntry::errorUserAborted);
					actionHandler();
				}
				sel->show();
				return true;
			}
			sel->show();
			break;
		}
	return false;
}

bool eTimerManager::removeEventFromTimerList( eWidget *sel, const eServiceReference *ref, const EITEvent *evt )
{
	for ( std::list<ePlaylistEntry>::iterator i( timerlist->getList().begin() ); i != timerlist->getList().end(); i++)
		if ( ( i->event_id != -1 && i->event_id == evt->event_id ) || ( *ref == i->service && evt->start_time == i->time_begin ) )
			return removeEventFromTimerList( sel, *i );
	return false;
}

bool eTimerManager::eventAlreadyInList( eWidget *w, EITEvent &e, eServiceReference &ref )
{
	for ( std::list<ePlaylistEntry>::iterator i( timerlist->getList().begin() ); i != timerlist->getList().end(); i++)
		if ( ( e.event_id != -1 && e.event_id == i->event_id ) ||
			   ( ref == i->service && e.start_time == i->time_begin ) )
		{
			eMessageBox box(
				_("This event is already in the timerlist."),
				_("Add event to timerlist"),
				eMessageBox::iconWarning|eMessageBox::btOK);
			w->hide();
			box.show();
			box.exec();
			box.hide();
			w->show();
			return true;
		}
	return false;
}

bool eTimerManager::addEventToTimerList( eWidget *sel, const ePlaylistEntry& entry )
{
/*	time_t nowTime = time(0)+eDVB::getInstance()->time_difference;
	if ( entry.time_begin < nowTime && !(entry.type & ePlaylistEntry::typeShutOffTimer) )
	{
		eMessageBox box(_("This event already began.\nYou can not add this to timerlist"), _("Add event to timerlist"), eMessageBox::iconWarning|eMessageBox::btOK);
		sel->hide();
		box.show();
		box.exec();
		box.hide();
		sel->show();
		return false;
	}*/
	for ( std::list<ePlaylistEntry>::iterator i( timerlist->getList().begin() ); i != timerlist->getList().end(); i++)
		if ( ( entry.event_id != -1 && entry.event_id == i->event_id ) ||
				 ( entry.service == i->service && entry.time_begin == i->time_begin ) )
		{
			eMessageBox box(_("This event is already in the timerlist."), _("Add event to timerlist"), eMessageBox::iconWarning|eMessageBox::btOK);
			sel->hide();
			box.show();
			box.exec();
			box.hide();
			sel->show();
			return false;
		}
		else
		{
			bool overlap=false;
			if ( i->type & ePlaylistEntry::typeMultiple )
			{
				struct tm multiple = *localtime( &i->time_begin ),
									Entry = *localtime( &entry.time_begin );
/*				eDebug("multiple %02d:%02d, duration = %d, entry %02d:%02d, duration = %d",
					multiple.tm_hour, multiple.tm_min, i->duration,
					Entry.tm_hour, Entry.tm_min, entry.duration );*/
				overlap = Overlap( Entry.tm_hour*3600+Entry.tm_min*60, entry.duration,
										multiple.tm_hour*3600+multiple.tm_min*60, i->duration );
			}
			else overlap = ( !( i->type & (ePlaylistEntry::stateError|ePlaylistEntry::stateFinished) )
									&& Overlap( entry.time_begin, entry.duration, i->time_begin, i->duration) );

			if ( overlap )
			{
				if ( entry.type & ePlaylistEntry::typeShutOffTimer )
				{
					eMessageBox box(_("The Endtime overlaps with another event in the timerlist"), _("Set Stop Time"), eMessageBox::iconWarning|eMessageBox::btOK);
					sel->hide();
					box.show();
					box.exec();
					box.hide();
					sel->show();
				}
				else
				{
					eMessageBox box(_("This event cannot added to the timerlist.\n"
						"The event overlaps with another event in the timerlist\n"
						"Please check the timerlist manually."), _("Add event to timerlist"), eMessageBox::iconWarning|eMessageBox::btOK);
					sel->hide();
					box.show();
					box.exec();
					box.hide();
					sel->show();
				}
				return false;
			}
		}

	timerlist->getList().push_back( entry );
	if ( ( ( nextStartingEvent != timerlist->getList().end() ) && (nextStartingEvent->type & ePlaylistEntry::stateWaiting) )
			|| ( nextStartingEvent == timerlist->getList().end() ) )
	{
		nextAction = setNextEvent;
		actionHandler();
	}
	return true;
}

bool eTimerManager::addEventToTimerList( eWidget *sel, const eServiceReference *ref, const EITEvent *evt, int type )
{
	eServiceReference *subref=0;
// add the event description

//	eString descr	= _("no description is available");

	eSubServiceSelector subservicesel(false);

	for (ePtrList<Descriptor>::const_iterator d(evt->descriptor); d != evt->descriptor.end(); ++d)
	{
		if (d->Tag()==DESCR_LINKAGE)
		{
			LinkageDescriptor *ld=(LinkageDescriptor*)*d;
			if (ld->linkage_type==0xB0)
				subservicesel.add(((eServiceReferenceDVB*)ref)->getDVBNamespace(), ld);
		}
	}
	if ( subservicesel.getSelected() )  // channel have subservices?
	{
		sel->hide();
		subservicesel.show();
		if ( !subservicesel.exec() )
			subref=subservicesel.getSelected();
		subservicesel.hide();
		sel->show();
	}
	if ( subref )
		subref->descr+=ref->descr;

	ePlaylistEntry e( subref?*subref:*ref, evt->start_time, evt->duration, evt->event_id, type );
	eDebug("e.service.descr = %s", e.service.descr.c_str() );
//	eDebug("descr = %s", descr.c_str() );
//	eString tmp = getLeft(e.service.descr, '/');
//	eDebug("tmp = %s", tmp.c_str() );	
//	e.service.descr = tmp + '/' + descr;
	return addEventToTimerList( sel, e );
}

eAutoInitP0<eTimerManager> init_eTimerManager(eAutoInitNumbers::osd-1, "Timer Manager");

gFont eListBoxEntryTimer::TimeFont;
gFont eListBoxEntryTimer::DescrFont;
gPixmap *eListBoxEntryTimer::ok=0;
gPixmap *eListBoxEntryTimer::failed=0;
int eListBoxEntryTimer::timeXSize=0;
int eListBoxEntryTimer::dateXSize=0;

struct eTimerViewActions
{
	eActionMap map;
	eAction addTimerEntry, removeTimerEntry;
	eTimerViewActions():
		map("timerView", _("timerView")),
		addTimerEntry(map, "addTimerEntry", _("add new event to Timerlist"), eAction::prioDialog ),
		removeTimerEntry(map, "removeTimerEntry", _("remove this entry from timer list"), eAction::prioDialog )
	{
	}
};
eAutoInitP0<eTimerViewActions> i_TimerViewActions(eAutoInitNumbers::actions, "timer view actions");

eListBoxEntryTimer::~eListBoxEntryTimer()
{
	if (paraTime)
		paraTime->destroy();

	if (paraDate)
		paraDate->destroy();

	if (paraDescr)
		paraDescr->destroy();

	if (paraService)
		paraService->destroy();
}

int eListBoxEntryTimer::getEntryHeight()
{
	if (!DescrFont.pointSize && !TimeFont.pointSize)
	{
		DescrFont = eSkin::getActive()->queryFont("eEPGSelector.Entry.Description");
		TimeFont = eSkin::getActive()->queryFont("eEPGSelector.Entry.DateTime");
		ok = eSkin::getActive()->queryImage("ok_symbol");
		failed = eSkin::getActive()->queryImage("failed_symbol");
		eTextPara* tmp = new eTextPara( eRect(0, 0, 200, 30) );
		tmp->setFont( TimeFont );
		tmp->renderString( "00:00 - 00:00" );
		timeXSize = tmp->getBoundBox().width();
		tmp->destroy();
		tmp = new eTextPara( eRect(0, 0, 200, 30) );
		tmp->setFont( TimeFont );
		tmp->renderString( "00.00," );
		dateXSize = tmp->getBoundBox().width();
		tmp->destroy();
	}
	return (calcFontHeight(DescrFont)+4)*2;
}

eListBoxEntryTimer::eListBoxEntryTimer( eListBox<eListBoxEntryTimer> *listbox, ePlaylistEntry* entry )
		:eListBoxEntry((eListBox<eListBoxEntry>*)listbox),
		paraDate(0), paraTime(0), paraDescr(0), paraService(0), entry(entry)
{
}

const eString &eListBoxEntryTimer::redraw(gPainter *rc, const eRect& rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int hilited)
{
	drawEntryRect(rc, rect, coActiveB, coActiveF, coNormalB, coNormalF, hilited);

	eString hlp;

	int xpos=rect.left()+10;

	if ( entry->type & ePlaylistEntry::stateFinished )
	{
		int ypos = ( rect.height() - ok->y ) / 2;
		rc->blit( *ok, ePoint( xpos, rect.top()+ypos ), eRect(), gPixmap::blitAlphaTest);		
	}
	else if ( entry->type & ePlaylistEntry::stateError )
	{
		int ypos = (rect.height() - failed->y) / 2;
		rc->blit( *failed, ePoint( xpos, rect.top()+ypos ), eRect(), gPixmap::blitAlphaTest);		
	}
	xpos+=24+10; // i think no people want to change the ok and false pixmaps....

	tm start_time = *localtime(&entry->time_begin);
	time_t t = entry->time_begin+entry->duration;
	tm stop_time = *localtime(&t);

	eString descr;
	if (!paraDescr)
	{
		descr = getRight( entry->service.descr, '/' );
		paraDescr = new eTextPara( eRect( 0 ,0, rect.width(), rect.height()/2) );
		paraDescr->setFont( DescrFont );
		paraDescr->renderString( descr );
		DescrYOffs = ((rect.height()/2 - paraDescr->getBoundBox().height()) / 2 ) - paraDescr->getBoundBox().top();
	}
	rc->renderPara(*paraDescr, ePoint( xpos, rect.top() + DescrYOffs + rect.height()/2 ) );

	if (!paraDate)
	{
		paraDate = new eTextPara( eRect( 0, 0, entry->type&ePlaylistEntry::typeMultiple?200:dateXSize, rect.height()/2) );
		paraDate->setFont( TimeFont );
		eString tmp;
		if ( entry->type & ePlaylistEntry::typeMultiple )
		{
			int mask = ePlaylistEntry::Su;
			for ( int i = 0; i < 7; i++ )
			{
				if ( entry->type & mask )
				{
					tmp+=dayStrShort[i];
					if ( i != 2 && i != 3 && i != 4 )
						tmp.erase( tmp.length()-1 );
					tmp+=' ';
				}
				mask*=2;
			}
			if ( tmp.length() )
				tmp.erase( tmp.length()-1 );
		}
		else
			tmp.sprintf("%02d.%02d,", start_time.tm_mday, start_time.tm_mon+1);
		paraDate->renderString( tmp );
//		paraDate->realign( eTextPara::dirRight );
		TimeYOffs = ((rect.height()/2 - paraDate->getBoundBox().height()) / 2 ) - paraDate->getBoundBox().top();
		hlp+=tmp;
	}
	dateXSize = paraDate->getBoundBox().width();
	rc->renderPara(*paraDate, ePoint( xpos, rect.top() + TimeYOffs ) );
	xpos+=dateXSize+paraDate->getBoundBox().height();

	if (!paraTime)
	{
		paraTime = new eTextPara( eRect( 0, 0, timeXSize, rect.height()/2) );
		paraTime->setFont( TimeFont );
		eString tmp;
		tmp.sprintf("%02d:%02d - %02d:%02d", start_time.tm_hour, start_time.tm_min, stop_time.tm_hour, stop_time.tm_min);
		paraTime->renderString( tmp );
//		paraTime->realign( eTextPara::dirRight );
		hlp+=tmp;
	}
	rc->renderPara(*paraTime, ePoint( xpos, rect.top() + TimeYOffs ) );
	xpos+=timeXSize+paraTime->getBoundBox().height();

	if (!paraService)
	{
		eString sname = getLeft(entry->service.descr,'/');
		if (!sname)
		{
			eService *service = eServiceInterface::getInstance()->addRef( entry->service );
			if ( service )
			{
				sname = service->service_name;
				eServiceInterface::getInstance()->removeRef( entry->service );
			}
		}
		if (sname)
		{
			paraService = new eTextPara( eRect( 0, 0, rect.width()-xpos, rect.height()/2) );
			paraService->setFont( TimeFont );
			paraService->renderString( sname );
		}
	}
	if ( paraService )
		rc->renderPara(*paraService, ePoint( xpos, rect.top() + TimeYOffs ) );

	static eString ret = hlp + " "+descr;
	return ret;
}

static int weekday (int d, int m, int y)
{
	static char table[13] = {0,0,3,2,5,0,3,5,1,4,6,2,4};
	if (m<3)
		--y;
	return (y+y/4-y/100+y/400+table[m]+d)%7;
}

void normalize( struct tm &t )
{
	while ( t.tm_min > 59 )
	{
		t.tm_min -= 60;
		t.tm_hour++;
	}
	while ( t.tm_hour > 23 )
	{
		t.tm_hour-=24;
		t.tm_mday++;
	}
	int days = monthdays[t.tm_mon+1];
	if ( __isleap(t.tm_year) )
		days++;
	while ( t.tm_mday > days )
	{
		t.tm_mday -= days;
		t.tm_mon++;
	}
	while ( t.tm_mon > 11 )
	{
		t.tm_year++;
		t.tm_mon-=11;
	}
	t.tm_wday=-1;
	t.tm_yday=-1;
}

eTimerListView::eTimerListView()
	:eWindow(0)
{
	events = new eListBox<eListBoxEntryTimer>(this);
	events->setName("events");
	events->setActiveColor(eSkin::getActive()->queryScheme("eServiceSelector.highlight.background"), eSkin::getActive()->queryScheme("eServiceSelector.highlight.foreground"));
	CONNECT(events->selected, eTimerListView::entrySelected );	

	add = new eButton( this );
	add->setName("add");
	CONNECT( add->selected, eTimerListView::addPressed );

	erase = new eButton( this );
	erase->setName("remove");
	CONNECT( erase->selected, eTimerListView::erasePressed );

	if (eSkin::getActive()->build(this, "eTimerListView"))
		eWarning("Timer view widget build failed!");

	setText(_("Timer list"));

	fillTimerList();

	addActionMap( &i_TimerViewActions->map );

}

void eTimerListView::erasePressed()
{
	if ( events->getCount() && eTimerManager::getInstance()->removeEventFromTimerList( this, *events->getCurrent()->entry ) )
		fillTimerList();
}

void eTimerListView::addPressed()
{
	hide();
	eTimerEditView e(0);
	e.show();
	if ( !e.exec() )
		fillTimerList();
	e.hide();
	show();
}

void eTimerListView::entrySelected(eListBoxEntryTimer *entry)
{
	if ( entry )
	{
		hide();
		eTimerEditView e( entry->entry );
		e.show();
		if ( !e.exec() )
			fillTimerList();
		e.hide();
		show();
	}
}

struct addToView: public std::unary_function<ePlaylistEntry*, void>
{
	eListBox<eListBoxEntryTimer> *listbox;

	addToView(eListBox<eListBoxEntryTimer> *lb): listbox(lb)
	{
	}

	void operator()(ePlaylistEntry* se)
	{
		new eListBoxEntryTimer(listbox, se);
	}
};

void eTimerListView::fillTimerList()
{
	events->beginAtomic();
	events->clearList();
	eTimerManager::getInstance()->forEachEntry( addToView(events) );
	events->sort();
	events->endAtomic();
}

void eTimerEditView::createWidgets()
{
	event_name = new eTextInputField(this);
	event_name->setName("event_name");
	event_name->setMaxChars(50);

	multiple = new eCheckbox(this);
	multiple->setName("multiple");
	CONNECT(multiple->checked, eTimerEditView::multipleChanged);

	cMo = new eCheckbox(this);
	cMo->setName("Mo");

	cTue = new eCheckbox(this);
	cTue->setName("Tue");
	
	cWed = new eCheckbox(this);
	cWed->setName("Wed");

	cThu = new eCheckbox(this);
	cThu->setName("Thu");

	byear = new eComboBox(this);
	byear->setName("b_year");

	bmonth = new eComboBox(this);
	bmonth->setName("b_month");

	bday = new eComboBox(this);
	bday->setName("b_day");

	lBegin = new eLabel(this);
	lBegin->setName("lBegin");

	btime = new eNumber( this, 2, 0, 59, 2, 0, 0, lBegin);
	btime->setName("b_time");
	btime->setFlags( eNumber::flagDrawPoints|eNumber::flagFillWithZeros|eNumber::flagTime );
	CONNECT( btime->selected, eTimerEditView::focusNext );

	cFr = new eCheckbox(this);
	cFr->setName("Fr");

	cSa = new eCheckbox(this);
	cSa->setName("Sa");

	cSu = new eCheckbox(this);
	cSu->setName("Su");

	eyear = new eComboBox(this);
	eyear->setName("e_year");

	emonth = new eComboBox(this);
	emonth->setName("e_month");

	eday = new eComboBox(this);
	eday->setName("e_day");

	lEnd = new eLabel(this);
	lEnd->setName("lEnd");

	etime = new eNumber( this, 2, 0, 59, 2, 0, 0, lEnd );
	etime->setName("e_time");
	etime->setFlags( eNumber::flagDrawPoints|eNumber::flagFillWithZeros|eNumber::flagTime );
	CONNECT( etime->selected, eTimerEditView::focusNext );

	type = new eComboBox( this );
	type->setName("type");

	bSelectService = new eButton( this );
	bSelectService->setName("select_service");
	CONNECT( bSelectService->selected, eTimerEditView::showServiceSelector );

	bApply = new eButton( this );
	bApply->setName("apply");
	CONNECT( bApply->selected, eTimerEditView::applyPressed );

	bScanEPG = new eButton(this);
	bScanEPG->setName("scanEPG");
	CONNECT( bScanEPG->selected, eTimerEditView::scanEPGPressed);

	CONNECT(byear->selchanged_id, eTimerEditView::comboBoxClosed);
	CONNECT(bmonth->selchanged_id, eTimerEditView::comboBoxClosed);
	CONNECT(bday->selchanged_id, eTimerEditView::comboBoxClosed);
	CONNECT(eyear->selchanged_id, eTimerEditView::comboBoxClosed);
	CONNECT(emonth->selchanged_id, eTimerEditView::comboBoxClosed);
	CONNECT(eday->selchanged_id, eTimerEditView::comboBoxClosed);

	if (eSkin::getActive()->build(this, "eTimerEditView"))
		eWarning("Timer view widget build failed!");

	time_t tmp = time(0)+eDVB::getInstance()->time_difference;
	tm now = *localtime( &tmp );

	for ( int i=0; i<10; i++ )
		new eListBoxEntryText( *byear, eString().sprintf("20%02d", now.tm_year+(i-100)), (void*) (now.tm_year+i) );

	for ( int i=0; i<=11; i++ )
		new eListBoxEntryText( *bmonth, monthStr[i], (void*) i );

	for ( int i=0; i<10; i++ )
		new eListBoxEntryText( *eyear, eString().sprintf("20%02d", now.tm_year+(i-100)), (void*) (now.tm_year+i) );

	for ( int i=0; i<=11; i++ )
		new eListBoxEntryText( *emonth, monthStr[i], (void*) i );

	new eListBoxEntryText( *type, _("switch"), (void*) ePlaylistEntry::SwitchTimerEntry );
	if(eDVB::getInstance()->getmID() != 6)
	{
		new eListBoxEntryText( *type, _("record DVR"), (void*) (ePlaylistEntry::RecTimerEntry|ePlaylistEntry::recDVR) );
		new eListBoxEntryText( *type, _("Ngrab"), (void*) (ePlaylistEntry::RecTimerEntry|ePlaylistEntry::recNgrab) );
//	new eListBoxEntryText( *type, _("record VCR"), (void*) ePlaylistEntry::RecTimerEntry|ePlaylisteEntry::recVCR ); );
	}
}

eTimerEditView::eTimerEditView( ePlaylistEntry* e)
	:curEntry(e)
{
	createWidgets();

	if ( e )
	{
		multipleChanged( e->type&ePlaylistEntry::typeMultiple );
		fillInData( e->time_begin, e->duration, e->type, e->service );
	}
	else
	{
		multipleChanged( 0 );
		time_t now = time(0)+eDVB::getInstance()->time_difference;
		int type =
			eDVB::getInstance()->getmID() == 6 ?
				ePlaylistEntry::SwitchTimerEntry :
			eDVB::getInstance()->getmID() == 5 ?
				ePlaylistEntry::RecTimerEntry|ePlaylistEntry::recDVR :
				ePlaylistEntry::RecTimerEntry|ePlaylistEntry::recNgrab;
		fillInData( now, 0, type, eServiceInterface::getInstance()->service );
	}
}

eTimerEditView::eTimerEditView( const EITEvent &e, int type, eServiceReference ref )
	:curEntry(0)
{
	createWidgets();
	fillInData( e.start_time, e.duration, type, ref );
	multipleChanged(0);
	scanEPGPressed();
}

void eTimerEditView::fillInData( time_t begin_time, int duration, int ttype, eServiceReference& ref )
{
	eString descr = getRight( ref.descr, '/' );
	if ( descr.length() )
		event_name->setText(descr);
	else
		event_name->setText(_("no description is available"));
	beginTime = *localtime( &begin_time );
	time_t tmp = begin_time + duration;
	endTime = *localtime( &tmp );
	updateDateTime( beginTime, endTime );
	type->setCurrent( (void*) ( ttype & (ePlaylistEntry::RecTimerEntry|ePlaylistEntry::SwitchTimerEntry|ePlaylistEntry::recDVR|ePlaylistEntry::recVCR|ePlaylistEntry::recNgrab ) ) );
	tmpService = ref;

	eString sname = getLeft( tmpService.descr, '/' );
	if ( sname )
		bSelectService->setText(sname);
	else
	{
		eService *service = eServiceInterface::getInstance()->addRef( tmpService );
		if (service)
		{
			bSelectService->setText( service->service_name );
			eServiceInterface::getInstance()->removeRef( tmpService );
		}
		else
			bSelectService->setText(_("unknown service"));
	}
	setMultipleCheckboxes( ttype );
}

void eTimerEditView::setMultipleCheckboxes( int type )
{
	if ( type & ePlaylistEntry::typeMultiple )
	{
		multiple->setCheck(1);
		cMo->setCheck(type&ePlaylistEntry::Mo);
		cTue->setCheck(type&ePlaylistEntry::Tue);
		cWed->setCheck(type&ePlaylistEntry::Wed);
		cThu->setCheck(type&ePlaylistEntry::Thu);
		cFr->setCheck(type&ePlaylistEntry::Fr);
		cSa->setCheck(type&ePlaylistEntry::Sa);
		cSu->setCheck(type&ePlaylistEntry::Su);
	}
	else
	{
		multiple->setCheck(0);
		cMo->setCheck(0);
		cTue->setCheck(0);
		cWed->setCheck(0);
		cThu->setCheck(0);
		cFr->setCheck(0);
		cSa->setCheck(0);
		cSu->setCheck(0);
	}
}

void eTimerEditView::applyPressed()
{
	EITEvent evt;
	time_t newEventBegin;
	int newEventDuration;
	int ttype = ( (int) type->getCurrent()->getKey() ) |
		ePlaylistEntry::stateWaiting;
	if ( multiple->isChecked() )
	{
		ttype |= ePlaylistEntry::typeMultiple;
		if ( cMo->isChecked() )
			ttype |= ePlaylistEntry::Mo;
		if ( cTue->isChecked() )
			ttype |= ePlaylistEntry::Tue;
		if ( cWed->isChecked() )
			ttype |= ePlaylistEntry::Wed;
		if ( cThu->isChecked() )
			ttype |= ePlaylistEntry::Thu;
		if ( cFr->isChecked() )
			ttype |= ePlaylistEntry::Fr;
		if ( cSa->isChecked() )
			ttype |= ePlaylistEntry::Sa;
		if ( cSu->isChecked() )
			ttype |= ePlaylistEntry::Su;
	}
	else
	{
		ttype &= ~ePlaylistEntry::typeMultiple;
		if ( cMo->isChecked() )
			ttype &= ~ePlaylistEntry::Mo;
		if ( cTue->isChecked() )
			ttype &= ~ePlaylistEntry::Tue;
		if ( cWed->isChecked() )
			ttype &= ~ePlaylistEntry::Wed;
		if ( cThu->isChecked() )
			ttype &= ~ePlaylistEntry::Thu;
		if ( cFr->isChecked() )
			ttype &= ~ePlaylistEntry::Fr;
		if ( cSa->isChecked() )
			ttype &= ~ePlaylistEntry::Sa;
		if ( cSu->isChecked() )
			ttype &= ~ePlaylistEntry::Su;
	}
	if ( getData( newEventBegin, newEventDuration) )  // all is okay... we add the event..
	{
		// parse EPGCache to get event informations
		EITEvent *tmp = eEPGCache::getInstance()->lookupEvent( (eServiceReferenceDVB&)tmpService, newEventBegin+newEventDuration / 2 );
		if (tmp)
		{
			evt = *tmp;
			evt.descriptor.setAutoDelete(true);
			tmp->descriptor.setAutoDelete(false); // Problem Ptrlist....
			delete tmp;
		}
		else  // ohh.. not found...
		{
			evt.running_status = -1;
			evt.free_CA_mode = -1;
			evt.event_id = -1;
		}
		evt.start_time = newEventBegin;
		evt.duration = newEventDuration;

		eString sname = getLeft(tmpService.descr,'/');
		eString descr = event_name->getText();

		// On Multiple Events.. kill Episode Infos..
		unsigned int pos=0;
		if ( multiple->isChecked() && ( pos = descr.find(" - ") ) != eString::npos )
			descr.erase( pos, descr.length() - pos );

		if ( descr.length() )
			tmpService.descr='/'+descr;
		if ( sname.length() )
			tmpService.descr=sname+tmpService.descr;

		bool ret = !curEntry;
		if ( curEntry )		// remove old event from list...
			ret = eTimerManager::getInstance()->removeEventFromTimerList( this, *curEntry, eTimerManager::update );
		if ( ret && eTimerManager::getInstance()->addEventToTimerList( this, &tmpService, &evt, ttype ) )
			close(0);
	}
	else
	{
		hide();
		eMessageBox box(_("Invalid begin or end time.!\nPlease check time and date"), _("Update event in timerlist"), eMessageBox::iconWarning|eMessageBox::btOK);
		box.show();
		box.exec();
		box.hide();
		show();
	}
}

bool eTimerEditView::getData( time_t &bTime, int &duration )
{
	if ( multiple->isChecked() )
	{
		beginTime.tm_year = 70;  // 1.1.1970
		beginTime.tm_mon = 0;
		beginTime.tm_mday = 1;

		endTime.tm_year = 70;  // 1.1.1970
		endTime.tm_mon = 0;
		endTime.tm_mday = 1;
	}
	else
	{
		beginTime.tm_year = (int)byear->getCurrent()->getKey();
		beginTime.tm_mon = (int)bmonth->getCurrent()->getKey();
		beginTime.tm_mday = (int)bday->getCurrent()->getKey();

		endTime.tm_year = (int)eyear->getCurrent()->getKey();
		endTime.tm_mon = (int)emonth->getCurrent()->getKey();
		endTime.tm_mday = (int)eday->getCurrent()->getKey();
	}

	beginTime.tm_hour = btime->getNumber(0);
	beginTime.tm_min = btime->getNumber(1);
	beginTime.tm_sec = 0;

	endTime.tm_hour = etime->getNumber(0);
	endTime.tm_min = etime->getNumber(1);
	endTime.tm_sec = 0;

	bTime = mktime( &beginTime );
	time_t tmp = mktime( &endTime );
	duration = tmp - bTime;

	return duration > 0;
}

void eTimerEditView::updateDateTime( const tm& beginTime, const tm& endTime )
{
	updateDay( bday, beginTime.tm_year+1900, beginTime.tm_mon+1, beginTime.tm_mday );
	updateDay( eday, endTime.tm_year+1900, endTime.tm_mon+1, endTime.tm_mday );

	btime->setNumber( 0, beginTime.tm_hour );
	btime->setNumber( 1, beginTime.tm_min );

	byear->setCurrent( (void*) beginTime.tm_year );
	bmonth->setCurrent( (void*) beginTime.tm_mon );

	etime->setNumber( 0, endTime.tm_hour );
	etime->setNumber( 1, endTime.tm_min );

	eyear->setCurrent( (void*) endTime.tm_year );
	emonth->setCurrent( (void*) endTime.tm_mon );
}

void eTimerEditView::multipleChanged( int i )
{
	if ( i )
	{
		byear->hide();
		eyear->hide();
		bmonth->hide();
		emonth->hide();
		bday->hide();
		eday->hide();
		lBegin->show();
		lEnd->show();
		cMo->show();
		cTue->show();
		cWed->show();
		cThu->show();
		cFr->show();
		cSa->show();
		cSu->show();
	}
	else
	{
		cMo->hide();
		cMo->setCheck(0);
		cTue->hide();
		cTue->setCheck(0);
		cWed->hide();
		cWed->setCheck(0);
		cThu->hide();
		cThu->setCheck(0);
		cFr->hide();
		cFr->setCheck(0);
		cSa->hide();
		cSa->setCheck(0);
		cSu->hide();
		cSu->setCheck(0);
		lBegin->hide();
		lEnd->hide();
		byear->show();
		eyear->show();
		bmonth->show();
		emonth->show();
		bday->show();
		eday->show();
	}
}

void eTimerEditView::updateDay( eComboBox* dayCombo, int year, int month, int day )
{
	dayCombo->clear();
	int wday = weekday( 1, month, year );
	int days = monthdays[ month-1 ];
	days += (days == 28 && __isleap( year ) );
	for ( int i = wday; i < wday+days; i++ )
		new eListBoxEntryText( *dayCombo, eString().sprintf("%s, %02d", dayStr[i%7], i-wday+1), (void*) (i-wday+1) );
	dayCombo->setCurrent( day>days ? 0 : (void*) day );
}

void eTimerEditView::comboBoxClosed( eComboBox *combo,  eListBoxEntryText* )
{
	if ( combo == bmonth || combo == byear )
		updateDay( bday, (int) byear->getCurrent()->getKey()+1900, (int) bmonth->getCurrent()->getKey()+1, (int) bday->getCurrent()->getKey() );
	else if ( combo == emonth || combo == eyear )
		updateDay( eday, (int) eyear->getCurrent()->getKey()+1900, (int) emonth->getCurrent()->getKey()+1, (int) eday->getCurrent()->getKey() );
}

void eTimerEditView::showServiceSelector()
{
	eServiceSelector sel;
	sel.setLCD(LCDTitle, LCDElement);
	hide();
	sel.setPath(eServiceStructureHandler::getRoot(eServiceStructureHandler::modeTvRadio),eServiceReference() );
	sel.setStyle(eServiceSelector::styleSingleColumn);

/*	if ( tmpService != eServiceReference() )
		sel.selectServiceRecursive( tmpService );*/
    
	const eServiceReference *ref = sel.choose(-1);

	if (ref)
	{
		if ( ref->data[0] == 4 ) // NVOD
		{
			eMessageBox box(_("Sorry, you can not add a time shifted service manually to the timer.\nPlease close the Timer and use the EPG of the service you wish to add!"), _("Information"), eMessageBox::iconInfo|eMessageBox::btOK);
			hide();
			box.show();
			box.exec();
			box.hide();
			show();
		}
		else if (tmpService != *ref)
		{
			tmpService = *ref;
			if ( ref->descr.length() )
				bSelectService->setText(ref->descr);
			else
			{
				eService *service = eServiceInterface::getInstance()->addRef( tmpService );
				if ( service )
				{
					bSelectService->setText(service->service_name);
					eServiceInterface::getInstance()->removeRef( tmpService );
				}
				else
					bSelectService->setText(_("unknown"));
			}
		}
	}
	show();
	setFocus(bSelectService);
}

void eTimerEditView::scanEPGPressed()
{
	time_t newEventBegin;
	int newEventDuration;
	if ( getData( newEventBegin, newEventDuration ) )  // all is okay... we add the event..
	{
		if ( multiple->isChecked() )
			newEventBegin = getNextEventStartTime( newEventBegin, newEventDuration, false );
		const eString &descr = getEventDescrFromEPGCache( tmpService, newEventBegin+newEventDuration/2);
		if ( descr )
		{
			event_name->setText( descr );
			return;
		}
		if ( !event_name->getText() )
			event_name->setText(_("no description found"));
	}
}
