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
 * $Id: enigma_setup.cpp,v 1.25.2.21 2003/05/17 21:25:03 alexvrs Exp $
 */

#include <enigma_setup.h>

#include <enigma_scan.h>
#include <setupnetwork.h>
#include <setupvideo.h>
#include <setup_audio.h>
#include <wizard_language.h>
#include <setup_timezone.h>
#include <setup_osd.h>
#include <setup_lcd.h>
#include <setup_rc.h>
#include <setup_harddisk.h>
#include <setup_rfmod.h>
#include <setup_extra.h>
#include <enigma_ci.h>
#include <enigma_scan.h>
#include <setupskin.h>
#include <setupengrab.h>
#include <parentallock.h>
#include <lib/gui/emessage.h>
#include <lib/base/i18n.h>
#include <lib/dvb/edvb.h>
#include <lib/gui/eskin.h>
#include <lib/gui/elabel.h>
#include <lib/gui/testpicture.h>
#include "upgrade.h"

eZapSetup::eZapSetup()
	:eListBoxWindow<eListBoxEntryMenu>(_("Setup"), 9, 450, true)
{
	move(ePoint(135, 120));
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
	
	CONNECT((new eListBoxEntryMenu(&list, _("[back]"), eString().sprintf("(%d) %s", ++entry, _("back to main menu")) ))->selected, eZapSetup::sel_close);
	CONNECT((new eListBoxEntryMenu(&list, _("Channels..."), eString().sprintf("(%d) %s", ++entry, _("open channel setup")) ))->selected, eZapSetup::sel_channels);
#ifndef DISABLE_NETWORK
	if (havenetwork)
		CONNECT((new eListBoxEntryMenu(&list, _("Network..."), eString().sprintf("(%d) %s", ++entry, _("open network setup")) ))->selected, eZapSetup::sel_network);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("OSD..."), eString().sprintf("(%d) %s", ++entry, _("open OSD setup")) ))->selected, eZapSetup::sel_osd);
#ifndef DISABLE_LCD
	if (havelcd)
		CONNECT((new eListBoxEntryMenu(&list, _("LCD..."), eString().sprintf("(%d) %s", ++entry, _("open LCD setup")) ))->selected, eZapSetup::sel_lcd);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Remote Control..."), eString().sprintf("(%d) %s", ++entry, _("open remote control setup")) ))->selected, eZapSetup::sel_rc);
	CONNECT((new eListBoxEntryMenu(&list, _("Video..."), eString().sprintf("(%d) %s", ++entry, _("open video setup")) ))->selected, eZapSetup::sel_video);
	CONNECT((new eListBoxEntryMenu(&list, _("Audio..."), eString().sprintf("(%d) %s", ++entry, _("open audio setup")) ))->selected, eZapSetup::sel_sound);
	CONNECT((new eListBoxEntryMenu(&list, _("Skin..."), eString().sprintf("(%d) %s", ++entry, _("open skin selector")) ))->selected, eZapSetup::sel_skin);
	CONNECT((new eListBoxEntryMenu(&list, _("Language..."), eString().sprintf("(%d) %s", ++entry, _("open language selector")) ))->selected, eZapSetup::sel_language);
	CONNECT((new eListBoxEntryMenu(&list, _("Time Zone..."), eString().sprintf("(%d) %s", ++entry, _("open time zone setup")) ))->selected, eZapSetup::sel_timezone);
#ifndef DISABLE_FILE
	CONNECT((new eListBoxEntryMenu(&list, _("Ngrab..."), eString().sprintf("(%d) %s", ++entry, _("open ngrab config")) ))->selected, eZapSetup::sel_engrab);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Extra settings..."), eString().sprintf("(%d) %s", ++entry, _("open extra config")) ))->selected, eZapSetup::sel_extra);
	CONNECT((new eListBoxEntryMenu(&list, _("Parental settings..."), eString().sprintf("(%d) %s", ++entry, _("open parental setup")) ))->selected, eZapSetup::sel_parental);
#ifndef DISABLE_FILE
	if (haveharddisk)
		CONNECT((new eListBoxEntryMenu(&list, _("Harddisk..."), eString().sprintf("(%d) %s", ++entry, _("open harddisk setup")) ))->selected, eZapSetup::sel_harddisk);
#endif
	if (haveci)
		CONNECT((new eListBoxEntryMenu(&list, _("Common Interface..."), eString().sprintf("(%d) %s", ++entry, _("show CI Menu")) ))->selected, eZapSetup::sel_ci);
	if (havenetwork)
		CONNECT((new eListBoxEntryMenu(&list, _("Upgrade..."), eString().sprintf("(%d) %s", ++entry, _("upgrade firmware")) ))->selected, eZapSetup::sel_upgrade);
#ifdef ENABLE_RFMOD
	if (haverfmod)
		CONNECT((new eListBoxEntryMenu(&list, _("RF-Modulator..."), eString().sprintf("(%d) %s", ++entry, _("setup modulator")) ))->selected, eZapSetup::sel_rfmod);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Video calibration..."), eString().sprintf("(%d) %s", ++entry, _("show calibration picture")) ))->selected, eZapSetup::sel_test);
}

eZapSetup::~eZapSetup()
{
}

void eZapSetup::sel_close()
{
	close(0);
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

void eZapSetup::sel_sound()
{
	hide();
	eZapAudioSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

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
		eMessageBox msg(_("You have to reboot to apply the new skin"), _("Skin changed"));
		msg.show();
		msg.exec();
		msg.hide();
	}
	show();
}

void eZapSetup::sel_video()
{
	hide();
	eZapVideoSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

#ifndef DISABLE_FILE
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

void eZapSetup::sel_language()
{
/*	hide();
	eZapLanguageSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show(); */
	eWizardLanguage::run();
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

#ifdef ENABLE_RFMOD
void eZapSetup::sel_rfmod()
{
	hide();
	eZapRFmodSetup setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}
#endif

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

void eZapSetup::sel_test()
{
	hide();
	{
		eMessageBox msg(
			// please don't translate, it's temporary only.
			("Warning, these collection of testpictures are in no way \"certified\" "
				"or checked by professionals. Don't use them for real calibration.\n"
				"Please note that because of anti-flimmer filters, the multiburst shows "
				"wrong results. Real bandwidth is higher.\n"
				"In doubt, don't use them.\n"
				"Otherwise: use 1..8 to cycle through the pictures, OK to abort."), "Calibration pictures",
				eMessageBox::btOK|eMessageBox::iconInfo);
		msg.show();
		msg.exec();
		msg.hide();
	}
	int curmode=eTestPicture::testFUBK;
	while (curmode != -1)
	{
		switch (eTestPicture::display(curmode))
		{
		case 1: curmode = eTestPicture::testRed; break;
		case 2: curmode = eTestPicture::testGreen; break;
		case 3: curmode = eTestPicture::testBlue; break;
		case 4: curmode = eTestPicture::testColorbar; break;
		case 5: curmode = eTestPicture::testWhite; break;
		case 6: curmode = eTestPicture::testFUBK; break;
		case 7: curmode = eTestPicture::testGray; break;
		case 8: curmode = eTestPicture::testBlack; break;
		default: curmode = -1;
		}
	}
	show();
}

class eZapSetupSelectN
{
	int n;
public:
	eZapSetupSelectN(int n): n(n) { }
	bool operator()(eListBoxEntryMenu &e)
	{
		if (!n--)
		{
			e.selected();
			return 1;
		}
		return 0;
	}
};

void eZapSetup::sel_num(int n)
{
	list.forEachEntry(eZapSetupSelectN(n));
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

