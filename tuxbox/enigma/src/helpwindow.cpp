#include <helpwindow.h>
#include <xmltree.h>
#include <unistd.h>

#include <lib/base/i18n.h>
#include <lib/gui/eskin.h>
#include <lib/gui/elabel.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/guiactions.h>
#include <lib/gdi/epng.h>
#include <lib/gdi/font.h>
#include <lib/dvb/edvb.h>
#include <lib/system/init_num.h>


struct enigmaHelpWindowActions
{
	eActionMap map;
	eAction close, up, down;
	enigmaHelpWindowActions():
		map("helpwindow", _("Help window")),
		close(map, "close", _("close the help window"), eAction::prioDialog),
		up(map, "up", _("scroll up"), eAction::prioDialogHi),
		down(map, "down", _("scroll down"), eAction::prioDialogHi)
	{
	}
};

eAutoInitP0<enigmaHelpWindowActions> i_helpwindowActions(eAutoInitNumbers::actions, "enigma helpwindow actions");


eHelpWindow::eHelpWindow(ePtrList<eAction> &parseActionHelpList, int helpID):
	eWindow(1)
{
	int xpos, ypos=10;
	int imgwidth, imgheight;

	scrollypos=0;

	setText(_("Help"));
	cmove(ePoint(125, 120));
	cresize(eSize(500, 250));

	scrollbox = new eWidget(this);
	scrollbox->move(ePoint(0, 0));
	scrollbox->resize(eSize(width(), height()*8));

	eString style=eActionMapList::getInstance()->getCurrentStyle();
	int mID=eDVB::getInstance()->getmID();

	for ( ePtrList<eAction>::iterator it( parseActionHelpList.begin() ); it != parseActionHelpList.end() ; it++ )
	{
			std::map< eString, keylist >::iterator b = it->keys.find( style );
			
			if ( b == it->keys.end() )
				b = it->keys.find("");

			if ( b != it->keys.end() )
			{
				keylist &keys = b->second;
				for ( keylist::iterator i( keys.begin() ); i != keys.end() ; i++ )
				{
/*
					eDebug("****** ----> %s    %s", i->producer->getDescription(), i->picture.c_str());
					eDebug("****** ----> %s", it->getDescription());
					eDebug("Picture: %s",eString((DATADIR)+eString("/enigma/pictures/")+i->picture).c_str());
*/
					xpos=10;

					if ((eString(i->producer->getDescription()).find("dreambox") != eString::npos) && ((mID == 5) || (mID == 6)))
					{
						imgheight=0;
						if (i->picture)
						{
							gPixmap *image=eSkin::getActive()->queryImage(i->picture);
							
							if (image)
							{
								label = new eLabel(scrollbox);
								label->setFlags(eLabel::flagVCenter);
								label->move(ePoint(10, ypos));
								imgwidth=image->getSize().width();
								imgheight=image->getSize().height();
								label->resize(eSize(imgwidth, imgheight));
								label->setBlitFlags(BF_ALPHATEST);
								label->setPixmap(image);
								label->setPixmapPosition(ePoint(1, 1));
								xpos=20+imgwidth;
							}
						}

						label = new eLabel(scrollbox);
						label->setFlags(eLabel::flagVCenter);
						label->setFlags(RS_WRAP);
						label->move(ePoint(xpos, ypos));
						label->resize(eSize(width()-xpos-20, 200));
						label->setText(it->getDescription());
						int labelheight=label->getExtend().height();
						label->resize(eSize(width()-xpos-20, labelheight));

						ypos+=(labelheight>imgheight?labelheight:imgheight)+20;
					}
				}
				
			}
	}

	if (helpID)
	{
		xpos+=20;

		eString helptext=loadHelpText(helpID);

		label = new eLabel(scrollbox);
		label->setFlags(eLabel::flagVCenter);
		label->setFlags(RS_WRAP);
		label->move(ePoint(10, ypos));
		label->resize(eSize(width()-40, 200));
		label->setText(helptext.c_str());
		label->resize(eSize(width()-40, label->getExtend().height()));
		ypos+=label->getExtend().height();
	}

	doscroll=ypos>height();
	scrollmax=ypos;

	addActionMap(&i_helpwindowActions->map);
}

eString eHelpWindow::loadHelpText(int helpIDtoLoad)
{
	FILE *in=fopen(eString((DATADIR)+eString("/enigma/resources/help.xml")).c_str(), "rt");
	if (!in)
	{
		eDebug("cannot open help.xml");
		return "";
	}

	XMLTreeParser *parser=new XMLTreeParser("ISO-8859-1");
	char buf[2048];
	
	int done;
	do
	{
		unsigned int len=fread(buf, 1, sizeof(buf), in);
		done=len<sizeof(buf);
		if (!parser->Parse(buf, len, done))
		{
			eFatal("parse error: %s at line %d", parser->ErrorString(parser->GetErrorCode()), parser->GetCurrentLineNumber());
			fclose(in);
			delete parser;
			return "";
		}
	} while (!done);
	fclose(in);

	XMLTreeNode *root=parser->RootNode();
	if (!root)
		return "";
	if (strcmp(root->GetType(), "enigmahelp"))
	{
		eFatal("not a enigma help file.");
		return "";
	}
	
	XMLTreeNode *node=parser->RootNode();
	
	for (node=node->GetChild(); node; node=node->GetNext())
		if (!strcmp(node->GetType(), "help"))
		{
			for (XMLTreeNode *xam=node->GetChild(); xam; xam=xam->GetNext())
				if (!strcmp(xam->GetType(), "helptext"))
				{
					if (helpIDtoLoad==atoi(xam->GetAttributeValue("id")))
					{
						const char *helptext=xam->GetAttributeValue("text");
						eString ht(helptext);
						delete parser;
						return ht;
					}
				}
		}

	delete parser;
	return "";
}



int eHelpWindow::eventHandler(const eWidgetEvent &event)
{
	int fd=eSkin::getActive()->queryValue("fontsize", 20)*2;
	
	switch (event.type)
	{
	case eWidgetEvent::evtAction:
		if (event.action == &i_helpwindowActions->up)
		{
			if (doscroll)
			{
				if (scrollypos)
				{
					scrollypos+=fd;
					if (scrollypos>0) scrollypos=0;
					scrollbox->move(ePoint(0, scrollypos));
				}
			}
		}	else
		if (event.action == &i_helpwindowActions->down)
		{
			if (doscroll && (scrollypos+scrollmax+2*fd>=height()))
			{
				scrollypos-=fd;
				scrollbox->move(ePoint(0, scrollypos));
			}
		}	else
		if (event.action == &i_helpwindowActions->close)
			close(0);
		return 1;
	default:
		break;
	}
	return eWindow::eventHandler(event);
}

eHelpWindow::~eHelpWindow()
{
}

