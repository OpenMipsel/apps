/*
 * enigma_scan.cpp
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
 * $Id: enigma_scan.cpp,v 1.10.2.13 2003/04/13 13:03:07 ghostrider Exp $
 */

#include <enigma_scan.h>

#include <satconfig.h>
#include <rotorconfig.h>
#include <scan.h>
#include <tpeditwindow.h>
#include <lib/dvb/edvb.h>
#include <lib/dvb/frontend.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/eskin.h>
#include <lib/gui/elabel.h>
#include <lib/gui/emessage.h>
#include <lib/base/i18n.h>

eZapScan::eZapScan()
	:eListBoxWindow<eListBoxEntryMenu>(_("Channels"), 6, 300, true)
{
	move(ePoint(150, 136));
	CONNECT((new eListBoxEntryMenu(&list, _("[back]"), _("back to main menu")))->selected, eZapScan::sel_close);
	CONNECT((new eListBoxEntryMenu(&list, _("Transponder scan"), _("goto transponder scan")))->selected, eZapScan::sel_scan);	
	if ( eFrontend::getInstance()->Type() == eFrontend::feSatellite )  // only when a sat box is avail we shows a satellite config
	{
		CONNECT((new eListBoxEntryMenu(&list, _("Transponders..."), _("goto transponder edit dialog")))->selected, eZapScan::sel_tpeditdlg);
		CONNECT((new eListBoxEntryMenu(&list, _("Satellites..."), _("goto satellite config")))->selected, eZapScan::sel_satconfig);
		CONNECT((new eListBoxEntryMenu(&list, _("Motor Setup..."), _("goto Motor Setup")))->selected, eZapScan::sel_rotorConfig);
	}
	CONNECT((new eListBoxEntryMenu(&list, _("SID...")))->selected, eZapScan::sel_bouquet);
}

eZapScan::~eZapScan()
{
}

void eZapScan::sel_close()
{
	close(0);
}

void eZapScan::sel_scan()
{
	TransponderScan setup(LCDTitle, LCDElement);
	hide();
	setup.exec();
	show();
}

void eZapScan::sel_bouquet()
{
	FILE *f=fopen("/scheissteil.sdx", "rt");

	int parsed=0, error=0;
	if (f)
	{
		while (1)
		{
			char line[129];
			if (fread(line, 128, 1, f) != 1)
				break;
			line[128]=0;
			int res=eDVB::getInstance()->settings->importSatcoDX(line);
			if (!res)
				parsed++;
			else
				error++;
		}
		eDebug("parsed %d lines ok, %d errornous.", parsed, error);
		fclose(f);
	}
}

void eZapScan::sel_satconfig()
{
	hide();
	eSatelliteConfigurationManager satconfig;
	satconfig.setLCD(LCDTitle, LCDElement);
	satconfig.show();
	satconfig.exec();
	satconfig.hide();
	show();
}

void eZapScan::sel_tpeditdlg()
{
	hide();
	eTransponderEditWindow wnd;
	wnd.setLCD(LCDTitle, LCDElement);
	wnd.show();
	wnd.exec();
	wnd.hide();
	show();
}


eLNB* eZapScan::getRotorLNB(int silent)
{
	int c=0;
	std::list<eLNB>::iterator RotorLnb = eTransponderList::getInstance()->getLNBs().end();
	std::list<eLNB>::iterator it( eTransponderList::getInstance()->getLNBs().begin());
	for (; it != eTransponderList::getInstance()->getLNBs().end(); it++ )
	{
		if ( it->getDiSEqC().DiSEqCMode == eDiSEqC::V1_2 )
		{
			if (!c++)
				RotorLnb=it;
		}
	}
	if ( c > 1 )  // we have more than one LNBs with DiSEqC 1.2
	{
		eMessageBox mb( _("More than one LNB have DiSEqC 1.2 enabled, please select the LNB where the motor is connected"), _("Info"), eMessageBox::iconWarning|eMessageBox::btOK );
		mb.show();
		mb.exec();
		mb.hide();
		eLNBSelector sel;
		sel.show();
		int ret = sel.exec();
		sel.hide();
		return (eLNB*) ret;
	}
	else if ( !c )
	{
		if (!silent)
		{
			eMessageBox mb( _("Found no LNB with DiSEqC 1.2 enabled,\nplease goto Satellite Config first, and enable DiSEqC 1.2"), _("Warning"), eMessageBox::iconWarning|eMessageBox::btOK );
			mb.show();
			mb.exec();
			mb.hide();
		}
		return 0;
	}
	else // only one lnb with DiSEqC 1.2 is found.. this is correct :)
		return &(*RotorLnb);
}

void eZapScan::sel_rotorConfig()
{
	hide();
	eLNB* lnb = getRotorLNB(0);
	if (lnb)
	{
		RotorConfig c(lnb);
		c.setLCD( LCDTitle, LCDElement );
		c.show();
		c.exec();
		c.hide();
	}
	show();
}

eLNBSelector::eLNBSelector()
	:eListBoxWindow<eListBoxEntryText>(_("Select LNB"), 5, 300, true)
{
	move(ePoint(150, 136));
	new eListBoxEntryText(&list, _("[back]"), 0, 0, _("go to prev menu") );
	int cnt=0;
	for ( std::list<eLNB>::iterator it( eTransponderList::getInstance()->getLNBs().begin()); it != eTransponderList::getInstance()->getLNBs().end(); it++, cnt++ )
	{
		if ( it->getDiSEqC().DiSEqCMode == eDiSEqC::V1_2 )
			new eListBoxEntryText( &list, eString().sprintf("LNB %d", cnt), (void*)&(*it), 0, eString().sprintf(_("use LNB %d for Motor"), cnt ).c_str());
	}
	CONNECT( list.selected, eLNBSelector::selected );
}

void eLNBSelector::selected( eListBoxEntryText *e )
{
	if ( e && e->getKey() )
		close((int)e->getKey());
	else
		close(0);
}
