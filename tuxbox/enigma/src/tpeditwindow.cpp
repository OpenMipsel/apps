#include <tpeditwindow.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/dvbwidgets.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/eskin.h>
#include <lib/gui/emessage.h>

gFont eListBoxEntryTransponder::font;

eListBoxEntryTransponder::eListBoxEntryTransponder( eListBox<eListBoxEntryTransponder>* lb, eTransponder* tp )
	:eListBoxEntry( (eListBox<eListBoxEntry>*)lb ), tp(tp)
{
}

const eString& eListBoxEntryTransponder::redraw(gPainter *rc, const eRect& rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int state)
{
	drawEntryRect( rc, rect, coActiveB, coActiveF, coNormalB, coNormalF, state > 0 );

	static eString text;
	text.sprintf("%d / %d / %c", tp->satellite.frequency/1000, tp->satellite.symbol_rate/1000, tp->satellite.polarisation?'V':'H' );
	rc->setFont(font);
	rc->renderText( rect, text );

	return text;
}

int eListBoxEntryTransponder::getEntryHeight()
{
	if ( !font.pointSize)
		font = eSkin::getActive()->queryFont("eListBox.EntryText.normal");

	return calcFontHeight( font ) + 4;
}

bool eListBoxEntryTransponder::operator < ( const eListBoxEntry& e ) const
{
	return tp->operator<(*((eListBoxEntryTransponder&)e).tp);
}

eTPEditDialog::eTPEditDialog( eTransponder *tp )
	:eWindow(0), tp(tp)
{
	setText(_("Transponder Edit"));
	cmove( ePoint( 150, 200 ) );
	cresize( eSize( 400, 210 ) );
	tpWidget=new eTransponderWidget(this, 1,
		eTransponderWidget::deliverySatellite|
		eTransponderWidget::flagNoSat|
		eTransponderWidget::flagNoInv);
	tpWidget->resize( eSize( 400, 130 ) );
	tpWidget->load();
	tpWidget->setTransponder( tp );
	tpWidget->move( ePoint(0,-40) );
	save=new eButton( this );
	save->setText(_("save"));
	save->setShortcut("green");
	save->setShortcutPixmap("green");
	save->setHelpText(_("save changes and return"));
	save->move(ePoint( 10, getClientSize().height()-80) );
	save->resize( eSize( 170, 40 ) );
	save->loadDeco();
	CONNECT( save->selected, eTPEditDialog::savePressed );
	eStatusBar *sbar = new eStatusBar(this);
	sbar->move( ePoint( 0, getClientSize().height()-30) );
	sbar->resize( eSize( getClientSize().width(), 30 ) );
	sbar->loadDeco();
}

void eTPEditDialog::savePressed()
{
	tpWidget->getTransponder( tp );
	close(0);
}

eTransponderEditWindow::eTransponderEditWindow()
	:eWindow(0), changed(0)
{
	sat = new eButton(this);
	sat->setName("sat");
	add = new eButton(this);
	add->setName("add");
	CONNECT(add->selected, eTransponderEditWindow::addPressed);
	edit = new eButton(this);
	edit->setName("edit");
	CONNECT(edit->selected, eTransponderEditWindow::editPressed);
	remove = new eButton(this);
	remove->setName("remove");
	CONNECT(remove->selected, eTransponderEditWindow::removePressed);
	satellites = new eListBox<eListBoxEntryText>( this, 0, 0 );
	satellites->setName("satlist");
	transponders = new eListBox<eListBoxEntryTransponder>( this, 0, 0 );
	transponders->setName("transponderlist");
	if ( eSkin::getActive()->build( this, "TransponderWindow") )
		eFatal("eTransponderEditWindow build failed");

	eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();
	eTransponder *tp=0;

	if (sapi && sapi->transponder)
		tp = sapi->transponder;

	eListBoxEntryText *sel=0;

	if( !eTransponderList::getInstance()->reloadNetworks() )
	{
		for ( std::list<tpPacket>::const_iterator i(eTransponderList::getInstance()->getNetworks().begin()); i != eTransponderList::getInstance()->getNetworks().end(); ++i)
			if ( tp && i->orbital_position == tp->satellite.orbital_position )
				sel = new eListBoxEntryText(satellites, i->name, (void*) &(*i));
			else
				new eListBoxEntryText(satellites, i->name, (void*) &(*i));
	}
	CONNECT(satellites->selchanged, eTransponderEditWindow::satSelChanged );

	if ( sel )
		satellites->setCurrent(sel);
	else
		satellites->setCurrent(0);

	satSelChanged( satellites->getCurrent() );
}

