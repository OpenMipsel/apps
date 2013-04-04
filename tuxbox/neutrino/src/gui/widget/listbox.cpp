/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

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
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gui/widget/listbox.h>

#include <global.h>
#include <neutrino.h>

#include <gui/widget/icons.h>
#include <driver/screen_max.h>

CListBox::CListBox(const char * const Caption)
{
	frameBuffer = CFrameBuffer::getInstance();
	caption = Caption;
	liststart = 0;
	selected =  0;
	width = w_max (400, 100);
	height = 420;
	ButtonHeight = 25;
	modified = false;
	theight     = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	fheight     = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height-theight-0)/fheight;
	height = theight+0+listmaxshow*fheight; // recalc height
	x = getScreenStartX (width);
	y = getScreenStartY (height);
}

void CListBox::setModified(void)
{
	modified = true;
}

void CListBox::paint()
{
	int c_rad_small = RADIUS_SMALL;;
	liststart = (selected/listmaxshow)*listmaxshow;
	int ypos = y+ theight;
	int sb = fheight* listmaxshow;

	frameBuffer->paintBoxRel(x, ypos, width, sb, COL_MENUCONTENT_PLUS_0); //mainframe
	frameBuffer->paintBoxRel(x+ width- 15,ypos, 15, sb,  COL_MENUCONTENT_PLUS_1);

	for(unsigned int count=0;count<listmaxshow;count++)
	{
		paintItem(count);
	}

	int sbc= ((getItemCount()- 1)/ listmaxshow)+ 1;
	int sbs= (selected/listmaxshow);

	frameBuffer->paintBoxRel(x+ width- 13, ypos+ 2+ sbs*(sb-4)/sbc, 11, (sb-4)/sbc, COL_MENUCONTENT_PLUS_3, c_rad_small);
}

void CListBox::paintHead()
{
	frameBuffer->paintBoxRel(x,y, width,theight+0, COL_MENUHEAD_PLUS_0);
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + 10, y + theight + 2, width, caption.c_str(), COL_MENUHEAD);
}

void CListBox::paintFoot()
{
	int ButtonWidth = width / 4;
	frameBuffer->paintBoxRel(x,y+height, width,ButtonHeight, COL_MENUHEAD_PLUS_0);
	frameBuffer->paintHLine(x, x+width,  y, COL_INFOBAR_SHADOW_PLUS_0);

	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_OKAY, x+width- 4* ButtonWidth+ 8, y+height+1);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x+width- 4* ButtonWidth+ 38, y+height+24 - 2, width, "edit", COL_INFOBAR);

	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_GREEN, x+width- 3* ButtonWidth+ 8, y+height+4);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x+width- 3* ButtonWidth+ 29, y+height+24 - 2, width- 26, "add", COL_INFOBAR);

	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_RED, x+width- 2* ButtonWidth+ 8, y+height+4);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x+width- 2* ButtonWidth+ 29, y+height+24 - 2, width- 26, "remove", COL_INFOBAR);

	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_HOME, x+width - ButtonWidth+ 8, y+height+1);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x+width - ButtonWidth+ 38, y+height+24 - 2, width, "ready", COL_INFOBAR);
}

void CListBox::paintItem(int pos)
{
	paintItem(liststart+pos, pos, (liststart+pos==selected) );
}

void CListBox::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height+ButtonHeight);
}

void CListBox::updateSelection(unsigned int newpos)
{
	if (selected != newpos)
	{
		unsigned int prev_selected = selected;
		unsigned int oldliststart = liststart;

		selected = newpos;
		liststart = (selected / listmaxshow) * listmaxshow;
		if (oldliststart != liststart)
			paint();
		else
		{
			paintItem(prev_selected - liststart);
			paintItem(selected - liststart);
		}
	}
}

unsigned int	CListBox::getItemCount()
{
	return 10;
}

int CListBox::getItemHeight()
{
	return fheight;
}

void CListBox::paintItem(unsigned int /*itemNr*/, int paintNr, bool _selected)
{
	int ypos = y+ theight + paintNr*getItemHeight();

	uint8_t    color;
	fb_pixel_t bgcolor;

	if (_selected)
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
	}
	else
	{
		color   = COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
	}

	frameBuffer->paintBoxRel(x,ypos, width- 15, getItemHeight(), bgcolor);
	g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 10, ypos+ fheight, width-20, "demo", color);
}

int CListBox::exec(CMenuTarget* parent, const std::string &)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = menu_return::RETURN_REPAINT;
	selected=0;

	if (parent)
	{
		parent->hide();
	}

	paintHead();
	paint();
	paintFoot();

	bool loop=true;
	modified = false;
	while (loop)
	{
		g_RCInput->getMsg(&msg, &data, g_settings.timing[SNeutrinoSettings::TIMING_EPG]);
		neutrino_msg_t msg_repeatok = msg & ~CRCInput::RC_Repeat;

		if (msg == g_settings.key_channelList_cancel || msg == CRCInput::RC_home)
		{
			loop = false;
		}
		else if (msg_repeatok == CRCInput::RC_up || msg_repeatok == g_settings.key_channelList_pageup)
		{
			int itemCount = getItemCount();
			if (itemCount > 0)
			{
				int step = (msg_repeatok == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
				int new_selected = selected - step;
				if (new_selected < 0)
					new_selected = itemCount - 1;
				updateSelection(new_selected);
			}
		}
		else if (msg_repeatok == CRCInput::RC_down || msg_repeatok == g_settings.key_channelList_pagedown)
		{
			unsigned int itemCount = getItemCount();
			if (itemCount > 0)
			{
				unsigned int step = (msg_repeatok == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
				unsigned int new_selected = selected + step;
				if (new_selected >= itemCount)
				{
					if ((itemCount / listmaxshow + 1) * listmaxshow == itemCount + listmaxshow) // last page has full entries
						new_selected = 0;
					else
						new_selected = ((step == listmaxshow) && (new_selected < ((itemCount / listmaxshow + 1) * listmaxshow))) ? (itemCount - 1) : 0;
				}
				updateSelection(new_selected);
			}
		}
		else if( msg ==CRCInput::RC_ok)
		{
			onOkKeyPressed();
		}
		else if ( msg ==CRCInput::RC_red)
		{
			onRedKeyPressed();
		}
		else if ( msg ==CRCInput::RC_green)
		{
			onGreenKeyPressed();
		}
		else if ( msg ==CRCInput::RC_yellow)
		{
			onYellowKeyPressed();
		}
		else if ( msg ==CRCInput::RC_blue)
		{
			onBlueKeyPressed();
		}
		else
		{
			CNeutrinoApp::getInstance()->handleMsg( msg, data );
			// kein canceling...
		}
	}

	hide();
	return res;
}
