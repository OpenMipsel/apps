/*
 * setup_av.cpp
 *
 * Copyright (C) 2003 Felix Domke <tmbinc@tuxbox.org>
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
 * $Id: setup_av.cpp,v 1.1.2.1 2003/05/30 22:36:54 ghostrider Exp $
 */

#include <setup_av.h>

#include <setupvideo.h>
#include <setup_rfmod.h>
#include <setup_audio.h>
#include <lib/base/i18n.h>
#include <lib/dvb/edvb.h>
#include <lib/gui/emessage.h>
#include <lib/gui/testpicture.h>

eZapAVSetup::eZapAVSetup()
	:eListBoxWindow<eListBoxEntryMenu>(_("A/V Setup"), 6, 300, true)
{
	move(ePoint(135, 120));

	int entry=0;

	CONNECT((new eListBoxEntryMenu(&list, _("[back]"), eString().sprintf("(%d) %s", ++entry, _("back to setup")) ))->selected, eWidget::accept);
	CONNECT((new eListBoxEntryMenu(&list, _("Video..."), eString().sprintf("(%d) %s", ++entry, _("open video setup")) ))->selected, eZapAVSetup::sel_video);
	CONNECT((new eListBoxEntryMenu(&list, _("Video calibration..."), eString().sprintf("(%d) %s", ++entry, _("show calibration picture")) ))->selected, eZapAVSetup::sel_test);
#ifdef ENABLE_RFMOD
	if (eDVB::getInstance()->getmID() == 6)
		CONNECT((new eListBoxEntryMenu(&list, _("RF-Modulator..."), eString().sprintf("(%d) %s", ++entry, _("setup modulator")) ))->selected, eZapAVSetup::sel_rfmod);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Audio..."), eString().sprintf("(%d) %s", ++entry, _("open audio setup")) ))->selected, eZapAVSetup::sel_sound);

	addActionMap(&i_shortcutActions->map);
}

void eZapAVSetup::sel_sound()
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

void eZapAVSetup::sel_video()
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

#ifdef ENABLE_RFMOD
void eZapAVSetup::sel_rfmod()
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

void eZapAVSetup::sel_test()
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

int eZapAVSetup::eventHandler(const eWidgetEvent &event)
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

void eZapAVSetup::sel_num(int n)
{
	list.forEachEntry(eZapSetupSelectN(n));
}
