/*
$Id: ints.h,v 1.2.2.3 2003/11/27 08:40:29 coronas Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 mainly for me to learn about dvb streams, mpeg2, mhp, dsm-cc, ...

 http://dvbsnoop.sourceforge.net/

 (c) 2001-2003   Rainer.Scherg@gmx.de


*/


#ifndef __INTS_H
#define __INTS_H 1


void  decode_DSMCC_INT (u_char *b, int len);
int   pto_descriptor_loop (u_char *name, u_char *b);


#endif