int eTransponderEditWindow::eventHandler( const eWidgetEvent & event )
{
	switch( event.type )
	{
		case eWidgetEvent::execDone:
			if ( changed )
			{
				eMessageBox mb(_("Save changed Transponders?"),
					_("Transponders changed"),
					eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion,
					eMessageBox::btYes );
				mb.show();
				int ret = mb.exec();
				mb.hide();
				if ( ret == eMessageBox::btNo || ret == -1 )
					return 0;
				if ( eTransponderList::getInstance()->saveNetworks() )
				{
					eMessageBox mb(_("Transponders couldnt written to file!"),
						_("Write Error"),
						eMessageBox::btOK|eMessageBox::iconWarning);
					mb.show();
					mb.exec();
					mb.hide();
				}
			}
			return 0;
		case eWidgetEvent::evtAction:
		{
			if ( event.action == &i_focusActions->up )
			{
				if ( focus == sat )
					satellites->goPrev();
				else
					transponders->goPrev();
			}
			else if ( event.action == &i_focusActions->down )
			{
				if ( focus == sat )
					satellites->goNext();
				else
					transponders->goNext();
			}
			else
				break;
			return 0;
		}
		default:
			break;
	}
	return eWindow::eventHandler( event );
}

eTransponderEditWindow::~eTransponderEditWindow()
{
}

void eTransponderEditWindow::satSelChanged( eListBoxEntryText* sat )
{
	transponders->beginAtomic();
	transponders->clearList();
	if (sat && sat->getKey())
	{
		tpPacket *satTPs = (tpPacket*) (sat->getKey());
		for (std::list<eTransponder>::iterator it( satTPs->possibleTransponders.begin() ); it != satTPs->possibleTransponders.end(); it++)
			new eListBoxEntryTransponder( transponders, &(*it) );

		if (transponders->getCount())
			transponders->setCurrent(0);
	}
	transponders->endAtomic();
}

void eTransponderEditWindow::addPressed()
{
	eTransponder t(*eDVB::getInstance()->settings->getTransponders());
	eListBoxEntryTransponder *te = transponders->getCurrent();
	if ( !te )
		return;
	eTransponder *tp = te->getTransponder();
	if ( !tp )
		return;
	eTPEditDialog dlg( &t );
	dlg.setLCD( LCDTitle, LCDElement );
	dlg.show();
	int ret = dlg.exec();
	dlg.hide();
	if ( !ret )
	{
		changed++;
		eListBoxEntryText *sat = satellites->getCurrent();
		if ( !sat )
			return;
		tpPacket *dest = (tpPacket*)sat->getKey();
		if ( !dest )
			return;
		dest->possibleTransponders.push_back( t );
		eListBoxEntryTransponder *e = new eListBoxEntryTransponder( transponders, &dest->possibleTransponders.back() );
		dest->possibleTransponders.sort();
		transponders->beginAtomic();
		transponders->sort();
		transponders->setCurrent(e);
		transponders->endAtomic();
	}
}

void eTransponderEditWindow::editPressed()
{
	eListBoxEntryTransponder *te = transponders->getCurrent();
	if ( !te )
		return;
	eTransponder *tp = te->getTransponder();
	if ( !tp )
		return;
	eTPEditDialog dlg( tp );
	dlg.setLCD( LCDTitle, LCDElement );
	dlg.show();
	int ret = dlg.exec();
	dlg.hide();

	if ( !ret )
	{
		changed++;
		transponders->beginAtomic();
		transponders->sort();
		transponders->endAtomic();
	}
}

void eTransponderEditWindow::removePressed()
{
	eListBoxEntryTransponder *te = transponders->getCurrent();
	if ( !te )
		return;
	eTransponder *tp = te->getTransponder();
	if ( !tp )
		return;
	eListBoxEntryText *se = satellites->getCurrent();
	if ( !se )
		return;
	tpPacket *packet = (tpPacket*)se->getKey();
	if ( !packet )
		return;
	std::list<eTransponder>::iterator it =
		std::find(
			packet->possibleTransponders.begin(),
			packet->possibleTransponders.end(),
			*tp);
	if ( it == packet->possibleTransponders.end() )
		return;
	packet->possibleTransponders.erase(it);
	transponders->remove( te );
	changed++;
}
