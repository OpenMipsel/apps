/*
	$Id: scan_setup.h,v 1.8 2011/12/09 22:36:28 dbt Exp $

	Copyright (C) 2009 Thilo Graf (dbt)
	http://www.dbox2-tuning.de

	Neutrino-GUI  -   DBoxII-Project

	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __scan_setup__
#define __scan_setup__

#include <gui/widget/menue.h>

#include <zapit/settings.h>

#include <string>

class CScanSetup : public CMenuTarget
{
	private:
		int width, selected;

		uint sat_list_size;
		uint provider_list_size;

		int showScanService();
		void showScanModeMenue();

	public:	
		CScanSetup();
		~CScanSetup();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void initScanSettings();
		std::string getScanModeString(const int& scan_type);
};

#endif
