/*
 * $Header: /cvs/tuxbox/apps/dvb/zapit/include/zapit/client/zapittypes.h,v 1.8.2.1 2003/02/18 15:16:46 alexw Exp $
 *
 * zapit's types which are used by the clientlib - d-box2 linux project
 *
 * (C) 2002 by thegoodguy <thegoodguy@berlios.de>
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

#ifndef __zapittypes_h__
#define __zapittypes_h__


#include <stdint.h>

typedef uint16_t t_service_id;
#define SCANF_SERVICE_ID_TYPE "%hx"

typedef uint16_t t_original_network_id;
#define SCANF_ORIGINAL_NETWORK_ID_TYPE "%hx"

typedef uint16_t t_transport_stream_id;

/* unique channel identification */
typedef uint32_t t_channel_id;
#define CREATE_CHANNEL_ID ((original_network_id << 16) | service_id)
#define PRINTF_CHANNEL_ID_TYPE "%08x"

/* diseqc types */
enum diseqc_t
{
	NO_DISEQC,
	MINI_DISEQC,
	DISEQC_1_0,
	DISEQC_1_1,
	SMATV_REMOTE_TUNING
};

/* video display formats (cf. video_displayformat_t in driver/dvb/include/linux/dvb/video.h): */
typedef enum {   
        ZAPIT_VIDEO_PAN_SCAN,       /* use pan and scan format */
	ZAPIT_VIDEO_LETTER_BOX,     /* use letterbox format */
	ZAPIT_VIDEO_CENTER_CUT_OUT  /* use center cut out format */
} video_display_format_t;

#endif /* __zapittypes_h__ */
