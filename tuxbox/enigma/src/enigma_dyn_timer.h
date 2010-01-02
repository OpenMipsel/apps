/*
 * $Id: enigma_dyn_timer.h,v 1.7.2.2 2010/01/02 17:21:04 fergy Exp $
 *
 * (C) 2005 by digi_casi <digi_casi@tuxbox.org>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
 
#ifndef __enigma_dyn_timer_h
#define __enigma_dyn_timer_h

class eHTTPDynPathResolver;
void ezapTimerInitializeDyn(eHTTPDynPathResolver *dyn_resolver, bool lockWeb);
eString getTimerList(eString format);

#endif /* __enigma_dyn_timer_h */

