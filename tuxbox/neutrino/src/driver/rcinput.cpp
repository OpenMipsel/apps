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


#include "rcinput.h"
#include "../global.h"


void printbin( int a)
{
	for(int pos = 15;pos>=0;pos--)
	{
		printf("%d", a&(1<<pos)?1:0);
	}
	printf("\n");
}



/**************************************************************************
*	Constructor - opens rc-input device and starts threads
*
**************************************************************************/
CRCInput::CRCInput()
{
	open();
}

void CRCInput::open()
{
	close();

	//+++++++++++++++++++++++++++++++++++++++
	fd_rc=::open("/dev/dbox/rc0", O_RDONLY);
	if (fd_rc<0)
	{
		perror("/dev/dbox/rc0");
		exit(-1);
	}
	ioctl(fd_rc, RC_IOCTL_BCODES, 1);
	fcntl(fd_rc, F_SETFL, O_NONBLOCK );

	//+++++++++++++++++++++++++++++++++++++++
	fd_keyb = 0;
	/*
	::open("/dev/dbox/rc0", O_RDONLY);
	if (fd_keyb<0)
	{
		perror("/dev/stdin");
		exit(-1);
	}
	*/
	fcntl(fd_keyb, F_SETFL, O_NONBLOCK );

	//+++++++++++++++++++++++++++++++++++++++
	fd_event = 0;
	fd_eventclient = -1;

	//network-setup
	struct sockaddr_un servaddr;
	int clilen;
	memset(&servaddr, 0, sizeof(struct sockaddr_un));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, NEUTRINO_UDS_NAME);
	clilen = sizeof(servaddr.sun_family) + strlen(servaddr.sun_path);
	unlink(NEUTRINO_UDS_NAME);

	//network-setup
	if ((fd_event = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("[neutrino] socket\n");
	}

	if ( bind(fd_event, (struct sockaddr*) &servaddr, clilen) <0 )
	{
		perror("[neutrino] bind failed...\n");
		exit(-1);
	}


	if (listen(fd_event, 5) !=0)
	{
		perror("[neutrino] listen failed...\n");
		exit( -1 );
	}

	calculateMaxFd();
}

void CRCInput::close()
{
	if(fd_rc)
	{
		::close(fd_rc);
	}
/*
	if(fd_keyb)
	{
		::close(fd_keyb);
	}
*/
	if(fd_event)
	{
		::close(fd_event);
	}
}

void CRCInput::calculateMaxFd()
{
	fd_max = fd_rc;
	if(fd_event > fd_max)
		fd_max = fd_event;
	if(fd_eventclient > fd_max)
		fd_max = fd_eventclient;
}


/**************************************************************************
*	Destructor - close the input-device
*
**************************************************************************/
CRCInput::~CRCInput()
{
	close();
}

/**************************************************************************
*	stopInput - stop reading rcin for plugins
*
**************************************************************************/
void CRCInput::stopInput()
{
	close();
}


/**************************************************************************
*	restartInput - restart reading rcin after calling plugins
*
**************************************************************************/
void CRCInput::restartInput()
{
	open();
}

