#include <setup_osd.h>

#include <lib/base/i18n.h>
#include <lib/dvb/edvb.h>
#include <lib/gdi/gfbdc.h>
#include <lib/gdi/font.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/eskin.h>
#include <lib/gui/guiactions.h>
#include <lib/system/econfig.h>
#include <lib/system/init.h>
#include <lib/system/init_num.h>

class PluginOffsetScreen: public eWidget
{
	enum { posLeftTop, posRightBottom } curPos;
	eLabel *descr;
	int eventHandler( const eWidgetEvent & e );
	int left, top, right, bottom;
	void redrawLeftTop( gPainter *target );
	void redrawRightBottom( gPainter *target );
	void redrawWidget(gPainter *target, const eRect &where);
	gColor foreColor, backColor;
public:
	PluginOffsetScreen();
};

struct PluginOffsetActions
{
	eActionMap map;
	eAction leftTop, rightBottom, store;
	PluginOffsetActions()
		:map("PluginOffsetActions", _("PluginOffsetActions")),
		leftTop(map,"leftTop", _("enable set the leftTop Point of the rectangle")),
		rightBottom(map,"rightBottom", _("enable set the rightBottom Point of the rectangle")),
		store(map, "store", _("saves the current positions"))
	{
	}
};

eAutoInitP0<PluginOffsetActions> i_PluginOffsetActions(eAutoInitNumbers::actions, "tuxtxt offset actions");

int PluginOffsetScreen::eventHandler( const eWidgetEvent &event )
{
	switch ( event.type )
	{
		case eWidgetEvent::execBegin:
			eConfig::getInstance()->getKey("/enigma/plugins/needoffsets/left", left);
			eConfig::getInstance()->getKey("/enigma/plugins/needoffsets/pos/top", top);
			eConfig::getInstance()->getKey("/enigma/plugins/needoffsets/right", right);
			eConfig::getInstance()->getKey("/enigma/plugins/needoffsets/bottom", bottom);
			invalidate();
			return 0;
		case eWidgetEvent::execDone:
			eConfig::getInstance()->setKey("/enigma/plugins/needoffsets/left", left);
			eConfig::getInstance()->setKey("/enigma/plugins/needoffsets/pos/top", top);
			eConfig::getInstance()->setKey("/enigma/plugins/needoffsets/right", right);
			eConfig::getInstance()->setKey("/enigma/plugins/needoffsets/bottom", bottom);
			return 0;
		case eWidgetEvent::willShow:
			invalidate();
			return 0;
		case eWidgetEvent::evtAction:
			if (event.action == &i_PluginOffsetActions->leftTop)
			{
				curPos=posLeftTop;
				return 0;
			}
			else if (event.action == &i_PluginOffsetActions->rightBottom)
			{
				curPos=posRightBottom;
				return 0;
			}
			else if (event.action == &i_cursorActions->cancel)
			{
				close(0);
				return 0;
			}
			else if (event.action == &i_PluginOffsetActions->store)
			{
				close(0);
				return 0;
			}
			else if (event.action == &i_cursorActions->left)
			{
				if ( curPos == posLeftTop )
					left--;
				else if (curPos == posRightBottom )
					right--;
			}
			else if (event.action == &i_cursorActions->right)
			{
				if ( curPos == posLeftTop )
					left++;
				else if (curPos == posRightBottom )
					right++;
			}
			else if (event.action == &i_cursorActions->up)
			{
				if ( curPos == posLeftTop )
					top--;
				else if (curPos == posRightBottom )
					bottom--;
			}
			else if (event.action == &i_cursorActions->down)
			{
				if ( curPos == posLeftTop )
					top++;
				else if (curPos == posRightBottom )
					bottom++;
			}
			else
				break;
			if ( curPos == posLeftTop )
				invalidate( eRect( ePoint(left-1, top-1), eSize(102, 102) ) );
			else if ( curPos == posRightBottom )
				invalidate( eRect( ePoint(right-101, bottom-101), eSize(102, 102) ) );
			return 0;
		default:
			break;
	}
	return eWidget::eventHandler( event );
}

void PluginOffsetScreen::redrawLeftTop( gPainter *target )
{
	target->fill( eRect( ePoint( left, top ), eSize( 100, 3 ) ) );
	target->fill( eRect( ePoint( left, top ), eSize( 3, 100 ) ) );
}

