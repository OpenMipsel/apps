/*
	$Id: audioplayer_setup.cpp,v 1.7 2011/12/09 22:36:27 dbt Exp $

	audioplayer setup implementation - Neutrino-GUI

	Copyright (C) 2001 Steffen Hehn 'McClean'
	and some other guys
	Homepage: http://dbox.cyberphoria.org/

	Copyright (C) 2009 T. Graf 'dbt'
	Homepage: http://www.dbox2-tuning.net/


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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "gui/audioplayer_setup.h"

#include <global.h>
#include <neutrino.h>

#include <gui/widget/icons.h>
#include <gui/widget/stringinput.h>

#include "gui/audioplayer.h"
#include "gui/filebrowser.h"

#include <driver/screen_max.h>

#include <system/debug.h>



CAudioPlayerSetup::CAudioPlayerSetup()
{
	width = w_max (500, 100);
	selected = -1;
}

CAudioPlayerSetup::~CAudioPlayerSetup()
{

}

int CAudioPlayerSetup::exec(CMenuTarget* parent, const std::string &actionKey)
{
	dprintf(DEBUG_DEBUG, "init audioplayer setup\n");
	int   res = menu_return::RETURN_REPAINT;

	if (parent)
	{
		parent->hide();
	}

	if(actionKey == "audioplayerdir")
	{
		parent->hide();
		CFileBrowser b;
		b.Dir_Mode=true;
		if (b.exec(g_settings.audioplayer_audioplayerdir))
			strncpy(g_settings.audioplayer_audioplayerdir, b.getSelectedFile()->Name.c_str(), sizeof(g_settings.audioplayer_audioplayerdir)-1);
		return res;
	}

	res = showAudioPlayerSetup();
	
	return res;
}

#define MESSAGEBOX_NO_YES_OPTION_COUNT 2
const CMenuOptionChooser::keyval MESSAGEBOX_NO_YES_OPTIONS[MESSAGEBOX_NO_YES_OPTION_COUNT] =
{
	{ 0, LOCALE_MESSAGEBOX_NO  },
	{ 1, LOCALE_MESSAGEBOX_YES }
};


#define AUDIOPLAYER_DISPLAY_ORDER_OPTION_COUNT 2
const CMenuOptionChooser::keyval AUDIOPLAYER_DISPLAY_ORDER_OPTIONS[AUDIOPLAYER_DISPLAY_ORDER_OPTION_COUNT] =
{
	{ CAudioPlayerGui::ARTIST_TITLE, LOCALE_AUDIOPLAYER_ARTIST_TITLE },
	{ CAudioPlayerGui::TITLE_ARTIST, LOCALE_AUDIOPLAYER_TITLE_ARTIST }
};


int CAudioPlayerSetup::showAudioPlayerSetup()
/*shows the audio setup menue*/
{
	CMenuWidget* audioplayerSetup = new CMenuWidget(LOCALE_MAINMENU_SETTINGS, NEUTRINO_ICON_SETTINGS, width);
	audioplayerSetup->setPreselected(selected);
	audioplayerSetup->addItem( new CMenuSeparator(CMenuSeparator::ALIGN_LEFT | CMenuSeparator::SUB_HEAD | CMenuSeparator::STRING, LOCALE_MAINMENU_AUDIOPLAYER));

	// intros
	audioplayerSetup->addItem(GenericMenuSeparator);
	audioplayerSetup->addItem(GenericMenuBack);
	audioplayerSetup->addItem(GenericMenuSeparatorLine);

	// display order
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_DISPLAY_ORDER, &g_settings.audioplayer_display , AUDIOPLAYER_DISPLAY_ORDER_OPTIONS, AUDIOPLAYER_DISPLAY_ORDER_OPTION_COUNT, true ));
	// auto select current
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_FOLLOW   , &g_settings.audioplayer_follow  , MESSAGEBOX_NO_YES_OPTIONS  , MESSAGEBOX_NO_YES_OPTION_COUNT  , true ));
	// search title by name/tilte
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_SELECT_TITLE_BY_NAME   , &g_settings.audioplayer_select_title_by_name  , MESSAGEBOX_NO_YES_OPTIONS  , MESSAGEBOX_NO_YES_OPTION_COUNT  , true ));
	// auto repeat y/n
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_REPEAT_ON   , &g_settings.audioplayer_repeat_on  , MESSAGEBOX_NO_YES_OPTIONS  , MESSAGEBOX_NO_YES_OPTION_COUNT  , true ));
	// show playlist y/n
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_SHOW_PLAYLIST, &g_settings.audioplayer_show_playlist, MESSAGEBOX_NO_YES_OPTIONS, MESSAGEBOX_NO_YES_OPTION_COUNT, true ));
	// screensaver timeout
	CStringInput audio_screensaver(LOCALE_AUDIOPLAYER_SCREENSAVER_TIMEOUT, g_settings.audioplayer_screensaver, 2, NONEXISTANT_LOCALE, NONEXISTANT_LOCALE, "0123456789 ");
	audioplayerSetup->addItem(new CMenuForwarder(LOCALE_AUDIOPLAYER_SCREENSAVER_TIMEOUT, true, g_settings.audioplayer_screensaver, &audio_screensaver));
	// high decode priority
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_HIGHPRIO , &g_settings.audioplayer_highprio    , MESSAGEBOX_NO_YES_OPTIONS  , MESSAGEBOX_NO_YES_OPTION_COUNT  , true ));
	// start directory
	audioplayerSetup->addItem(new CMenuForwarder(LOCALE_AUDIOPLAYER_DEFDIR, true, g_settings.audioplayer_audioplayerdir, this, "audioplayerdir"));
	// shoutcast metadata parsing y/n
	audioplayerSetup->addItem(new CMenuOptionChooser(LOCALE_AUDIOPLAYER_ENABLE_SC_METADATA, &g_settings.audioplayer_enable_sc_metadata, MESSAGEBOX_NO_YES_OPTIONS, MESSAGEBOX_NO_YES_OPTION_COUNT, true ));

	int res = audioplayerSetup->exec(NULL, "");
	audioplayerSetup->hide();
	selected = audioplayerSetup->getSelected();
	delete audioplayerSetup;

	return res;
}
