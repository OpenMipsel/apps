/*
 * $Id: time_date_section.cpp,v 1.2 2004/02/13 17:51:08 obi Exp $
 *
 * Copyright (C) 2002-2004 Andreas Oberritter <obi@saftware.de>
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

#include <dvbsi++/byte_stream.h>
#include <dvbsi++/time_date_section.h>

TimeAndDateSection::TimeAndDateSection(const uint8_t * const buffer) : ShortSection(buffer)
{
	utcTimeMjd = UINT16(&buffer[3]);
	utcTimeBcd = (buffer[5] << 16) | UINT16(&buffer[6]);
}

uint16_t TimeAndDateSection::getUtcTimeMjd(void) const
{
	return utcTimeMjd;
}

uint32_t TimeAndDateSection::getUtcTimeBcd(void) const
{
	return utcTimeBcd;
}