void PluginOffsetScreen::redrawRightBottom( gPainter *target )
{
	target->fill( eRect( ePoint( right-3, bottom-100 ), eSize( 3, 100 ) ) );
	target->fill( eRect( ePoint( right-100, bottom-3 ), eSize( 100, 3 ) ) );
}

void PluginOffsetScreen::redrawWidget(gPainter *target, const eRect &where)
{
	target->setForegroundColor( foreColor );
	if ( where.intersects( eRect(	ePoint( left, top ), eSize( 100, 100 ) ) ) )
		redrawLeftTop( target );
	if ( where.intersects( eRect( ePoint( right-3, bottom-100 ), eSize( 3, 100 ) ) ) )
		redrawRightBottom( target );
}

PluginOffsetScreen::PluginOffsetScreen()
	:eWidget(0, 1), curPos( posLeftTop ),
		left(20), top(20), right( 699 ), bottom( 555 )
{
	foreColor = eSkin::getActive()->queryColor("eWindow.titleBarFont");
	setForegroundColor( foreColor );
	move(ePoint(0,0));
	resize(eSize(768,576));
	descr = new eLabel( this );
	descr->setFlags( eLabel::flagVCenter|RS_WRAP );
	descr->setForegroundColor( foreColor );
	descr->resize(eSize(568,300));
	descr->move(ePoint(100,100));
	descr->setText(_("here you can center the plugin rectangle...for more infos press help"));
	eSize ext = descr->getExtend();
	descr->resize( ext );
	descr->move( ePoint( (width()/2)-(ext.width()/2) , (height()/2)-(ext.height()/2) ) );
	descr->show();
	addActionMap(&i_PluginOffsetActions->map);
	addActionMap(&i_cursorActions->map);
	addActionToHelpList( &i_PluginOffsetActions->leftTop );
	addActionToHelpList( &i_PluginOffsetActions->rightBottom );
}

