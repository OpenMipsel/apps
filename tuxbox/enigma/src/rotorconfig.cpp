#include <rotorconfig.h>

#include <lib/base/i18n.h>
#include <lib/gui/eskin.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/emessage.h>
#include <lib/gui/echeckbox.h>
#include <lib/dvb/edvb.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/frontend.h>
#include <lib/dvb/dvbwidgets.h>
#include <lib/dvb/edvb.h>
//#include <lib/driver/rc.h>

RotorConfig::RotorConfig(eLNB *lnb )
	:lnb(lnb)
{
	LCDTitle=parent->LCDTitle;
	LCDElement=parent->LCDElement;

	useRotorInPower = new eCheckbox(this);
	useRotorInPower->setName("useRotorInPower");

	lDegPerSec = new eLabel(this);
	lDegPerSec->setName("lDegPerSec");
	lDegPerSec->hide();
	DegPerSec = new eNumber( this, 2, 0, 10, 3, 0, 0, lDegPerSec);
	DegPerSec->setFlags( eNumber::flagFixedNum );
	DegPerSec->setName("DegPerSec");
	DegPerSec->hide();

	lDeltaA = new eLabel(this);
	lDeltaA->setName("lDeltaA");
	lDeltaA->hide();
	DeltaA = new eNumber( this, 1, 0, 200, 3, 0, 0, lDeltaA);
	DeltaA->setName("DeltaA");
	DeltaA->hide();

	useGotoXX = new eCheckbox(this);
	useGotoXX->setName("useGotoXX");

	lLongitude = new eLabel(this);
	lLongitude->setName("lLongitude");
	lLongitude->hide();

	Longitude = new eNumber(this, 2, 0, 360, 3, 0, 0, lLongitude );
	Longitude->setFlags( eNumber::flagFixedNum );
	Longitude->setName("Longitude");
	Longitude->hide();

	LoDirection = new eComboBox( this, 2 );
	LoDirection->setName("LoDirection");
	LoDirection->hide();
	new eListBoxEntryText( *LoDirection, _("East"), (void*)eDiSEqC::EAST, 0, _("East") );
	new eListBoxEntryText( *LoDirection, _("West"), (void*)eDiSEqC::WEST, 0, _("West") );

	lLatitude = new eLabel(this);
	lLatitude->setName("lLatitude");
	lLatitude->hide();

	Latitude = new eNumber(this, 2, 0, 360, 3, 0, 0, lLatitude );
	Latitude->setFlags( eNumber::flagFixedNum );
	Latitude->setName("Latitude");
	Latitude->hide();

	LaDirection = new eComboBox( this, 2 );
	LaDirection->setName("LaDirection");
	LaDirection->hide();
	new eListBoxEntryText( *LaDirection, _("North"), (void*)eDiSEqC::NORTH, 0, _("North") );
	new eListBoxEntryText( *LaDirection, _("South"), (void*)eDiSEqC::SOUTH, 0, _("South") );

	positions = new eListBox< eListBoxEntryText >( this );
	positions->setFlags( eListBoxBase::flagNoPageMovement );
	positions->setName("positions");
	positions->hide();

	lStoredRotorNo = new eLabel(this);
	lStoredRotorNo->setName("lStoredRotorNo");
	lStoredRotorNo->hide();
	number = new eNumber( this, 1, 0, 255, 3, 0, 0, lStoredRotorNo);
	number->setName("StoredRotorNo");
	number->hide();

	lOrbitalPosition = new eLabel(this);
	lOrbitalPosition->setName("lOrbitalPosition");
	lOrbitalPosition->hide();
	orbital_position = new eNumber( this, 1, 0, 3600, 4, 0, 0, lOrbitalPosition);

	orbital_position->setName("OrbitalPosition");
	orbital_position->hide();

	lDirection = new eLabel(this);
	lDirection->setName("lDirection");
	lDirection->hide();
	direction = new eComboBox( this, 2, lDirection );
	direction->setName("Direction");
	direction->hide();
	new eListBoxEntryText( *direction, _("East"), (void*)0, 0, _("East") );
	new eListBoxEntryText( *direction, _("West"), (void*)1, 0, _("West") );

	add = new eButton( this );
	add->setName("add");
	add->hide();

	remove = new eButton ( this );
	remove->setName("remove");
	remove->hide();

	save = new eButton(this);
	save->setName("save");

	cancel = new eButton(this);
	cancel->setName("cancel");

	eSkin *skin=eSkin::getActive();
	if (skin->build(this, "RotorConfig"))
		eFatal("skin load of \"RotorConfig\" failed");

	CONNECT( orbital_position->selected, RotorConfig::numSelected );
	CONNECT( Longitude->selected, RotorConfig::numSelected );
	CONNECT( Latitude->selected, RotorConfig::numSelected );
	CONNECT( number->selected, RotorConfig::numSelected );
	CONNECT( DegPerSec->selected, RotorConfig::numSelected );
	CONNECT( DeltaA->selected, RotorConfig::numSelected );
	CONNECT( add->selected, RotorConfig::onAdd );
	CONNECT( remove->selected, RotorConfig::onRemove );
	CONNECT( positions->selchanged, RotorConfig::posChanged );
	CONNECT( useGotoXX->checked, RotorConfig::gotoXXChanged );
	CONNECT( useRotorInPower->checked, RotorConfig::useRotorInPowerChanged );	

	CONNECT( save->selected, RotorConfig::onSavePressed );
	CONNECT( cancel->selected, RotorConfig::reject);

	addActionMap(&i_focusActions->map);

	eDebug("lnb = %p", lnb);

	if (lnb)
		setLNBData(lnb);
}

