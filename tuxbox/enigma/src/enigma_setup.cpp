/*
 * enigma_setup.cpp
 *
 * Copyright (C) 2002 Felix Domke <tmbinc@tuxbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: enigma_setup.cpp,v 1.25.2.30 2003/07/22 22:59:42 ghostrider Exp $
 */

#include <enigma_setup.h>

#include <enigma_scan.h>
#include <enigma_main.h>
#include <setupnetwork.h>
#include <wizard_language.h>
#include <setup_timezone.h>
#include <setup_av.h>
#include <setup_osd.h>
#include <setup_lcd.h>
#include <setup_rc.h>
#include <setup_harddisk.h>
#include <setup_extra.h>
#include <enigma_ci.h>
#include <enigma_scan.h>
#include <setupskin.h>
#include <setupengrab.h>
#include <parentallock.h>
#include <upgrade.h>
#include <enigma.h>
#include <timer.h>
#include <lib/base/i18n.h>
#include <lib/dvb/edvb.h>
#include <lib/dvb/dvbservice.h>
#include <lib/gui/eskin.h>
#include <lib/gui/elabel.h>
#include <lib/gui/emessage.h>

Signal1<void,eZapSetup*> eZapSetup::setupHook;

eZapSetup::eZapSetup()
	:eListBoxWindow<eListBoxEntryMenu>(_("Setup"), 9, 500, true)
{
	move(ePoint(115, 120));
	int havenetwork, haveci, haveharddisk, havelcd, haverfmod;
	switch (eDVB::getInstance()->getmID())
	{
	case 1:
	case 2:
	case 3:
		havenetwork=1;
		haveci=0;
		haveharddisk=0;
		havelcd=1;
		haverfmod=0;
		break;
	case 5:
		havenetwork=1;
		haveci=1;
		haveharddisk=1;
		havelcd=1;
		haverfmod=0;
		break;
	case 6:			//have garnix
		havenetwork=0;
		haveci=1;
		haveharddisk=0;
		havelcd=0;
		haverfmod=1;
		break;
	default:		//have noch weniger
		havenetwork=0;
		haveci=0;
		haveharddisk=0;
		havelcd=0;
		haverfmod=0;
	}
	
	list.setColumns(2);
	addActionMap(&i_shortcutActions->map);
		
	int entry=0;
	
	CONNECT((new eListBoxEntryMenu(&list, _("[back]"), eString().sprintf("(%d) %s", ++entry, _("back to main menu")) ))->selected, eWidget::accept);
	CONNECT((new eListBoxEntryMenu(&list, _("Channels..."), eString().sprintf("(%d) %s", ++entry, _("open channel setup")) ))->selected, eZapSetup::sel_channels);
	CONNECT((new eListBoxEntryMenu(&list, _("A/V Setup..."), eString().sprintf("(%d) %s", ++entry, _("open audio/video setup")) ))->selected, eZapSetup::sel_setup_av);
#ifndef DISABLE_NETWORK
	if (havenetwork)
		CONNECT((new eListBoxEntryMenu(&list, _("Communication..."), eString().sprintf("(%d) %s", ++entry, _("open communication setup")) ))->selected, eZapSetup::sel_network);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("OSD..."), eString().sprintf("(%d) %s", ++entry, _("open on screen display setup")) ))->selected, eZapSetup::sel_osd);
#ifndef DISABLE_LCD
	if (havelcd)
		CONNECT((new eListBoxEntryMenu(&list, _("LCD..."), eString().sprintf("(%d) %s", ++entry, _("open LC display setup")) ))->selected, eZapSetup::sel_lcd);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Remote Control..."), eString().sprintf("(%d) %s", ++entry, _("open remote control setup")) ))->selected, eZapSetup::sel_rc);
	CONNECT((new eListBoxEntryMenu(&list, _("Skin..."), eString().sprintf("(%d) %s", ++entry, _("open skin selector")) ))->selected, eZapSetup::sel_skin);
	CONNECT((new eListBoxEntryMenu(&list, _("Language..."), eString().sprintf("(%d) %s", ++entry, _("open language selector")) ))->selected, eZapSetup::sel_language);
	CONNECT((new eListBoxEntryMenu(&list, _("Time Zone..."), eString().sprintf("(%d) %s", ++entry, _("open time zone setup")) ))->selected, eZapSetup::sel_timezone);
	CONNECT((new eListBoxEntryMenu(&list, _("Time Correction..."), eString().sprintf("(%d) %s", ++entry, _("open time correction window")) ))->selected, eZapSetup::sel_timeCorrection);
#ifndef DISABLE_NETWORK
	CONNECT((new eListBoxEntryMenu(&list, _("Ngrab..."), eString().sprintf("(%d) %s", ++entry, _("open ngrab config")) ))->selected, eZapSetup::sel_engrab);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Extra settings..."), eString().sprintf("(%d) %s", ++entry, _("open extra config")) ))->selected, eZapSetup::sel_extra);
	CONNECT((new eListBoxEntryMenu(&list, _("Parental settings..."), eString().sprintf("(%d) %s", ++entry, _("open parental setup")) ))->selected, eZapSetup::sel_parental);
#ifndef DISABLE_FILE
	if (haveharddisk)
		CONNECT((new eListBoxEntryMenu(&list, _("Harddisk..."), eString().sprintf("(%d) %s", ++entry, _("open harddisk setup")) ))->selected, eZapSetup::sel_harddisk);
