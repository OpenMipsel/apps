/*
$Id: tsdt.h,v 1.2.2.1 2003/11/26 20:38:11 coronas Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 mainly for me to learn about dvb streams, mpeg2, mhp, dsm-cc, ...

 http://dvbsnoop.sourceforge.net/

 (c) 2001-2003   Rainer.Scherg@gmx.de

*/


#ifndef __TSDT_H
#define __TSDT_H 1


void decode_TSDT (u_char *b, int len);


#endif