/**************************************************************************
*	get rc-key - timeout can be specified
*
**************************************************************************/
void CRCInput::getMsg(uint *msg, uint *data, int Timeout=-1, bool bAllowRepeatLR= false)
{
	static long long last_keypress=0;
	long long getKeyBegin;
	static __u16 rc_last_key = 0;
	static __u16 rc_last_repeat_key = 0;

	struct timeval tv, tvselect;
	struct timeval *tvslectp;
	int InitialTimeout = Timeout;
	fd_set rfds;
	__u16 rc_key;

	//set 0
	*data = 0;

	//es ist ein event im Buffer - diesen zur�ckgeben
	if( eventlist.size() > 0 )
	{
		vector<event*>::iterator e = eventlist.begin();

		*msg = (*e)->msg;
		*data = (*e)->data;
		eventlist.erase( e );
        delete *e;

		return;
	}

	if(Timeout==-1)
	{
		tvslectp = NULL;
	}
	else
	{
		tvslectp = &tvselect;
	}

	// wiederholung reinmachen - dass wirklich die ganze zeit gis timeout gewartet wird!
	gettimeofday( &tv, NULL );
	getKeyBegin = (long long) tv.tv_usec + (long long)((long long) tv.tv_sec * (long long) 1000000);
	while(1)
	{
		//nicht genau - verbessern!
	    tvselect.tv_sec = Timeout/10;
		tvselect.tv_usec = (Timeout*100000)%1000000;

		FD_ZERO(&rfds);
		FD_SET(fd_rc, &rfds);
		//FD_SET(fd_keyb, &rfds);
		FD_SET(fd_event, &rfds);
		if(fd_eventclient!=-1)
		{
			FD_SET(fd_eventclient, &rfds);
		}
		calculateMaxFd();
		int status =  select(fd_max+1, &rfds, NULL, NULL, tvslectp);
/*
		if(FD_ISSET(fd_keyb, &rfds))
		{
			char key = 0;
			read(fd_keyb, &key, sizeof(key));
			printf("keyboard: %d\n", rc_key);
		}
*/
		if(FD_ISSET(fd_event, &rfds))
		{
			//printf("[neutrino] network - accept!\n");
			socklen_t	clilen;
			SAI			cliaddr;
			clilen = sizeof(cliaddr);
			fd_eventclient = accept(fd_event, (SA *) &cliaddr, &clilen);
		}

		if(fd_eventclient!=-1)
		{
			if(FD_ISSET(fd_eventclient, &rfds))
			{
				*msg = RC_nokey;
				//printf("[neutrino] network - read!\n");
				CEventServer::eventHead emsg;
				int read_bytes= recv(fd_eventclient, &emsg, sizeof(emsg), MSG_WAITALL);
				//printf("[neutrino] event read %d bytes - following %d bytes\n", read_bytes, emsg.dataSize );
				if ( read_bytes == sizeof(emsg) )
				{
                    unsigned char* p;
					p= new unsigned char[ emsg.dataSize ];
					if ( p!=NULL )
					{
						read_bytes= recv(fd_eventclient, p, emsg.dataSize, MSG_WAITALL);
						//printf("[neutrino] eventbody read %d bytes\n", read_bytes );

						if (emsg.eventID==CControldClient::EVT_VOLUMECHANGED)
						{
							*msg = messages::EVT_VOLCHANGED;
							*data = *(char*) p;
						}

						if (emsg.eventID==CControldClient::EVT_MUTECHANGED)
						{
							*msg = messages::EVT_MUTECHANGED;
							*data = *(bool*) p;
						}
						else if (emsg.eventID==CControldClient::EVT_VCRCHANGED)
						{
            	            *msg = messages::EVT_VCRCHANGED;
							*data = *(int*) p;
						}
						else if (emsg.eventID==CControldClient::EVT_MODECHANGED)
						{
            	            *msg = messages::EVT_MODECHANGED;
							*data = *(int*) p;
						}

						delete p;
						p= NULL;
					}

				}
				else
				{
					printf("[neutrino] event - read failed!\n");
				}

				::close(fd_eventclient);
				fd_eventclient = -1;

				if ( *msg != RC_nokey )
				{
					// raus hier :)
					//printf("[neutrino] event 0x%x\n", *msg);
					return;
				}
			}
		}

		if(FD_ISSET(fd_rc, &rfds))
		{
			status = read(fd_rc, &rc_key, sizeof(rc_key));
			if (status==2)
			{
				if(rc_key!=0x5cfe)
				{
					//printf("got key native key: %04x %04x\n", rc_key, rc_key&0x1f );
					long long now_pressed;
					bool keyok = true;

					gettimeofday( &tv, NULL );
					now_pressed = (long long) tv.tv_usec + (long long)((long long) tv.tv_sec * (long long) 1000000);
					//printf("diff: %lld - %lld = %lld should: %d\n", now_pressed, last_keypress, now_pressed-last_keypress, repeat_block);

					//alter nokia-rc-code - lastkey l�schen weil sonst z.b. nicht zweimal nacheinander ok gedr�ckt werden kann
					if((rc_key&0xff00)==0x5c00)
					{
						rc_last_key = 0;
					}
					//test auf wiederholenden key (gedr�ckt gehalten)
					if (rc_key == rc_last_key)
					{
						keyok = false;
						//nur diese tasten sind wiederholbar
						int trkey = translate(rc_key);
						if  ( (trkey==RC_up) || (trkey==RC_down) || (trkey==RC_plus) || (trkey==RC_minus) || (trkey==RC_standby) ||
							  ((bAllowRepeatLR) && ((trkey==RC_left) || (trkey==RC_right))) )
						{
							if( rc_last_repeat_key!=rc_key )
							{
								if(abs(now_pressed-last_keypress)>repeat_block)
								{
									keyok = true;
									rc_last_repeat_key = rc_key;
								}
							}
							else
							{
									keyok = true;
							}
						}
					}
					else
					{
						rc_last_repeat_key = 0;
					}
					rc_last_key = rc_key;
                    //printf("!!!!!!!  native key: %04x %04x\n", rc_key, rc_key&0x1f );
					if(abs(now_pressed-last_keypress)>repeat_block_generic)
					{
						if(keyok)
						{
							last_keypress = now_pressed;
							int trkey= translate(rc_key);
							//printf("--!!!!!  translated key: %04x\n", trkey );
							if (trkey!=RC_nokey)
							{
								*msg = trkey;
								*data = 0;
								return;
							}
						}
					}

				}
			}
		}

		if ( InitialTimeout == 0 )
		{//nicht warten wenn kein key da ist
		   	*msg = RC_timeout;
			*data = 0;
			return;
		}
		else if(tvslectp != NULL)
		{//timeout neu kalkulieren
			gettimeofday( &tv, NULL );
			long long getKeyNow = (long long) tv.tv_usec + (long long)((long long) tv.tv_sec * (long long) 1000000);
			long long diff = abs( (getKeyNow - getKeyBegin) / 100000 );
			Timeout -= diff;
			//printf("[rcin] diff timeout: %lld, %d von %d\n", diff, Timeout, Timeout2);
			if( Timeout <= 0 )
			{
				*msg = RC_timeout;
				*data = 0;
				return;
			}
		}
	}
}



