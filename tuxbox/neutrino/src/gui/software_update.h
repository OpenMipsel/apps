/*
	$Id: software_update.h,v 1.1 2009/10/09 05:04:06 dbt Exp $

	Neutrino-GUI  -   DBoxII-Project

        Software update implementation - Neutrino-GUI

	Copyright (C) 2001 Steffen Hehn 'McClean'
	and some other guys
	Homepage: http://dbox.cyberphoria.org/

	Copyright (C) 2009 T. Graf 'dbt'
	Homepage: http://www.dbox2-tuning.net/

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

	$Log: software_update.h,v $
	Revision 1.1  2009/10/09 05:04:06  dbt
	init stand alone module for neutrino software update
	
*/

#ifndef __software_update__
#define __software_update__

#include <gui/widget/menue.h>

#include <driver/framebuffer.h>

#include <string>

class CSoftwareUpdate : public CMenuTarget
{
	private:
		CFrameBuffer *frameBuffer;
		int x, y, width, height, menue_width, hheight, mheight;

		void hide();
		void showSoftwareUpdate();
		void showSoftwareUpdateExpert();
		void showSoftwareUpdateImageinfo(CMenuWidget * entry);
		void showSoftwareUpdateProxySetup(CMenuWidget * entry);
	
	public:	
		CSoftwareUpdate();
		~CSoftwareUpdate();
		int exec(CMenuTarget* parent, const std::string & actionKey);
};


#endif
