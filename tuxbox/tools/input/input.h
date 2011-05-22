/*
 * $Id: input.h,v 1.3 2011/05/22 15:14:21 rhabarber1848 Exp $
 *
 * input - d-box2 linux project
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
*/

#ifndef __MSGBOX_H__

#define __MSGBOX_H__

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/un.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_CACHE_SMALL_BITMAPS_H
#if (FREETYPE_MAJOR > 2 || (FREETYPE_MAJOR == 2 && (FREETYPE_MINOR > 1 || (FREETYPE_MINOR == 1 && FREETYPE_PATCH >= 8))))
#define FT_NEW_CACHE_API
#endif


#include "color.h"
//freetype stuff

extern unsigned char FONT[64];

enum {LEFT, CENTER, RIGHT};
enum {SMALL, MED, BIG};
//enum {FT_PIC=1, FT_TEXT, FT_HTML, FT_TXHTM};

FT_Error 		error;
FT_Library		library;
FTC_Manager		manager;
FTC_SBitCache		cache;
FTC_SBit		sbit;
#if FT_NEW_CACHE_API
FTC_ImageTypeRec	desc;
#else
FTC_Image_Desc		desc;
#endif
FT_Face			face;
FT_UInt			prev_glyphindex;
FT_Bool			use_kerning;

//devs

int fb;

//framebuffer stuff

enum {FILL, GRID};

#define CMCST	COL_MENUCONTENTSELECTED+7
#define CMCS	COL_MENUCONTENTSELECTED
#define CMCT	COL_MENUCONTENT+7
#define CMC	COL_MENUCONTENT
#define CMCIT	COL_MENUCONTENTINACTIVE+7
#define CMCI	COL_MENUCONTENTINACTIVE
#define CMHT	COL_MENUHEAD+7
#define CMH	COL_MENUHEAD
#define WHITE	0x10
#define BLUE0	0x0D
#define BLUE1	0x0F
#define TRANSP	0xFF
#define FLASH	0x50
#define CMS	COL_MENUCONTENTSELECTED
#define GREEN	0x03
#define YELLOW	0x04
#define RED	0x02

extern unsigned char *lfb, *lbb, *obb;

struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

int startx, starty, sx, ex, sy, ey;
static unsigned sc[8], tc[8];
extern char *butmsg[3];
extern int buttons,selection;

#define FB_DEVICE	"/dev/fb/0"

#endif