void CRCInput::insertMsgAtTop(uint msg, uint data)
{
	event* tmp = new event();
	tmp->msg 	= msg;
	tmp->data	= data;
	eventlist.insert(eventlist.begin(), tmp);
}

void CRCInput::pushbackMsg(uint msg, uint data)
{
	event* tmp = new event();
	tmp->msg 	= msg;
	tmp->data	= data;
	eventlist.insert(eventlist.end(), tmp);
}


void CRCInput::clearMsg(uint min = 0, uint max = 0xFFFFFFFF )
{
	vector<event*>::iterator e = eventlist.begin();

	while ( e != eventlist.end() )
	{
		if ( ( (*e)->msg >= min ) && ( (*e)->msg <= max ) )
		{
			eventlist.erase( e );
			delete *e;
		}
		else
			e++;
	}
}

/**************************************************************************
*       isNumeric - test if key is 0..9
*
**************************************************************************/
bool CRCInput::isNumeric(int key)
{
	if( (key>=RC_0) && (key<=RC_9))
		return true;
	else
		return false;
}

/**************************************************************************
*       transforms the rc-key to string
*
**************************************************************************/
string CRCInput:: getKeyName(int code)
{
	switch(code)
	{
			case RC_standby:
			return "standby";
			case RC_home:
			return "home";
			case RC_setup:
			return "setup";
			case RC_0:
			return "0";
			case RC_1:
			return "1";
			case RC_2:
			return "2";
			case RC_3:
			return "3";
			case RC_4:
			return "4";
			case RC_5:
			return "5";
			case RC_6:
			return "6";
			case RC_7:
			return "7";
			case RC_8:
			return "8";
			case RC_9:
			return "9";
			case RC_red:
			return "red button";
			case RC_green:
			return "green button";
			case RC_yellow:
			return "yellow button";
			case RC_blue:
			return "blue button";
			case RC_page_up:
			return "page up";
			case RC_page_down:
			return "page down";
			case RC_up:
			return "cursor up";
			case RC_down:
			return "cursor down";
			case RC_left:
			return "cursor left";
			case RC_right:
			return "cursor right";
			case RC_ok:
			return "ok";
			case RC_plus:
			return "vol. inc";
			case RC_minus:
			return "vol. dec";
			case RC_spkr:
			return "mute";
			case RC_help:
			return "help";
			case RC_top_left:
			return "cursor top+left";
			case RC_top_right:
			return "cursor top+right";
			case RC_bottom_left:
			return "cursor bottom+left";
			case RC_bottom_right:
			return "cursor bottom+right";
			case RC_timeout:
			return "timeout";
			case RC_nokey:
			return "none";

			default:
			return "unknown";
	}
}


/**************************************************************************
*	transforms the rc-key to generic - internal use only!
*
**************************************************************************/
int CRCInput::translate(int code)
{

	if ((code&0xFF00)==0x5C00)
	{
		switch (code&0xFF)
		{
				case 0x0C:
				return RC_standby;
				case 0x20:
				return RC_home;
				case 0x27:
				return RC_setup;
				case 0x00:
				return RC_0;
				case 0x01:
				return RC_1;
				case 0x02:
				return RC_2;
				case 0x03:
				return RC_3;
				case 0x04:
				return RC_4;
				case 0x05:
				return RC_5;
				case 0x06:
				return RC_6;
				case 0x07:
				return RC_7;
				case 0x08:
				return RC_8;
				case 0x09:
				return RC_9;
				case 0x3B:
				return RC_blue;
				case 0x52:
				return RC_yellow;
				case 0x55:
				return RC_green;
				case 0x2D:
				return RC_red;
				case 0x54:
				return RC_page_up;
				case 0x53:
				return RC_page_down;
				case 0x0E:
				return RC_up;
				case 0x0F:
				return RC_down;
				case 0x2F:
				return RC_left;
				case 0x2E:
				return RC_right;
				case 0x30:
				return RC_ok;
				case 0x16:
				return RC_plus;
				case 0x17:
				return RC_minus;
				case 0x28:
				return RC_spkr;
				case 0x82:
				return RC_help;
				default:
				//perror("unknown old rc code");
				return RC_nokey;
		}
	}
	else if ((code&0xFF00)==0xFF00)
	{
		//Fronttasten
		//printf("-!!!!!!  before 0xFF key: %04x\n", code );
		switch (code&0xFF)
		{
				case 0x12:
				case 0x9d:
				return RC_standby;
				case 0x48:
				case 0xab:
				return RC_down;
				case 0x24:
				case 0xc7:
				return RC_up;
				case 0x20:
				case 0x40:
				case 0xaf:
				case 0xcf:
				return RC_nokey;
				case 0x10:
				case 0x9f:
				return RC_standby_release;
		}
	}
	else if (!(code&0x00))
	{
		//printf("-!!!!!!  before not-translated key: %04x\n", code );
		return code&0x3F;
	}
	//else
	//perror("unknown rc code");
	return RC_nokey;
}
