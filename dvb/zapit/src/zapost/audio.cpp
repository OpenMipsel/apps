/*
 * $Id: audio.cpp,v 1.12.2.3 2003/03/27 14:35:43 thegoodguy Exp $
 *
 * (C) 2002 by Steffen Hehn 'McClean' &
 *	Andreas Oberritter <obi@tuxbox.org>
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

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <zapit/audio.h>
#include <zapit/debug.h>
#include <zapit/settings.h>



CAudio::CAudio(void)
{
	initialized = false;

	if ((fd = open(AUDIO_DEVICE, O_RDWR)) < 0)
	{
		ERROR(AUDIO_DEVICE);
	}
	else if (ioctl(fd, AUDIO_GET_STATUS, &status) < 0)
	{
		ERROR("AUDIO_GET_STATUS");
		close(fd);
	}
	else
	{
		initialized = true;
	}
}

CAudio::~CAudio(void)
{
	if (initialized)
		close(fd);
}

int CAudio::setBypassMode(bool enable)
{
	if (ioctl(fd, AUDIO_SET_BYPASS_MODE, enable ? 0 : 1) < 0)
	{
		ERROR("AUDIO_SET_BYPASS_MODE");
		return -1;
	}

	status.bypassMode = enable;

	return 0;
}

int CAudio::setMute(bool enable)
{
	if (ioctl(fd, AUDIO_SET_MUTE, enable) < 0)
	{
		ERROR("AUDIO_SET_MUTE");
		return -1;
	}

	status.muteState = enable;

	return 0;
}

int CAudio::mute(void)
{
	if (status.muteState == false)
		return setMute(true);

	return -1;

}

int CAudio::unmute(void)
{
	if (status.muteState == true)
		return setMute(false);

	return -1;
}

int CAudio::enableBypass(void)
{
	if (status.bypassMode == false)
		return setBypassMode(true);

	return -1;
}

int CAudio::disableBypass(void)
{
	if (status.bypassMode == true)
		return setBypassMode(false);

	return -1;
}

int CAudio::setVolume(unsigned char left, unsigned char right)
{
	if ((mixer.volume_left == left) && (mixer.volume_right == right))
		return 0;

	mixer.volume_left = left;
	mixer.volume_right = right;

	if (ioctl(fd, AUDIO_SET_MIXER, &mixer) < 0)
	{
		perror("AUDIO_SET_MIXER");
		return -1;
	}

	return -1;
}

int CAudio::setSource(audio_stream_source_t source)
{
	if (status.streamSource == source)
		return 0;

	if (status.playState != AUDIO_STOPPED)
		return -1;

	if (ioctl(fd, AUDIO_SELECT_SOURCE, source) < 0)
	{
		ERROR("AUDIO_SELECT_SOURCE");
		return -1;
	}

	status.streamSource = source;
	return 0;
}

audio_stream_source_t CAudio::getSource(void)
{
	struct audio_status status;
	fop(ioctl, AUDIO_GET_STATUS, &status);
	return status.streamSource;
}

int CAudio::start(void)
{
	if (status.playState == AUDIO_PLAYING)
		return 0;

	if (ioctl(fd, AUDIO_PLAY) < 0)
	{
		ERROR("AUDIO_PLAY");
		return -1;
	}

	status.playState = AUDIO_PLAYING;

	return 0;
}

int CAudio::stop(void)
{
	if (status.playState == AUDIO_STOPPED)
		return 0;

	if (ioctl(fd, AUDIO_STOP) < 0)
	{
		ERROR("AUDIO_STOP");
		return -1;
	}

	status.playState = AUDIO_STOPPED;

	return 0;
}

int CAudio::setChannel(audio_channel_select_t channel)
{
	return fop(ioctl, AUDIO_CHANNEL_SELECT, channel);
}

audio_channel_select_t CAudio::getChannel(void)
{
	struct audio_status status;
	fop(ioctl, AUDIO_GET_STATUS, &status);
	return status.channelSelect;
}

