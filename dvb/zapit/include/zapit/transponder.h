/*
 * $Id: transponder.h,v 1.3 2002/05/15 22:14:33 McClean Exp $
 *
 * (C) 2002 by Steffen Hehn "McClean" <McClean@tuxbox.org>
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

#ifndef __transponder_h__
#define __transponder_h__



class CTransponder
{
	private:
		unsigned int	frequency;
		unsigned char	modulation;
		unsigned int	symbolrate;
		unsigned char	polarisation;
		unsigned char	innerFec;
		unsigned char	diseqc;

		unsigned short	originalNetworkId;
		unsigned short	transportStreamId;

	public:

		CTransponder();

		unsigned int getFrequency();
		void setFrequency(unsigned int);

		unsigned char getModulation();
		void setModulation(unsigned char);

		unsigned int getSymbolrate();
		void setSymbolrate(unsigned int);

		unsigned char getPolarisation();
		void setPolarisation(unsigned char);

		unsigned char getDiseqc();
		void setDiseqc(unsigned char);

		unsigned char getInnerFec();
		void setInnerFec(unsigned char);

		unsigned short getOriginalNetworkId();
		void setOriginalNetworkId(unsigned short);

		unsigned short getTransportStreamId();
		void setTransportStreamId(unsigned short);

		unsigned int getTsidOnid();
};

#endif