#endif
	if (haveci)
		CONNECT((new eListBoxEntryMenu(&list, _("Common Interface..."), eString().sprintf("(%d) %s", ++entry, _("open common interface menu")) ))->selected, eZapSetup::sel_ci);
	if (havenetwork)
		CONNECT((new eListBoxEntryMenu(&list, _("Upgrade..."), eString().sprintf("(%d) %s", ++entry, _("upgrade firmware")) ))->selected, eZapSetup::sel_upgrade);
	setupHook(this);
}

eZapSetup::~eZapSetup()
{
}

void eZapSetup::sel_channels()
{
	hide();
	eZapScan setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();	
}

#ifndef DISABLE_NETWORK
void eZapSetup::sel_network()
{
	hide();
	eZapNetworkSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}
#endif

void eZapSetup::sel_osd()
{
	hide();
	eZapOsdSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

#ifndef DISABLE_LCD
void eZapSetup::sel_lcd()
{
	hide();
	eZapLCDSetup setup;
	setup.setLCD(LCDTitle, LCDElement);
	setup.show();
	setup.exec();
	setup.hide();
	show();
}
#endif

void eZapSetup::sel_rc()
{
	hide();
	eZapRCSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

void eZapSetup::sel_skin()
{
	hide();
	eSkinSetup setup;
	int res;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	res=setup.exec();
	setup.hide();
	if (!res)
	{
		eMessageBox msg(_("You have to restart enigma to apply the new skin\nRestart now?"), _("Skin changed"), eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btYes );
		msg.show();
		if ( msg.exec() == eMessageBox::btYes )
			eZap::getInstance()->quit(2);
		msg.hide();
	}
	show();
}

#ifndef DISABLE_NETWORK
void eZapSetup::sel_engrab()
{
	hide();
	ENgrabSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}
#endif

void eZapSetup::sel_extra()
{
	hide();
	eZapExtraSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

void eZapSetup::sel_setup_av()
{
	hide();
	eZapAVSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show(); 
}

void eZapSetup::sel_language()
{
	hide();
	eWizardLanguage::run();
	show();
}

void eZapSetup::sel_timezone()
{
	hide();
	eZapTimeZoneSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

#ifndef DISABLE_FILE
void eZapSetup::sel_harddisk()
{
	hide();
	eHarddiskSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	if (!setup.getNr())
	{
		eMessageBox msg(_("sorry, no harddisks found!"), _("Harddisk setup..."));
		msg.show();
		msg.exec();
		msg.hide();
	} else
	{
		setup.show();
		setup.exec();
		setup.hide();
	}
	show();
}
#endif

void eZapSetup::sel_ci()
{
#ifndef DISABLE_CI
	hide();
	enigmaCI ci;
#ifndef DISABLE_LCD
	ci.setLCD(LCDTitle, LCDElement);
#endif
	ci.show();
	ci.exec();
	ci.hide();
	show();
#endif
}

void eZapSetup::sel_upgrade()
{
	hide();
	{
		eUpgrade up;
#ifndef DISABLE_LCD
		up.setLCD(LCDTitle, LCDElement);
#endif
		up.show();
		up.exec();
		up.hide();
	}
	show();
}

void eZapSetup::sel_parental()
{
	hide();
	eParentalSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

void eZapSetup::sel_timeCorrection()
{
	eDVBServiceController *sapi = eDVB::getInstance()->getServiceAPI();
	if (sapi && sapi->transponder)
  {
		tsref ref = *sapi->transponder;
		hide();
		eTimeCorrectionEditWindow w(ref);
#ifndef DISABLE_LCD
		w.setLCD(LCDTitle, LCDElement);
#endif
		w.show();
		if (!w.exec())
		{
			std::map<tsref, int> &map =
				eTransponderList::getInstance()->TimeOffsetMap;
			int tCorrection=0;
			std::map<tsref, int>::iterator it = map.find(ref);
			if ( it != map.end() )
				tCorrection = it->second;
			map.clear();
			map[ref] = tCorrection;
			eTimerManager::getInstance()->timeChanged();
		}
		w.hide();
		show();
	}
	else
	{
		hide();
		eMessageBox mb( _("To change time correction you must tune first to any transponder"), _("time correction change error"), eMessageBox::btOK|eMessageBox::iconInfo );
		mb.show();
		mb.exec();
		mb.hide();
		show();
	}
}

int eZapSetup::eventHandler(const eWidgetEvent &event)
{
	int num=-1;
	switch (event.type)
	{
	case eWidgetEvent::evtAction:
		if (event.action == &i_shortcutActions->number0)
			num=9;
		else if (event.action == &i_shortcutActions->number1)
			num=0;
		else if (event.action == &i_shortcutActions->number2)
			num=1;
		else if (event.action == &i_shortcutActions->number3)
			num=2;
		else if (event.action == &i_shortcutActions->number4)
			num=3;
		else if (event.action == &i_shortcutActions->number5)
			num=4;
		else if (event.action == &i_shortcutActions->number6)
			num=5;
		else if (event.action == &i_shortcutActions->number7)
			num=6;
		else if (event.action == &i_shortcutActions->number8)
			num=7;
		else if (event.action == &i_shortcutActions->number9)
			num=8;
		else if (event.action == &i_cursorActions->cancel)
			close(0);
		else
			break;
		if (num != -1)
			sel_num(num);
		return 1;
	default:
		break;
	}
	return eWidget::eventHandler(event);
}

void eZapSetup::sel_num(int n)
{
	list.forEachEntry(eZapSetupSelectN(n));
}
