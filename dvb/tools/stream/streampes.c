/*
* Calling:
 * GET /<pid>  \r\n -> for tcp opration
 * GET /<pid>,<udpport> \r\n -> for udp operation, tcp connection ist maintained as control connection
 * to end udp streaming
 *
 * TCP-Streaming Code by Simplex
 * Quick and Dirty udp Extension by Gandalfx (for ggrab and Tuxvision)
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>

#include <ost/dmx.h>
#include <ost/frontend.h>
#include <ost/sec.h>
#include <ost/video.h>

#include <pthread.h>
#include <unistd.h>

#define DEBUG
#ifdef DEBUG
#define dprint fprintf
#else
#define dprint
#endif


// size of one buffer
#define BSIZE					(1024*16)
// number of buffers
#define BUFFERS 				50

#define UDP_SEND_LEN				1444 //First Version, comaptible to ggrab 0.21 will later be higher

char 		gbuffers[BUFFERS][BSIZE];
volatile int 	gbufferState[BUFFERS];
int		gport;
pthread_cond_t	gsignal;
pthread_mutex_t	gmutex;

void * tcp_writer(void *);
void * udp_writer(void *);
void * check_alive(void *);

int main(int argc, char **argv)
{
	pthread_t			h_writer;
	pthread_t			h_control;
	int				fd;
	int				pid;
	struct dmxPesFilterParams 	flt;
	char *				bp;
	int 				currentBuffer = 0;
	int 				i;
	int				fill_len;
	int				pr;
	int				tr;
	int				r;




	
	for(i = 0; i < BUFFERS; i++)
	{
		gbufferState[i] = 0;
	}

	bp=gbuffers[0];
	while (bp-gbuffers[0] < BSIZE)
	{
		unsigned char c;
		read(1, &c, 1);
		if ((*bp++=c)=='\n')
			break;
	}
	*bp++=0;

	bp=gbuffers[0];
	if (!strncmp(bp, "GET /", 5))
	{
		printf("HTTP/1.1 200 OK\r\nServer: d-Box network\r\n\r\n"); // Content-Type: video/mpeg\r\n\r\n");
		bp+=5;
	}
	fflush(stdout);

	fd=open("/dev/dvb/card0/demux0", O_RDWR);
	if (fd<0)
	{
		perror("/dev/dvb/card0/demux0");
		return -fd;
	}
	ioctl(fd, DMX_SET_BUFFER_SIZE, 1024*1024);
	sscanf(bp, "%x", &pid);
	
	gport = 0;
	if ((bp=strchr(bp,',')) != 0) {
		bp++;
		sscanf(bp, "%d", &gport);
	}
	flt.pid =	pid;
	flt.input =	DMX_IN_FRONTEND;
	flt.output =	DMX_OUT_TAP;
	flt.pesType =	DMX_PES_OTHER;
	flt.flags =	0;
	
	if (ioctl(fd, DMX_SET_PES_FILTER, &flt)<0)
	{
		perror ("DMX_SET_PES_FILTER");
		return errno;
	}
	if (ioctl(fd, DMX_START, 0)<0)
	{
		perror ("DMX_SET_PES_FILTER");
		return errno;
	}
	pthread_cond_init(&gsignal, 0);
	pthread_mutex_init(&gmutex, 0);
	
	if (gport > 1023) {
		pthread_create( &h_writer, 0, udp_writer, NULL);
		pthread_create( &h_control, 0, check_alive, NULL);
		fill_len = UDP_SEND_LEN - 4;

	}
	else {
		pthread_create( &h_writer, 0, tcp_writer, NULL);
		fill_len = BSIZE;
	}

	fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)| O_NONBLOCK);

	while (1)
	{
		pr=0;
		tr=fill_len;
		
		while (gbufferState[currentBuffer] != 0) {
			static struct timespec t1 = {0,20 * 1000 * 1000};
			nanosleep(&t1, 0);
			perror("buffer overflow");
		}
		
		while (tr)
		{
			if ((r=read(fd, gbuffers[currentBuffer]+pr, tr))<=0)
			{
				if (errno == EAGAIN) {
					static struct timespec t2 = {0,10 * 1000 * 1000};
					nanosleep(&t2, 0);
				}
				continue;
			}
			pr+=r;
			tr-=r;
			if (tr) {
				static struct timespec t2 = {0,10 * 1000 * 1000};
				nanosleep(&t2, 0);;
			}
		}

		pthread_mutex_lock(&gmutex);
		gbufferState[currentBuffer] = 1;
		pthread_mutex_unlock(&gmutex);
		pthread_cond_signal (&gsignal);	
		
		currentBuffer ++;
		currentBuffer %= BUFFERS;
	}
	close(fd);
	return 0;
}

void* tcp_writer( void* param)
{
	int currentBuffer = 0;
	int tr,pr,b;
	
	while(1)
	{
		pthread_mutex_lock(&gmutex);
        	
		while (gbufferState[currentBuffer] != 1)
	        {
			pthread_cond_wait(&gsignal, &gmutex);
	        }
	
		pthread_mutex_unlock(&gmutex);
		
		tr = BSIZE;
		pr = 0;
		while (tr)
		{
			b = write(STDOUT_FILENO, gbuffers[currentBuffer] + pr, tr);

			if (b <= 0)
        		{
				if (b == EAGAIN) {
					static struct timespec t1 = {0,5 * 1000 * 1000};
					nanosleep(&t1, 0);
					continue;
				}
        		}
			
			tr -= b;
			pr += b;
			if (tr) {
				static struct timespec t1 = {0,5 * 1000 * 1000};
				nanosleep(&t1, 0);
			}
		}
		
		gbufferState[currentBuffer] = 0;
		currentBuffer++;
		currentBuffer %= BUFFERS;
	}
	return (0);
}
void *
udp_writer (void * p) {
	int			pnr = 0;
	int                     sockfd;
        struct sockaddr_in      cli_addr, serv_addr;
	int			addr_len = sizeof (serv_addr);
	int			currentBuffer = 0;
	int			b;

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
	
	//fcntl(gfd,F_SETFL,fcntl(gfd,F_GETFL)| O_NONBLOCK);
	
	while(1)
	{
		pthread_mutex_lock(&gmutex);
        	
		while (gbufferState[currentBuffer] != 1)
	        {
			pthread_cond_wait(&gsignal, &gmutex);
	        }
	
		pthread_mutex_unlock(&gmutex);
		
		while (1)
		{
			*((int *)(&(gbuffers[currentBuffer][UDP_SEND_LEN-4])))=pnr++;	
			b = sendto(sockfd, gbuffers[currentBuffer], UDP_SEND_LEN, 0, (struct sockaddr *) &serv_addr, addr_len);

			if (b <= 0)
        		{
				if (b == EAGAIN) {
					static struct timespec t1 = {0,5 * 1000 * 1000};
					nanosleep(&t1, 0);
					continue;
				}
				perror("udp_writer: sendto");
				continue;
        		}
			break;
		}
		
		gbufferState[currentBuffer] = 0;
		currentBuffer++;
		currentBuffer %= BUFFERS;
	}
	return (0);
}

void * 
check_alive(void * p) {
	char b;
	while (read(STDIN_FILENO,&b,1) >= 0);
 	exit(0);
}
	
