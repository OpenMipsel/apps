/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


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


#ifndef __keychooser__
#define __keychooser__

#include <string>

#include <driver/framebuffer.h>
#include <driver/fontrenderer.h>
#include <driver/rcinput.h>
#include <system/localize.h>

#include "menue.h"


class CKeyChooserItem;
class CKeyChooserItemNoKey;
class CKeyChooser : public CMenuWidget
{
	private:
		CFrameBuffer		*frameBuffer;
		neutrino_msg_t*		key;
		std::string		keyName;
		CKeyChooserItem		*keyChooser;
		CKeyChooserItemNoKey	*keyDeleter;
		CMenuForwarder		*keyChooserForwarder;
		CMenuForwarder		*keyDeleterForwarder;

	public:
		CKeyChooser(neutrino_msg_t * const Key, const neutrino_locale_t title, const std::string & Icon = "");
		~CKeyChooser();

		void paint();
		const std::string & getKeyName(){return keyName;};
};

class CKeyChooserItem : public CMenuTarget
{
	private:

		int x;
		int y;
		int width;
		int height;

		neutrino_locale_t name;
		neutrino_msg_t *  key;

		void paint();

	public:

		CKeyChooserItem(const neutrino_locale_t Name, neutrino_msg_t *Key);

		void hide();
		int exec(CMenuTarget* parent, const std::string & actionKey);

};

class CKeyChooserItemNoKey : public CMenuTarget
{
		neutrino_msg_t	*key;

	public:

		CKeyChooserItemNoKey(neutrino_msg_t *Key)
		{
			key=Key;
		};

		int exec(CMenuTarget*, const std::string &)
		{
			*key=CRCInput::RC_nokey;
			return menu_return::RETURN_REPAINT;
		}

};


#endif
