#include <setup_rfmod.h>

#include <lib/base/i18n.h>

#include <lib/driver/rc.h>
#include <lib/gui/elabel.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/eskin.h>
#include <lib/system/econfig.h>


eZapRFmodSetup::eZapRFmodSetup(): eWindow(0)
{
	int fd=eSkin::getActive()->queryValue("fontsize", 20);

	setText(_("RF-Modulator Setup"));
	move(ePoint(150, 136));
	cresize(eSize(390, 200));

	ok=new eButton(this);
	ok->setText(_("save"));
	ok->move(ePoint(20, 120));
	ok->resize(eSize(170, 40));
	ok->setHelpText(_("save settings and leave rf setup"));
	ok->loadDeco();

	CONNECT(ok->selected, eZapRFmodSetup::okPressed);		

	abort=new eButton(this);
	abort->setText(_("abort"));
	abort->move(ePoint(210, 120));
	abort->resize(eSize(170, 40));
	abort->setHelpText(_("leave rf setup (no changes are saved)"));
	abort->loadDeco();

	CONNECT(abort->selected, eZapRFmodSetup::abortPressed);

	status = new eStatusBar(this);	
	status->move( ePoint(0, clientrect.height()-30) );
	status->resize( eSize( clientrect.width(), 30) );
	status->loadDeco();
}

eZapRFmodSetup::~eZapRFmodSetup()
{
	if (status)
		delete status;
}

void eZapRFmodSetup::okPressed()
{
	close(1);
}

void eZapRFmodSetup::abortPressed()
{
	close(0);
}


