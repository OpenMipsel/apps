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
#include <sys/poll.h>
#include <errno.h>
#include <string.h>

#include <ost/dmx.h>
#include <ost/frontend.h>
#include <ost/sec.h>
#include <ost/video.h>

#include <pthread.h>
#include <unistd.h>

// size of one buffer
#define BSIZE					1024*16
// number of buffers
#define BUFFERS 				100

#define UDP_SEND_LEN				1444 //First Version, comaptible to ggrab 0.21 will later be higher

// the mutex for locking the bufferstate array
pthread_mutex_t lock;
// the thread writing the stream
pthread_t writer;

// array holding the state of each buffer
// 0 : buffer is empty
// 1 : buffer is ready to write
int bufferState[BUFFERS];

// the buffers
char buffers[BUFFERS][BSIZE];

int	gfd;
int	gport;


void * send_udp(void *);

void* start_writer( void* param)
{
	long counter = 0;
	int currentBuffer = 0;

	while(1)
	{
    	// sleep until our current buffer is ready to write
        while (bufferState[currentBuffer] != 1)
        {
			usleep(10);
        }
        // write the buffer
		int p = 0;
		while (p < BSIZE)
		{
			int b = write(1, buffers[currentBuffer], BSIZE - p);
        	p += b;
			if (b <= 0)
        	{
        		perror("output");
        		break;
        	}
		}
		// set the buffer's state to empty
		pthread_mutex_lock(&lock);
		bufferState[currentBuffer] = 0;
		pthread_mutex_unlock(&lock);
		// set currentBuffer to the next one
		counter++;
		currentBuffer = counter % BUFFERS;
	}
	return (0);
}

int main(int argc, char **argv)
{
	int fd, pid;
	struct dmxPesFilterParams flt;
	char *bp;
	long counter = 0;
	int currentBuffer = 0;
	int i;
	for(i = 0; i<BUFFERS; i++)
	{
		bufferState[i] = 0;
	}

	bp=buffers[0];
	while (bp-buffers[0] < BSIZE)
	{
		unsigned char c;
		read(1, &c, 1);
		if ((*bp++=c)=='\n')
			break;
	}
	*bp++=0;

	bp=buffers[0];
	if (!strncmp(buffers[0], "GET /", 5))
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
	flt.pid=pid;
	flt.input=DMX_IN_FRONTEND;
	flt.output=DMX_OUT_TAP;
	flt.pesType=DMX_PES_OTHER;
	flt.flags=0;
	
	if (ioctl(fd, DMX_SET_PES_FILTER, &flt)<0)
	{
		perror("DMX_SET_PES_FILTER");
		return errno;
	}
	if (ioctl(fd, DMX_START, 0)<0)
	{
		perror("DMX_SET_PES_FILTER");
		return errno;
	}
	if (gport > 1023) {
		gfd = fd;
		pthread_create( &writer, 0, send_udp, NULL);
		while (read(STDIN_FILENO,buffers[0],1) >= 0);
	 	exit(0);
	}
	else {
		// init mutex for the bufferstate array
		pthread_mutex_init(&lock, NULL);
		// create the thread that writes the buffers to the streaming app
		pthread_create( &writer, 0, start_writer, NULL);


		while (1)
		{
			int pr=0, r;
			int tr=BSIZE;

		if (bufferState[currentBuffer] != 0)
			{
				// current buffer is not ready for read
				perror("output");
			}
			// read stream into buffer until buffer is full
			while (tr)
			{
				if ((r=read(fd, buffers[currentBuffer]+pr, tr))<=0)
				{
					continue;
				}
				pr+=r;
				tr-=r;
			}

			// set the buffer state on "ready to write"
			pthread_mutex_lock(&lock);
			bufferState[currentBuffer] = 1;
			pthread_mutex_unlock(&lock);
			// set current buffer to the next one
			counter++;
			currentBuffer = counter % BUFFERS;
		}
	}

	close(fd);
	return 0;
}
void *
send_udp (void * p) {
	int			pnr = 0;
	int                     sockfd;
        struct sockaddr_in      cli_addr, serv_addr;
	int			addr_len = sizeof (serv_addr);
	static	char 		buffer[UDP_SEND_LEN];

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
	
	while (1) {
		int pr=0, r;
		int tr=UDP_SEND_LEN-4;
		while (tr) {
			if ((r=read(gfd, buffer+pr, tr)) <= 0) {
				usleep(10000); //wait for 10 ms = max. 10 kb Only for NONBLOCK operation not activated due to Bug in driver 
				continue;
			}
			pr+=r;
			tr-=r;
		}
		*((int *)(&(buffer[UDP_SEND_LEN-4])))=pnr++;	
		if (sendto(sockfd, buffer, UDP_SEND_LEN, 0, (struct sockaddr *) &serv_addr, addr_len) < 0) {
			continue;
		}
	}
	return(p);
}