struct savePosition: public std::unary_function< eListBoxEntryText&, void>
{
	std::map<int,int> &map;

	savePosition(std::map<int,int> &map): map(map)
	{
	}

	bool operator()(eListBoxEntryText& s)
	{
		if ( (int)s.getKey() == 0xFFFF )
			return 0; // ignore sample Entry... delete me...

		int num = atoi( s.getText().left( s.getText().find('/') ).c_str() );
		map[ (int)s.getKey() ] = num;
		return 0;
	}
};

void RotorConfig::onSavePressed()
{
	lnb->getDiSEqC().useGotoXX = useGotoXX->isChecked();
	lnb->getDiSEqC().useRotorInPower = useRotorInPower->isChecked()?1:0;
	lnb->getDiSEqC().useRotorInPower |= DeltaA->getNumber()<<8;
	lnb->getDiSEqC().DegPerSec = DegPerSec->getFixedNum();
	lnb->getDiSEqC().gotoXXLaDirection = (int) LaDirection->getCurrent()->getKey();
	lnb->getDiSEqC().gotoXXLoDirection = (int) LoDirection->getCurrent()->getKey();
	lnb->getDiSEqC().gotoXXLatitude = Latitude->getFixedNum();
	lnb->getDiSEqC().gotoXXLongitude = Longitude->getFixedNum();
	lnb->getDiSEqC().RotorTable.clear();
	positions->forEachEntry( savePosition( lnb->getDiSEqC().RotorTable ) );
	eTransponderList::getInstance()->writeLNBData();	
	close(0);
}

void RotorConfig::useRotorInPowerChanged( int state )
{
	eDebug("useRotorInPowerChanged to %d", state);
	if (state)
	{
		lDegPerSec->hide();
		DegPerSec->hide();
		lDeltaA->show();
		DeltaA->show();
	}
	else
	{
		lDeltaA->hide();
		DeltaA->hide();
		lDegPerSec->show();
		DegPerSec->show();
	}
}

void RotorConfig::gotoXXChanged( int state )
{
	eDebug("gotoXXChanged to %d", state);
	if ( state )
	{
		add->hide();
		remove->hide();
		lOrbitalPosition->hide();
		orbital_position->hide();
		lStoredRotorNo->hide();
		number->hide();
		lDirection->hide();
		direction->hide();
		positions->hide();

		lLongitude->show();
		Longitude->show();
		LoDirection->show();
		lLatitude->show();
		Latitude->show();
		LaDirection->show();
	}
	else
	{
		lLongitude->hide();
		Longitude->hide();
		LoDirection->hide();
		lLatitude->hide();
		Latitude->hide();
		LaDirection->hide();
		
		positions->show();
		add->show();
		remove->show();
		lOrbitalPosition->show();
		orbital_position->show();
		lStoredRotorNo->show();
		number->show();
		lDirection->show();
		direction->show();
		positions->show();
	}
}

int RotorConfig::eventHandler( const eWidgetEvent& e)
{
	switch(e.type)
	{
	case eWidgetEvent::execBegin:
		// send no more DiSEqC Commands on transponder::tune to Rotor
		eFrontend::getInstance()->disableRotor();
	break;

	case eWidgetEvent::execDone:
		// enable send DiSEqC Commands to Rotor on eTransponder::tune
		eFrontend::getInstance()->enableRotor();
	break;
	
	default:
		return eWindow::eventHandler(e);
	break;
	}
	return 1;
}

