/*
 * $Id: streampes.c,v 1.4.2.5 2003/02/18 19:15:11 gandalfx Exp $
 *
 * Copyright (C) 2001 by tmbinc
 * Copyright (C) 2001 by kwon
 * Copyright (C) 2002 by Andreas Oberritter <obi@tuxbox.org>
 * Copyright (C) 2003 by Gandalfx
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
 * Calling:
 * GET /<pid>  \r\n -> for tcp opration
 * GET /<pid>,<udpport> \r\n -> for udp operation, tcp connection ist maintained as control connection
 *                              to end udp streaming
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <ost/dmx.h>
#include <ost/frontend.h>
#include <ost/sec.h>
#include <ost/video.h>

#define UDP_SEND_LEN	(1444)
#define BSIZE		(16*1024)
#define DMX_BUFFER_SIZE (1024*1024)

static void * send_udp(void *);
static int    gport;
static int    gfd;

int main(int argc, char **argv)
{
	pthread_t 	thread;
	unsigned short	pid;
	struct 		dmxPesFilterParams 	flt;
	static char 	buffer[BSIZE];
	char * 		bp;
	unsigned char 	c;
	
	bp = buffer;
	while ((bp-buffer) < BSIZE) {
		read(STDIN_FILENO, &c, 1);
		if ((*bp++=c)=='\n')
			break;
	}

	*bp++ = 0;
	bp = buffer;
	if (!strncmp(buffer, "GET /", 5))
	{
		printf("HTTP/1.1 200 OK\r\nServer: d-Box network\r\n\r\n");
		bp += 5;
	}
	sscanf(bp, "%hx", &pid);
	
	gport = 0;
	if ((bp=strchr(bp,',')) != 0) {
		bp++;
		sscanf(bp, "%d", &gport);
	}

	fflush(stdout);
	gfd=open("/dev/dvb/card0/demux0", O_RDWR);
	if (gfd<0)
	{
		perror("/dev/dvb/card0/demux0");
		return -gfd;
	}
	ioctl(gfd, DMX_SET_BUFFER_SIZE, DMX_BUFFER_SIZE);
	
	flt.pid =	pid;
	flt.input =	DMX_IN_FRONTEND;
	flt.output =	DMX_OUT_TAP;
	flt.pesType =	DMX_PES_OTHER;
	flt.flags =	0;
	
	if (ioctl(gfd, DMX_SET_PES_FILTER, &flt)<0)
	{
		perror ("DMX_SET_PES_FILTER");
		return errno;
	}
	if (ioctl(gfd, DMX_START, 0)<0)
	{
		perror ("DMX_SET_PES_FILTER");
		return errno;
	}

	if (gport > 1023) {
		if (pthread_create(&thread, 0, send_udp, NULL)) {
			return (-1);
		}
			
		while (read(STDIN_FILENO,buffer,1) >= 0);
	}
	else {
		setpriority(PRIO_PROCESS,0,-10);
		// Workaround für Write-Blockierung im Socket Interface
//		int sockopt = 100;
//		sockopt = setsockopt(STDOUT_FILENO, SOL_SOCKET,SO_SNDBUF,&sockopt,sizeof(sockopt));
		
		while (1) {
			int pr = 0, r;
			int tr = BSIZE;
			while (tr) {
				if ((r=read(gfd, buffer+pr, tr)) <= 0) {
					if (errno == EAGAIN) {
						usleep(3000);
						continue;
					}
					break;
				}
				pr+=r;
				tr-=r;
				if(tr) {
					usleep(3000);  
				}
			}
			tr = BSIZE;
			pr = 0;

			while (tr) {
				r = write(STDOUT_FILENO, buffer+pr, tr);
				if (r <= 0) {
					if (errno == EAGAIN) {
						continue;
					}
					break;
				}
				pr += r;
				tr -= r;
			}
		}
	}

	close(gfd);
	exit(0);
}

void *
send_udp (void * ret) {
	int			pnr = 0;
	int                     sockfd;
        struct sockaddr_in      cli_addr, serv_addr;
	int			addr_len = sizeof (serv_addr);
	static	char 		buffer[UDP_SEND_LEN];
				char a[100];
				sprintf(a,"/tmp/aaa%d.log",gport);
				FILE * fp = fopen (a,"a");
				fprintf(fp,"hallo1");
				fclose(fp);

	setpriority(PRIO_PROCESS,0,-10);
	
	int flags = fcntl(gfd,F_GETFL,0);
	if (flags != -1) {
		fcntl(gfd,F_SETFL, flags | O_NONBLOCK);
	}
	
	if(getpeername(STDOUT_FILENO, (struct sockaddr *) &serv_addr, &addr_len)) {
		exit(-1);
	}

	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_port        = htons(gport);

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		exit(-1);
	}

	memset((char *) &cli_addr,0, sizeof(cli_addr));
	cli_addr.sin_family      = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port        = htons(0);

	if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
		exit(-1);
	}
		
	// Workaround für Write-Blockierung im Socket Interface
//	int sockopt = 100;
//	sockopt = setsockopt(sockfd, SOL_SOCKET,SO_SNDBUF,&sockopt,sizeof(sockopt));
	
	while (1) {
		int pr=0, r;
		int tr=UDP_SEND_LEN-4;
		while (tr) {
			if ((r=read(gfd, buffer+pr, tr)) <= 0) {
				if (errno == EAGAIN) {
					usleep(20000); //wait for 10 ms = max. 10 kb 
					continue;
				}
				continue;
			}
			pr+=r;
			tr-=r;
			if(tr) {
				usleep(20000);  
			}
		}

		*((int *)(&(buffer[UDP_SEND_LEN-4])))=pnr++;	
		while (1) {
			fd_set 		tfds;
			struct timeval 	tv;
			FD_ZERO(&tfds);
			FD_SET(sockfd, &tfds);
			tv.tv_sec = 0;
			tv.tv_usec = 500 * 1000;

			// New Workaround fpor blocking of socket: close, and open it again....
			r = select(sockfd + 1, NULL, &tfds, NULL, &tv);
			if (r == 0) {
				close(sockfd);	
				if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					exit(-1);
				}
				if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
					exit(-1);
				}
			}
			if ((r = sendto(sockfd, buffer, UDP_SEND_LEN, 0, (struct sockaddr *) &serv_addr, addr_len)) < 0) {
				if (errno == EAGAIN) {
					usleep (10000);
					continue;
				}
			}
			break;
		}
	}
	return ret;
}

