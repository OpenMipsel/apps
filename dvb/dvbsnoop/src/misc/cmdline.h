/*

 -- (c) 2001 rasc


$Id: cmdline.h,v 1.2.2.1 2003/10/28 19:33:17 coronas Exp $


*/




/*
 -- defs...
*/

enum PACKET_MODE  {SECT, TS, PES};
enum TIME_MODE    {NO_TIME, FULL_TIME, DELTA_TIME};

typedef struct _OPTIONS {
  int         packet_mode;
  int         printhex;
  int         printdecode;
  int         binary_out;
  char        *inpPidFile;	// read from file instead of dmux if not NULL
  int         help;
  char        *devDemux;
  char        *devDvr;
  u_int       pid;
  u_int       filter;
  u_int       mask;
  int         crc;
  long        packet_count;
  int         time_mode;
} OPTION;


/*
 -- prototypes
*/

int  cmdline_options (int argc, char **argv, OPTION *opt);