eZapOsdSetup::eZapOsdSetup(): eWindow(0)
{
	setText("OSD Setup");
	move(ePoint(120, 95));
	cresize(eSize(440, 395));

	int fd=eSkin::getActive()->queryValue("fontsize", 20);

	int state=0;
	eConfig::getInstance()->getKey("/ezap/osd/showOSDOnEITUpdate", state);

	showOSDOnEITUpdate = new eCheckbox(this, state, fd);
	showOSDOnEITUpdate->setText(_("Show OSD on EIT Update"));
	showOSDOnEITUpdate->move(ePoint(20, 25));
	showOSDOnEITUpdate->resize(eSize(fd+4+300, fd+4));
	showOSDOnEITUpdate->setHelpText(_("shows OSD when now/next info is changed"));

	state=0;
	eConfig::getInstance()->getKey("/ezap/osd/showConsoleOnFB", state);
	showConsoleOnFB = new eCheckbox(this, state, fd);
	showConsoleOnFB->setText(_("Show Console on Framebuffer"));
	showConsoleOnFB->move(ePoint(20, 65));
	showConsoleOnFB->resize(eSize(fd+4+300, fd+4));
	showConsoleOnFB->setHelpText(_("shows the linux console on TV"));

	if ( eDVB::getInstance()->getmID() > 4 )
		showConsoleOnFB->hide();

	alpha = gFBDC::getInstance()->getAlpha();
	eLabel* l = new eLabel(this);
	l->setText(_("Alpha:"));
	l->move(ePoint(20, 105));
	l->resize(eSize(110, fd+4));
	sAlpha = new eSlider( this, l, 0, 512 );
	sAlpha->setIncrement( 10 ); // Percent !
	sAlpha->move( ePoint( 140, 105 ) );
	sAlpha->resize(eSize( 280, fd+4 ) );
	sAlpha->setHelpText(_("change the transparency correction"));
	sAlpha->setValue( alpha);
	CONNECT( sAlpha->changed, eZapOsdSetup::alphaChanged );

	brightness = gFBDC::getInstance()->getBrightness();
	l = new eLabel(this);
	l->setText(_("Brightness:"));
	l->move(ePoint(20, 145));
	l->resize(eSize(110, fd+4));
	sBrightness = new eSlider( this, l, 0, 255 );
	sBrightness->setIncrement( 5 ); // Percent !
	sBrightness->move( ePoint( 140, 145 ) );
	sBrightness->resize(eSize( 280, fd+4 ) );
	sBrightness->setHelpText(_("change the brightness correction"));
	sBrightness->setValue( brightness);
	CONNECT( sBrightness->changed, eZapOsdSetup::brightnessChanged );

	gamma = gFBDC::getInstance()->getGamma();
	l = new eLabel(this);
	l->setText(_("Contrast:"));
	l->move(ePoint(20, 185));
	l->resize(eSize(110, fd+4));
	sGamma = new eSlider( this, l, 0, 255 );
	sGamma->setIncrement( 5 ); // Percent !
	sGamma->move( ePoint( 140, 185 ) );
	sGamma->resize(eSize( 280, fd+4 ) );
	sGamma->setHelpText(_("change the contrast"));
	sGamma->setValue( gamma);
	CONNECT( sGamma->changed, eZapOsdSetup::gammaChanged );

	pluginoffs=new eButton(this);
	pluginoffs->setText(_("plugin offset"));
	pluginoffs->setHelpText(_("set the pluging offset coordinates (tuxtxt, a.o. )"));

	pluginoffs->setShortcut("yellow");
	pluginoffs->setShortcutPixmap("yellow");
	pluginoffs->move(ePoint(20, 240));
	pluginoffs->resize(eSize(250, 40));
	pluginoffs->loadDeco();
	CONNECT( pluginoffs->selected, eZapOsdSetup::PluginOffsetPressed );

	ok=new eButton(this);
	ok->setText(_("save"));
	ok->setShortcut("green");
	ok->setShortcutPixmap("green");
	ok->move(ePoint(20, 310));
	ok->resize(eSize(170, 40));
	ok->setHelpText(_("save changes and return"));
	ok->loadDeco();

	CONNECT(ok->selected, eZapOsdSetup::okPressed);

	abort=new eButton(this);
	abort->setText(_("abort"));
	abort->move(ePoint(210, 310));
	abort->resize(eSize(170, 40));
	abort->setHelpText(_("ignore changes and return"));
	abort->loadDeco();

	CONNECT(abort->selected, eZapOsdSetup::abortPressed);

	statusbar=new eStatusBar(this);
	statusbar->move( ePoint(0, clientrect.height()-30 ) );
	statusbar->resize( eSize( clientrect.width(), 30) );
	statusbar->loadDeco();
}

eZapOsdSetup::~eZapOsdSetup()
{
}

void eZapOsdSetup::consoleStateChanged( int i )
{
	fbClass::getInstance()->showConsole( i );
}

void eZapOsdSetup::alphaChanged( int i )
{
	alpha = i;
	gFBDC::getInstance()->setAlpha(alpha);
}

void eZapOsdSetup::brightnessChanged( int i )
{
	brightness = i;
	gFBDC::getInstance()->setBrightness(brightness);
}

void eZapOsdSetup::gammaChanged( int i )
{
	gamma = i;
	gFBDC::getInstance()->setGamma(gamma);
}

void eZapOsdSetup::fieldSelected(int *number)
{
	focusNext(eWidget::focusDirNext);
}

void eZapOsdSetup::PluginOffsetPressed()
{
	hide();
	PluginOffsetScreen scr;
	scr.show();
	scr.exec();
	scr.hide();
	show();
}

void eZapOsdSetup::okPressed()
{
	gFBDC::getInstance()->saveSettings();
	eConfig::getInstance()->setKey("/ezap/osd/showOSDOnEITUpdate", showOSDOnEITUpdate->isChecked());
	eConfig::getInstance()->setKey("/ezap/osd/showConsoleOnFB", showConsoleOnFB->isChecked());

	eConfig::getInstance()->flush();
	close(1);
}

void eZapOsdSetup::abortPressed()
{
// restore old settings..
	int state=0;
	eConfig::getInstance()->getKey("/ezap/osd/showConsoleOnFB", state);
	fbClass::getInstance()->showConsole(state);
	gFBDC::getInstance()->reloadSettings();

	close(0);
}
