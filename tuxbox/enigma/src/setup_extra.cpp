/*
 * setup_extra.cpp
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
 * $Id: setup_extra.cpp,v 1.1.2.11 2003/08/07 19:18:49 ghostrider Exp $
 */

#include <setup_extra.h>
#include <enigma.h>
#include <setupengrab.h>
#include <setupnetwork.h>
#include <software_update.h>
#include <setupskin.h>
#include <setup_rc.h>
#include <lib/gui/emessage.h>

eExpertSetup::eExpertSetup()
	:eSetupWindow(_("Expert Setup"), 10, 400)
{
	cmove(ePoint(170, 145));

	int entry=0;
#ifndef DISABLE_NETWORK
	CONNECT((new eListBoxEntryMenu(&list, _("Communication Setup"), eString().sprintf("(%d) %s", ++entry, _("open on screen display settings")) ))->selected, eExpertSetup::communication_setup);
	CONNECT((new eListBoxEntryMenu(&list, _("Ngrab Streaming Setup"), eString().sprintf("(%d) %s", ++entry, _("open on screen display settings")) ))->selected, eExpertSetup::ngrab_setup);
	CONNECT((new eListBoxEntryMenu(&list, _("Software Update"), eString().sprintf("(%d) %s", ++entry, _("open on software update")) ))->selected, eExpertSetup::software_update);
#endif
	CONNECT((new eListBoxEntryMenu(&list, _("Remote Control"), eString().sprintf("(%d) %s", ++entry, _("open on audio/video settings")) ))->selected, eExpertSetup::rc_setup);
	CONNECT((new eListBoxEntryMenu(&list, _("Skin Setup"), eString().sprintf("(%d) %s", ++entry, _("open skin setup")) ))->selected, eExpertSetup::skin_setup);
	new eListBoxEntrySeparator( (eListBox<eListBoxEntry>*)&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	new eListBoxEntryCheck( (eListBox<eListBoxEntry>*)&list, _("Skip confirmations"), "/elitedvb/extra/profimode", _("enable/disable confirmations"));
	new eListBoxEntryCheck( (eListBox<eListBoxEntry>*)&list, _("Hide error windows"), "/elitedvb/extra/hideerror", _("enable/disable showing the zap error messages"));
	CONNECT((new eListBoxEntryCheck((eListBox<eListBoxEntry>*)&list,_("Serviceselector help buttons"),"/ezap/serviceselector/showButtons",	_("show/hide service selector (color) help buttons")))->selected, eExpertSetup::colorbuttonsChanged );
	setHelpID(92);
}

void eExpertSetup::colorbuttonsChanged(bool b)
{
	eServiceSelector *sel = eZap::getInstance()->getServiceSelector();
	sel->setStyle( sel->getStyle(), true );
}

#ifndef DISABLE_NETWORK
void eExpertSetup::communication_setup()
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

void eExpertSetup::ngrab_setup()
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

void eExpertSetup::software_update()
{
	hide();
	eSoftwareUpdate setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

#endif

void eExpertSetup::rc_setup()
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

void eExpertSetup::skin_setup()
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
