/*
 * system_settings.cpp
 *
 * Copyright (C) 2003 Andreas Monzner <ghostrider@tuxbox.org>
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
 * $Id: system_settings.cpp,v 1.1.2.1 2003/07/31 17:07:07 ghostrider Exp $
 */

#include <system_settings.h>
#include <setup_osd.h>
#include <time_settings.h>
#include <setup_rfmod.h>
#include <setup_lcd.h>
#include <setupvideo.h>

eSystemSettings::eSystemSettings()
	:eSetupWindow(_("System Settings"), 7, 350)
{
	move(ePoint(130, 120));
	int entry=0;
	CONNECT((new eListBoxEntryMenu(&list, _("OSD Settings"), eString().sprintf("(%d) %s", ++entry, _("open on screen display settings")) ))->selected, eSystemSettings::osd_settings);
	CONNECT((new eListBoxEntryMenu(&list, _("Time Settings"), eString().sprintf("(%d) %s", ++entry, _("open on screen display settings")) ))->selected, eSystemSettings::time_settings);
#ifdef ENABLE_RFMOD
	CONNECT((new eListBoxEntryMenu(&list, _("UHF Modulator"), eString().sprintf("(%d) %s", ++entry, _("open on audio/video settings")) ))->selected, eSystemSettings::uhf_modulator);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("A/V Settings"), eString().sprintf("(%d) %s", ++entry, _("open on audio/video settings")) ))->selected, eSystemSettings::av_settings);
#ifndef DISABLE_LCD
	CONNECT((new eListBoxEntryMenu(&list, _("LCD Settings"), eString().sprintf("(%d) %s", ++entry, _("open on audio/video settings")) ))->selected, eSystemSettings::lcd_settings);
#endif
	new eListBoxEntrySeparator( (eListBox<eListBoxEntry>*)&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	new eListBoxEntryCheck( (eListBox<eListBoxEntry>*)&list, _("Expert Mode"), "/ezap/extra/expertmode", _("enable/disable expert setup") );
}

void eSystemSettings::osd_settings()
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

void eSystemSettings::time_settings()
{
	hide();
	eTimeSettings setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

void eSystemSettings::av_settings()
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
void eSystemSettings::uhf_modulator()
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

#ifndef DISABLE_LCD
void eSystemSettings::lcd_settings()
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