void RotorConfig::setLNBData( eLNB *lnb )
{
	positions->beginAtomic();
	positions->clearList();
	eDiSEqC &DiSEqC = lnb->getDiSEqC();

	if ( lnb )
	{
		for ( std::map<int, int>::iterator it ( DiSEqC.RotorTable.begin() ); it != DiSEqC.RotorTable.end(); it++ )
			new eListBoxEntryText( positions, eString().sprintf(" %d / %03d %c", it->second, abs(it->first), it->first > 0 ? 'E' : 'W'), (void*) it->first );

		useGotoXX->setCheck( (int) (lnb->getDiSEqC().useGotoXX & 1 ? 1 : 0) );
		gotoXXChanged( (int) lnb->getDiSEqC().useGotoXX & 1 );
		useRotorInPower->setCheck( (int) lnb->getDiSEqC().useRotorInPower & 1 );
		useRotorInPowerChanged( (int) lnb->getDiSEqC().useRotorInPower & 1 );
		Latitude->setFixedNum( lnb->getDiSEqC().gotoXXLatitude );
		LaDirection->setCurrent( (void*) lnb->getDiSEqC().gotoXXLaDirection );
		Longitude->setFixedNum( lnb->getDiSEqC().gotoXXLongitude );
		LoDirection->setCurrent( (void*) lnb->getDiSEqC().gotoXXLoDirection );
		DegPerSec->setFixedNum( lnb->getDiSEqC().DegPerSec );
		DeltaA->setNumber( (lnb->getDiSEqC().useRotorInPower & 0x0FFFFFFF) >> 8 );
	}
	else
	{
		Latitude->setFixedNum(0);
		LaDirection->setCurrent(0);
		Longitude->setFixedNum(0);
		LoDirection->setCurrent(0);
		DegPerSec->setFixedNum( 1.0 );
		DeltaA->setNumber(40);
		useGotoXX->setCheck( 1 );
		useRotorInPower->setCheck( 0 );
	}

	if ( positions->getCount() )
	{
		positions->sort();
		positions->moveSelection(eListBox<eListBoxEntryText>::dirFirst);
	}
	else
	{
		new eListBoxEntryText( positions, _("delete me"), (void*) 0xFFFF );
		posChanged(0);
	}

	positions->endAtomic();
}

void RotorConfig::posChanged( eListBoxEntryText *e )
{
	if ( e && (int)e->getKey() != 0xFFFF )
	{
		direction->setCurrent( e->getText().right( 1 ) == "E" ? 0 : 1 );
		orbital_position->setNumber( (int) e->getKey() );
		number->setNumber( atoi( e->getText().mid( 1 , e->getText().find('/')-1 ).c_str()) );
	}
	else
	{
		orbital_position->setNumber( 0 );
		number->setNumber( 0 );
		direction->setCurrent( 0 );
	}
}

void RotorConfig::numSelected(int*)
{
	focusNext( eWidget::focusDirNext );
}

void RotorConfig::onAdd()
{
	positions->beginAtomic();

	new eListBoxEntryText( positions,eString().sprintf(" %d / %03d %c",
																											number->getNumber(),
																											orbital_position->getNumber(),
																											direction->getCurrent()->getKey() ? 'W':'E'
																										),
													(void*) ( direction->getCurrent()->getKey()
													? - orbital_position->getNumber()
													: orbital_position->getNumber() )
												);

	positions->sort();
	positions->invalidateContent();
	positions->endAtomic();
}

void RotorConfig::onRemove()
{
	positions->beginAtomic();

	if (positions->getCurrent())
		positions->remove( positions->getCurrent() );

	if (!positions->getCount())
	{
		new eListBoxEntryText( positions, _("delete me"), (void*) 0xFFFF );
		posChanged(0);
	}

	positions->invalidate();
	positions->endAtomic();
}

/*
eManuallyRotorPage::eManuallyRotorPage( eWidget *parent )
	:eWidget(parent), transponder(*eDVB::getInstance()->settings->getTransponders())
{
	int ft=0;

	switch (eFrontend::getInstance()->Type())
	{
	case eFrontend::feSatellite:
		ft=eTransponderWidget::deliverySatellite;
		break;
	case eFrontend::feCable:
		ft=eTransponderWidget::deliveryCable;
		break;
	default:
		ft=eTransponderWidget::deliverySatellite;
		break;
	}

	transponder_widget=new eTransponderWidget(this, 1, ft);
	transponder_widget->load();
	transponder_widget->setName("transponder");

	festatus_widget=new eFEStatusWidget(this, eFrontend::getInstance());
	festatus_widget->setName("festatus");

	prev = new eButton(this);
	prev->setName("prev");

	if ( eSkin::getActive()->build(this, "RotorConfigManual"))
		eFatal("skin load of \"RotorConfigManual\" failed");

	CONNECT(transponder_widget->updated, eManuallyRotorPage::retune );
}

void eManuallyRotorPage::retune()
{
	if (!transponder_widget->getTransponder(&transponder))
		transponder.tune();
}

int eManuallyRotorPage::eventHandler( const eWidgetEvent &event )
{
	eDebug("eventHandler");
	switch (event.type)
	{
		case eWidgetEvent::willShow:
		{
			eDebug("eventHandler->willShow()");
			eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();

			if (sapi && sapi->transponder)
				transponder=*sapi->transponder;
			else
				switch (eFrontend::getInstance()->Type())
				{
				case eFrontend::feCable:
					transponder.setCable(402000, 6900000, 0, 3);	// some cable transponder
					break;
				case eFrontend::feSatellite:
					transponder.setSatellite(12551500, 22000000, eFrontend::polVert, 4, 0, 192 ); // some astra transponder
					break;
				default:
					break;
				}
			transponder_widget->setTransponder(&transponder);
		}
		default:
			return eWidget::eventHandler(event);
	}
	return 1;
}
*/
