#include <setup_timezone.h>

#include <lib/gui/slider.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/combobox.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/enumber.h>
#include <lib/gui/eskin.h>
#include <lib/gui/actions.h>
#include <lib/system/econfig.h>
#include <enigma_main.h>

eZapTimeZoneSetup::eZapTimeZoneSetup() : eWindow(0)
{									
	setText(_("Time Zone Setup"));
	cmove(ePoint(110, 146));
	cresize(eSize(530, 270));

	int fd=eSkin::getActive()->queryValue("fontsize", 20);

	ltimeZone=new eLabel(this);
	ltimeZone->move(ePoint(20, 30));
	ltimeZone->resize(eSize(220, fd+4));
	ltimeZone->setText(_("Time Zone:"));
		
	timeZone=new eComboBox(this, 4, ltimeZone);
	timeZone->move(ePoint(20, 70));
	timeZone->resize(eSize(clientrect.width()-40, 35));
	timeZone->setHelpText(_("select your time zone (ok)"));
	timeZone->loadDeco();
		
	if(loadTimeZones())
		close(0);
	
	int cuseDst;
	if ( eConfig::getInstance()->getKey("/elitedvb/useDst", cuseDst) )
		cuseDst=1;

	useDst=new eCheckbox(this);
	useDst->setName("useDst");
	useDst->setText(_("use automatically daylight saving time"));
	useDst->move(ePoint(20, 120));
	useDst->resize(eSize(clientrect.width()-40, 35));
	useDst->setHelpText(_("Automatically adjust clock for daylight saving changes"));
	useDst->setCheck(cuseDst);

	ok=new eButton(this);
	ok->setText(_("save"));
	ok->setShortcut("green");
	ok->setShortcutPixmap("green");
	ok->move(ePoint(20, clientrect.height()-80));
	ok->resize(eSize(170, 40));
	ok->setHelpText(_("save changes and return"));
	ok->loadDeco();
	CONNECT(ok->selected, eZapTimeZoneSetup::okPressed);

	abort=new eButton(this);
	abort->setText(_("abort"));
	abort->move(ePoint(210, clientrect.height()-80));
	abort->resize(eSize(170, 40));
	abort->setHelpText(_("ignore changes and return"));
	abort->loadDeco();
	CONNECT(abort->selected, eZapTimeZoneSetup::abortPressed);

	statusbar=new eStatusBar(this);
	statusbar->move( ePoint(0, clientrect.height()-30 ) );
	statusbar->resize( eSize( clientrect.width(), 30) );
	statusbar->loadDeco();

}

eZapTimeZoneSetup::~eZapTimeZoneSetup()
{
}

void eZapTimeZoneSetup::okPressed()
{

	// save current selected time zone
	if ( eConfig::getInstance()->setKey("/elitedvb/timezone", ((eString*) timeZone->getCurrent()->getKey())->c_str()))
	{
		eConfig::getInstance()->delKey("/elitedvb/timezone");
		eDebug("Write timezone with error %i", eConfig::getInstance()->setKey("/elitedvb/timezone", ((eString*) timeZone->getCurrent()->getKey())->c_str()));
	}
	if ( eConfig::getInstance()->setKey("/elitedvb/useDst", useDst->isChecked()))
	{
		eConfig::getInstance()->delKey("/elitedvb/timezone");
		eDebug("Write timezone with error %i", eConfig::getInstance()->setKey("/elitedvb/useDst", useDst->isChecked()));
	}
	eConfig::getInstance()->flush();
	setTimeZone();
	close(1);
}

void eZapTimeZoneSetup::abortPressed()
{
	close(0);
}

void eZapTimeZoneSetup::setTimeZone()
{
	const char *ctimeZone;
	ctimeZone = cmdTimeZones();
	if (ctimeZone!="")
		setenv("TZ", ctimeZone, 1);
	eDebug("setenv TZ=%s", ctimeZone);
}

int eZapTimeZoneSetup::loadTimeZones()
{
	XMLTreeParser parser("ISO-8859-1");
	int done=0;
	const char *filename="/etc/timezone.xml";

	FILE *in=fopen(filename, "rt");
	if (!in)
	{
		eWarning("unable to open %s", filename);
		return -1;
	}
	do
	{
		char buf[2048];
		unsigned int len=fread(buf, 1, sizeof(buf), in);
		done=len<sizeof(buf);
		if (!parser.Parse(buf, len, done))
		{
			eDebug("parse error: %s at line %d",
				parser.ErrorString(parser.GetErrorCode()),
				parser.GetCurrentLineNumber());
			fclose(in);
			return -1;
		}
	} while (!done);
	fclose(in);

	XMLTreeNode *root=parser.RootNode();
	if (!root)
		return -1;

	char *temp;
	if ( eConfig::getInstance()->getKey("/elitedvb/timezone", temp) )
		temp=0;
		
	const char *ctimeZone = (const char*) temp;
	
	eListBoxEntryText *cur=0;
	for (XMLTreeNode *node = root->GetChild(); node; node = node->GetNext())
		if (!strcmp(node->GetType(), "zone"))
		{
			const char *name=node->GetAttributeValue("name");
			if (!name)
			{
				eFatal("error in a file timezone.xml, no name timezone");
				return -1;
			}
			eListBoxEntryText *tz=new eListBoxEntryText( *timeZone, name, (void*) new eString(name) );
			if (strcmp(ctimeZone, name)==0)
			{
				cur=tz;
			}
		} else
			eFatal("error in a file timezone.xml, unknown timezone");	
	timeZone->setCurrent(cur);
	free(temp);

	return 0;
}

const char *eZapTimeZoneSetup::cmdTimeZones()
{
	XMLTreeParser parser("ISO-8859-1");
	int done=0;
	const char *filename="/etc/timezone.xml";

	FILE *in=fopen(filename, "rt");
	if (!in)
	{
		eWarning("unable to open %s", filename);
		return "";
	}
	do
	{
		char buf[2048];
		unsigned int len=fread(buf, 1, sizeof(buf), in);
		done=len<sizeof(buf);
		if (!parser.Parse(buf, len, done))
		{
			eDebug("parse error: %s at line %d",
				parser.ErrorString(parser.GetErrorCode()),
				parser.GetCurrentLineNumber());
			fclose(in);
			return "";
		}
	} while (!done);
	fclose(in);

	XMLTreeNode *root=parser.RootNode();
	if (!root)
		return "";

	char *temp;
	if ( eConfig::getInstance()->getKey("/elitedvb/timezone", temp) )
		temp=0;
		
	int cuseDst;
	if ( eConfig::getInstance()->getKey("/elitedvb/useDst", cuseDst) )
		cuseDst=1;
		
	const char *ctimeZone = (const char*) temp;
	
	for (XMLTreeNode *node = root->GetChild(); node; node = node->GetNext())
		if (!strcmp(node->GetType(), "zone"))
		{
			const char *name=node->GetAttributeValue("name"),
					*zone=node->GetAttributeValue("zone"),
					*dst=node->GetAttributeValue("dst");
			if (!zone)
			{
				eFatal("error in a file timezone.xml, no name timezone");
				return "";
			}
			if (strcmp(ctimeZone, name)==0)
			{
				if (cuseDst)
					return eString().sprintf("%s%s",zone,dst).c_str();
				else
					return eString().sprintf("%s",zone).c_str();
			}
		} 
		else
			eFatal("error in a file timezone.xml, unknown timezone");
	free(temp);
	
	return "";
}
