#include <setup_extra.h>

#include <lib/base/i18n.h>

#include <lib/dvb/edvb.h>
#include <lib/gui/elabel.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/enumber.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/eskin.h>
#include <lib/driver/rc.h>
#include <lib/system/econfig.h>

eZapExtraSetup::eZapExtraSetup():
	eWindow(0)
{
	setText(_("Extra setup"));
	cmove(ePoint(140, 186));
	cresize(eSize(390, 230));

	int fd=eSkin::getActive()->queryValue("fontsize", 20);

	loadSettings();

	profimode=new eCheckbox(this, sprofimode, 1);
	profimode->setText(_("Skip Confirmations"));
	profimode->move(ePoint(20, 20));
	profimode->resize(eSize(fd+4+240, fd+4));
	profimode->setHelpText(_("enable/disable confirmations (ok)"));

	hideerror=new eCheckbox(this, shideerror, 1);
	hideerror->setText(_("Hide error windows"));
	hideerror->move(ePoint(20, 60));
	hideerror->resize(eSize(fd+4+240, fd+4));
	hideerror->setHelpText(_("enable/disable showing the zap error messages(ok)"));

	extZap=new eCheckbox(this, sextzap, 1);
	extZap->setText(_("Advanced zap mode"));
	extZap->move(ePoint(20, 100));
	extZap->resize(eSize(fd+4+240, fd+4));
	extZap->setHelpText(_("enable/disable advanced zap mode"));

	ok=new eButton(this);
	ok->setText(_("save"));
	ok->setShortcut("green");
	ok->setShortcutPixmap("green");
	ok->move(ePoint(20, 140));
	ok->resize(eSize(170, 40));
	ok->setHelpText(_("save changes and return"));
	ok->loadDeco();
	
	CONNECT(ok->selected, eZapExtraSetup::okPressed);

	abort=new eButton(this);
	abort->loadDeco();
	abort->setText(_("abort"));
	abort->move(ePoint(210, 140));
	abort->resize(eSize(170, 40));
	abort->setHelpText(_("ignore changes and return"));

	CONNECT(abort->selected, eZapExtraSetup::abortPressed);

	statusbar=new eStatusBar(this);
	statusbar->move( ePoint(0, clientrect.height()-30 ) );
	statusbar->resize( eSize( clientrect.width(), 30) );
	statusbar->loadDeco();

	setHelpID(2);
}

void eZapExtraSetup::loadSettings()
{
	if (eConfig::getInstance()->getKey("/elitedvb/extra/profimode", sprofimode))
		sprofimode=0;

	if (eConfig::getInstance()->getKey("/elitedvb/extra/hideerror", shideerror))
		shideerror=0;

	if (eConfig::getInstance()->getKey("/elitedvb/extra/extzapping", sextzap))
		sextzap=0;	
}

void eZapExtraSetup::saveSettings()
{
	sprofimode=profimode->isChecked();
	shideerror=hideerror->isChecked();
	sextzap=extZap->isChecked();

	eConfig::getInstance()->setKey("/elitedvb/extra/profimode", sprofimode);
	eConfig::getInstance()->setKey("/elitedvb/extra/hideerror", shideerror);
	eConfig::getInstance()->setKey("/elitedvb/extra/extzapping", sextzap);
}

eZapExtraSetup::~eZapExtraSetup()
{
}

void eZapExtraSetup::okPressed()
{
	saveSettings();
	close(1);
}

void eZapExtraSetup::abortPressed()
{
	close(0);
}

