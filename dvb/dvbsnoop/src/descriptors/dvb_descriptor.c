/*
$Id: dvb_descriptor.c,v 1.9 2003/10/24 22:45:04 rasc Exp $ 


  dvbsnoop
  (c) Rainer Scherg 2001-2003

  DVB Descriptors  ETSI 300 468


  EN/ISO/TR document references in comments may habe been changed
  during updates of documents by ETSI.



$Log: dvb_descriptor.c,v $
Revision 1.9  2003/10/24 22:45:04  rasc
code reorg...

Revision 1.8  2003/10/24 22:17:17  rasc
code reorg...

Revision 1.7  2003/10/21 21:31:29  rasc
no message

Revision 1.6  2003/10/21 19:54:43  rasc
no message

Revision 1.5  2003/10/19 22:31:38  rasc
- some datacarousell stuff started

Revision 1.4  2003/10/19 22:22:57  rasc
- some datacarousell stuff started

Revision 1.3  2003/10/19 21:05:53  rasc
- some datacarousell stuff started

Revision 1.2  2003/10/16 19:02:28  rasc
some updates to dvbsnoop...
- small bugfixes
- tables updates from ETR 162

Revision 1.1  2003/07/08 19:59:50  rasc
restructuring... some new, some fixes,
trying to include DSM-CC, Well someone a ISO13818-6 and latest version of ISO 18313-1 to spare?



*/


#include "dvbsnoop.h"
#include "dvb_descriptor.h"
#include "misc/hexprint.h"
#include "misc/output.h"






/*
  determine descriptor type and print it...
  return byte length
*/

int  descriptorDVB  (u_char *b)

{
 int len;
 int id;


  id  =  (int) b[0];
  len = ((int) b[1]) + 2;

  out_NL (4);
  out_S2B_NL (4,"DVB-DescriptorTag: ",id, dvbstrDVBDescriptorTAG(id));
  out_SW_NL  (5,"Descriptor_length: ",b[1]);

  // empty ??
  len = ((int)b[1]) + 2;
  if (b[1] == 0)
	 return len;

  // print hex buf of descriptor
  printhex_buf (9, b,len);



  switch (b[0]) {

     case 0x40:  descriptorDVB_NetName (b);  break;
     case 0x41:  descriptorDVB_ServList (b);  break;
     case 0x42:  descriptorDVB_Stuffing (b);  break;
     case 0x43:  descriptorDVB_SatDelivSys (b);  break;
     case 0x44:  descriptorDVB_CableDelivSys (b);  break;
     case 0x45:  descriptorDVB_VBI_Data (b);  break;
     case 0x46:  descriptorDVB_VBI_Teletext (b);  break;
     case 0x47:  descriptorDVB_BouquetName (b);  break;
     case 0x48:  descriptorDVB_Service (b);  break;
     case 0x49:  descriptorDVB_CountryAvail (b);  break;
     case 0x4A:  descriptorDVB_Linkage (b);  break;
     case 0x4B:  descriptorDVB_NVOD_Reference (b);  break;
     case 0x4C:  descriptorDVB_TimeShiftedService (b);  break;
     case 0x4D:  descriptorDVB_ShortEvent (b);  break;
     case 0x4E:  descriptorDVB_ExtendedEvent (b);  break;
     case 0x4F:  descriptorDVB_TimeShiftedEvent(b);  break;
     case 0x50:  descriptorDVB_Component(b);  break;
     case 0x51:  descriptorDVB_Mosaic(b);  break;
     case 0x52:  descriptorDVB_StreamIdent (b);  break;
     case 0x53:  descriptorDVB_CAIdentifier (b);  break;
     case 0x54:  descriptorDVB_Content (b);  break;
     case 0x55:  descriptorDVB_ParentalRating(b);  break;
     case 0x56:  descriptorDVB_Teletext (b);  break;
     case 0x57:  descriptorDVB_Telephone (b);  break;
     case 0x58:  descriptorDVB_LocalTimeOffset (b);  break;
     case 0x59:  descriptorDVB_Subtitling (b);  break;
     case 0x5A:  descriptorDVB_TerrestDelivSys (b);  break;
     case 0x5B:  descriptorDVB_MultilingNetworkName (b);  break;
     case 0x5C:  descriptorDVB_MultilingBouquetName (b);  break;
     case 0x5D:  descriptorDVB_MultilingServiceName (b);  break;
     case 0x5E:  descriptorDVB_MultilingComponent (b);  break;
     case 0x5F:  descriptorDVB_PrivateDataSpecifier (b);  break;
     case 0x60:  descriptorDVB_ServiceMove (b);  break;
     case 0x61:  descriptorDVB_ShortSmoothingBuffer (b);  break;
     case 0x62:  descriptorDVB_FrequencyList (b);  break;
     case 0x63:  descriptorDVB_PartialTransportStream(b);  break;
     case 0x64:  descriptorDVB_DataBroadcast(b);  break;
     case 0x65:  descriptorDVB_CASystem(b);  break;
     case 0x66:  descriptorDVB_DataBroadcastID(b);  break;
     case 0x67:  descriptorDVB_TransportStream(b);  break;
     case 0x68:  descriptorDVB_DSNG(b);  break;
     case 0x69:  descriptorDVB_PDC(b);  break;
     case 0x6A:  descriptorDVB_AC3(b);  break;
     case 0x6B:  descriptorDVB_AncillaryData(b);  break;
     case 0x6C:  descriptorDVB_CellList(b);  break;
     case 0x6D:  descriptorDVB_CellFrequencyLink(b);  break;
     case 0x6E:  descriptorDVB_AnnouncementSupport(b);  break;
     case 0x6F:  descriptorDVB_ApplicationSignalling(b);  break;
     case 0x70:  descriptorDVB_AdaptionFieldData(b);  break;
     case 0x71:  descriptorDVB_ServiceIdentifier(b);  break;
     case 0x72:  descriptorDVB_ServiceAvailability(b);  break;


     default: 
	if (b[0] < 0x80) {
	    out_nl (0,"  ----> ERROR: unimplemented descriptor (dvb context), Report!");
	}
	descriptorDVB_any (b);
	break;
  } 


  return len;   // (descriptor total length)
}








/*
  Any  descriptor  (Basic Descriptor output)
  ETSI 300 468 
*/

void descriptorDVB_any (u_char *b)

{

 typedef struct  _descANY {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // private data bytes

 } descANY;


 descANY  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 out_nl (4,"Descriptor-Data:");
 printhexdump_buf (4,b+2,d.descriptor_length);

}







/* --------------------------------------------------------------- 
  well known DVB descriptors
   --------------------------------------------------------------- */



/*
  0x40 NetName  descriptor  (network name descriptor)
*/

void descriptorDVB_NetName (u_char *b)

{
 /* ETSI 300 468 */

 typedef struct  _descNND {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_char     *network_name;   // with controls


 } descNND;

 descNND  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];
 // d.network_name to be assigned 


 out (4,"Network_name:  ");
 b += 2;

 print_name (4, b,d.descriptor_length);
 out_NL (4);

}




/*
  0x41 Service List Descriptor 
*/

void descriptorDVB_ServList (u_char *b)

{
 /* ETSI 300 468  6.2.xx */

 typedef struct  _descServList {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N ... Service ID & Types

 } descServList;


 typedef struct _descServList2 {
    u_int      service_id;
    u_int      service_type;
 } descServList2;


 descServList  d;
 descServList2 d2;
 int           len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 len = d.descriptor_length - 2;
 b += 2;

 indent (+1);
 while (len > 0) {
   d2.service_id 		= getBits (b, 0, 0, 16);
   d2.service_type 		= getBits (b, 0, 16, 8);
   b   += 3;
   len -= 3;
   

    out_S2W_NL (4,"Service_ID: ",d2.service_id,
	   " --> refers to PMS program_number");
    out_S2B_NL (4,"Service_type: ",d2.service_type,
	   dvbstrService_TYPE(d2.service_type));
    out_NL (4);

 }
 indent (-1);


}





/*
  0x42  Stuffing descriptor 
  ETSI EN 300 468  
*/

void descriptorDVB_Stuffing (u_char *b)

{
  descriptorDVB_any (b);
}





/*
  0x43 SatDelivSys  descriptor  (Satellite delivery system descriptor)
*/

void descriptorDVB_SatDelivSys (u_char *b)

{
 /* ETSI 300 468    6.2.xx */

 typedef struct  _descSDS {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_long     frequency;
    u_int      orbital_position;
    u_int      west_east_flag;
    u_int      polarisation;
    u_int      modulation;
    u_long     symbol_rate;
    u_int      FEC_inner;

 } descSDS;

 descSDS  d;
 //int      i;




 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 d.frequency			 = getBits (b, 0, 16, 32);
 d.orbital_position		 = getBits (b, 0, 48, 16);
 d.west_east_flag		 = getBits (b, 0, 64, 1);
 d.polarisation			 = getBits (b, 0, 65, 2);
 d.modulation			 = getBits (b, 0, 67, 5);
 d.symbol_rate			 = getBits (b, 0, 72, 28);
 d.FEC_inner			 = getBits (b, 0, 100, 4);


 out_nl (4,"Frequency: %lu (= %3lx.%05lx GHz)",d.frequency,
	 d.frequency >> 20, d.frequency & 0x000FFFFF );
 out_nl (4,"Orbital_position: %u (= %3x.%01x)",d.orbital_position,
	 d.orbital_position >> 4, d.orbital_position & 0x000F);

 out_S2B_NL (4,"West_East_flag: ",d.west_east_flag,
	 dvbstrWEST_EAST_FLAG(d.west_east_flag));

 out_S2B_NL (4,"Polarisation: ",d.polarisation,
	 dvbstrPolarisation_FLAG(d.polarisation));

 out_S2B_NL (4,"Modulation (Sat): ",d.modulation,
	 dvbstrModulationSAT_FLAG(d.modulation));

 out_nl (4,"Symbol_rate: %u (= %3x.%04x)",d.symbol_rate,
	 d.symbol_rate >> 16, d.symbol_rate & 0x0000FFFF );

 out_S2B_NL (4,"FEC_inner: ",d.FEC_inner,
	 dvbstrFECinner_SCHEME (d.FEC_inner));


}





/*
  0x44 CableDelivSys  descriptor  (Cable delivery system descriptor)
*/

void descriptorDVB_CableDelivSys (u_char *b)

{
 /* ETSI 300 468    6.2.xx */

 typedef struct  _descCDS {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_long     frequency;
    u_int      FEC_outer;
    u_int      reserved_1;
    u_int      modulation;
    u_long     symbol_rate;
    u_int      FEC_inner;

 } descCDS;

 descCDS  d;
 //int      i;




 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 d.frequency			 = getBits (b, 0, 16, 32);
 d.reserved_1			 = getBits (b, 0, 48, 12);
 d.FEC_outer			 = getBits (b, 0, 60, 4);
 d.modulation			 = getBits (b, 0, 64, 8);
 d.symbol_rate			 = getBits (b, 0, 72, 28);
 d.FEC_inner			 = getBits (b, 0, 100, 4);


 out_nl (4,"Frequency: %lu (= %3lx.%05lx MHz)",d.frequency,
	 d.frequency >> 16, d.frequency & 0x0000FFFF );

 out_S2B_NL (4,"FEC_outer: ",d.FEC_outer,
	 dvbstrFECouter_SCHEME (d.FEC_outer));

 out_SB_NL (6,"reserved_1: ",d.reserved_1);


 out_S2B_NL (4,"Modulation (Cable): ",d.modulation,
	 dvbstrModulationCable_FLAG(d.modulation));

 out_nl (4,"  Symbol_rate: %u (= %4x.%04x)",d.symbol_rate,
	 d.symbol_rate >> 16, d.symbol_rate & 0x0000FFFF );

 out_S2B_NL (4,"FEC_inner: ",d.FEC_inner,
	 dvbstrFECinner_SCHEME (d.FEC_inner));

}






/*
  0x45  VBI Data  descriptor 
  ETSI EN 300 468  2.2.42
*/

void descriptorDVB_VBI_Data  (u_char *b)

{

 typedef struct  _descVBIData {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N ... DataList 1
 } descVBIData;

 typedef struct  _descVBIData2 {
    u_int      data_service_id;		
    u_int      data_service_descriptor_length;		

    //    N ... DataList 3
    // or N ... reserved bytes
 } descVBIData2;

 typedef struct  _descVBIData3 {
    u_int      reserved_1;		
    u_int      field_parity;		
    u_int      line_offset;		
 } descVBIData3;



 descVBIData   d;
 descVBIData2  d2;
 descVBIData3  d3;
 int           len1,len2;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 b += 2;
 len1 = d.descriptor_length;


 indent (+1);
 while (len1 > 0) {

    d2.data_service_id		       = b[0];
    d2.data_service_descriptor_length   = b[1];

    out_NL (4); 
    out_nl (4, "Data_service_id: %u  [= %s]", d2.data_service_id,
		dvbstrDataService_ID(d2.data_service_id));

    out_SB_NL (5,"Data_service_descriptor_length: ",
		d2.data_service_descriptor_length);


    b    += 2;
    len1 -= 2;
    len2  = d2.data_service_descriptor_length;

    if (d2.data_service_id >= 1 && d2.data_service_id <= 7) {

       indent (+1);
       while (len2 > 0) {
           d3.reserved_1		 = getBits (b, 0, 0, 2);
           d3.field_parity		 = getBits (b, 0, 2, 1);
           d3.line_offset		 = getBits (b, 0, 3, 5);

           out_NL (4);
           out_SB_NL (6,"reserved_1: ",d3.reserved_1);
           out_SB_NL (4,"field_parity: ",d3.field_parity);
           out_SB_NL (4,"line_offset: ",d3.line_offset);
	   len2--;
       } 
       indent (-1);

    } else {

       out_nl (6,"Reserved Data:");
       printhexdump_buf (6, b,len2);

    }


 }
 indent (-1);


}






/*
  0x46  VBI teletext descriptor 
  ETSI EN 300 468   6.2.xx
*/

void descriptorDVB_VBI_Teletext (u_char *b)

{
  descriptorDVB_Teletext (b);
}






/*
  0x47  Bouquet Name  descriptor 
  ETSI EN 300 468    6.2.xx
*/

void descriptorDVB_BouquetName  (u_char *b)

{

 typedef struct  _descBouquetName {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N   ... char name

 } descBouquetName;


 descBouquetName d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 out (4,"BouquetName: ");
 print_name (4, b+2,d.descriptor_length);
 out_NL (4);

}







/*
  0x48  Service  descriptor 
  ETSI EN 300 468   6.2.xx
*/

void descriptorDVB_Service  (u_char *b)

{

 typedef struct  _descService {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      service_type;
    u_int      service_provider_name_length;

    // N   char 
    // char   *service_provider_name;

    u_int      service_name_length;

    // N2  char
    // char   *service_name;

 } descService;


 descService d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.service_type			 = getBits (b, 0, 16, 8);
 d.service_provider_name_length  = getBits (b, 0, 24, 8);


 b += 4;

 out_S2B_NL (4,"Service_type: ",d.service_type,
	dvbstrService_TYPE(d.service_type));
 out_SB_NL (5,"Service_provider_name_length: ",d.service_provider_name_length);
 out (4,"Service_provider_name: ");
        print_name (4, b,d.service_provider_name_length);
 out_NL (4);
 

 b += d.service_provider_name_length;
 d.service_name_length		  = getBits (b, 0, 0, 8);
 b += 1;

 out_SW_NL (5,"Service_name_length: ",d.service_name_length);
 out (4,"Service_name: ");
        print_name (4, b,d.service_name_length);
 out_NL (4);

}





/*
  0x49  Country Availibility  descriptor 
  ETSI EN 300 468   6.2.xx
*/

void descriptorDVB_CountryAvail  (u_char *b)

{

 typedef struct  _descCountryAvail {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      country_availability_flag;
    u_int      reserved_1;

    //  N   countrycodes[3]

 } descCountryAvail;


 descCountryAvail d;
 int              len;
 u_char           country_code[4];



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.country_availability_flag	 = getBits (b, 0, 16, 1);
 d.reserved_1			 = getBits (b, 0, 17, 7);

 
 out_SB_NL (4,"country_availability_flag: ",d.country_availability_flag);
 out_SB_NL (6,"reserved_1: ",d.reserved_1);

 b  += 3;
 len = d.descriptor_length - 1;

 indent (+1);
 while (len > 0) {
    strncpy (country_code, b, 3);
    *(country_code+3) = '\0';
    b   += 3;
    len -= 3;

    out_nl (4,"Country:  %3.3s",country_code);
 }
 indent (-1);

}







/*
  0x4A  Linkage  descriptor  
*/

void descriptorDVB_Linkage (u_char *b)

{
 /* ETSI 300 468   6.2.xx */

 typedef struct  _descLinkage {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      transport_stream_id;
    u_int      original_network_id;
    u_int      service_id;
    u_int      linkage_type;

    // if linkage_type == 8
    // the following field are conditional!!
    u_int      handover_type;
    u_int      reserved_1; 
    u_int      origin_type;
    u_int      network_id;
    u_int      initial_service_id;

 } descLinkage;


 descLinkage  d;
 int          len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.transport_stream_id		 = getBits (b, 0, 16, 16);
 d.original_network_id		 = getBits (b, 0, 32, 16);
 d.service_id			 = getBits (b, 0, 48, 16);
 d.linkage_type			 = getBits (b, 0, 64, 8);


 out_SW_NL  (4,"Transport_stream_ID: ",d.transport_stream_id);
 out_S2W_NL (4,"Original_network_ID: ",d.original_network_id,
	dvbstrOriginalNetwork_ID(d.original_network_id));
 out_S2W_NL (4,"Service_ID: ",d.service_id,
      " --> refers to PMS program_number");
 out_S2B_NL (4,"Linkage_type: ",d.linkage_type,
	dvbstrLinkage_TYPE (d.linkage_type));

 len = d.descriptor_length - 7;
 b  += 7 + 2;

 indent (+1);
 if (d.linkage_type != 0x08) {

    if (d.linkage_type == 0x0B) {		/* EN 301 192 */
        sub_descriptorDVB_Linkage0x0B (b, len);
    } else if (d.linkage_type == 0x0C) {	/* EN 301 192 */
        sub_descriptorDVB_Linkage0x0C (b, len);
    } else {
    	// private data
	out_nl (4,"Private data:"); 
    	printhexdump_buf (4, b,len);
    }

 } else {
    d.handover_type		= getBits (b, 0, 0, 4);
    d.reserved_1		= getBits (b, 0, 4, 3);
    d.origin_type		= getBits (b, 0, 7, 1);
    b   += 1;
    len -= 1;
    
    out_S2B_NL (4,"Handover_type: ",d.handover_type,
	dvbstrHandover_TYPE(d.handover_type));

    out_SB_NL (6,"reserved_1: ",d.reserved_1);

    out_S2B_NL (4,"Origin_type: ",d.origin_type,
	dvbstrOrigin_TYPE(d.origin_type));

    if (   d.handover_type == 0x01
        || d.handover_type == 0x02
        || d.handover_type == 0x03) {
        d.network_id		= getBits (b, 0, 0, 16);
        out_S2W_NL (4,"Network_ID: ",d.network_id,
		dvbstrNetworkIdent_ID(d.network_id));
        b   += 1;
        len -= 1;
    } 
    if (d.origin_type == 0x00) {
        d.initial_service_id	= getBits (b, 0, 0, 16);
        out_SW_NL (4,"Initial_service_ID: ",d.initial_service_id);
        b   += 1;
        len -= 1;
    }

    if (len > 0) {
      // private data 
       out_nl (4,"Private data:"); 
       printhexdump_buf (4, b,len);
    }
    

 } //if
 indent (-1);

}



/*
 * -- as defined as private data for DSM-CC
 * -- in EN 301 192
 */

void sub_descriptorDVB_Linkage0x0B (u_char *b, int len)			 /* $$$ TODO */
{

 typedef struct  _descLinkage0x0B {
    u_int      platform_id_data_length;		

        // inner Loop 1
    	u_long     platform_id;		
	u_int      platform_name_loop_length;		

        // inner Loop 2
    	u_char     ISO639_2_language_code[4];
    	u_int      platform_name_length;

 } descLinkage0x0B;



 descLinkage0x0B  d;
 int		  len_loop1;


 /* $$$ */ out_nl (4, "-----> $$$$ Check this output !!! not verified, may be buggy ");
 d.platform_id_data_length     	 = b[0];
 out_SB_NL  (4,"Platform_ID_data_length: ",d.platform_id_data_length);

 len_loop1 = d.platform_id_data_length;
 b++;
 len--;


 while (len_loop1 > 0) {

	int		 len_loop2;

        d.platform_id 	 		= getBits (b, 0,  0, 24);
        d.platform_name_loop_length  	= getBits (b, 0, 24,  8);
	b += 4;
	len -= 4;

        out_SL_NL  (4,"Platform_id: ",d.platform_id);	/* $$$$ TODO: platform_id_str nach EN 162 */
        out_SB_NL  (4,"Platform_name_loop_length: ",d.platform_name_loop_length);

	len_loop1 -= 4;
        len_loop2 = d.platform_name_loop_length;

	while (len_loop2 > 0) {

	 	getISO639_3 (d.ISO639_2_language_code, b);	
        	d.platform_name_length  = getBits (b, 0, 24,  8);
		b += 4;
		len -= 4;
		len_loop2 -= 4;

 		print_name (4, b,d.platform_name_length);
		out_NL (4);

		b +=  d.platform_name_length;
		len -= d.platform_name_length;
		len_loop2 -= d.platform_name_length;

	}

        len_loop1 -= d.platform_name_loop_length;

 }


  // private data
  out_nl (4,"Private data:"); 
  printhexdump_buf (4, b,len);


}



void sub_descriptorDVB_Linkage0x0C (u_char *b, int len)
{

 typedef struct  _descLinkage0x0C {
    u_int      table_id;		
    // conditional
    u_int      bouquet_id;

 } descLinkage0x0C;



 descLinkage0x0C  d;

 d.table_id				= getBits (b, 0,  0, 8);
 out_S2W_NL  (4,"Table_id: ",d.table_id, dvbstrLinkage0CTable_TYPE(d.table_id));

 if (d.table_id == 2) {
	d.bouquet_id			= getBits (b, 0,  8, 16);
 	out_S2W_NL  (4,"Bouquet_id: ",d.bouquet_id, dvbstrBouquetTable_ID (d.bouquet_id));
 }

}







/*
  0x4B  NVOD Reference  descriptor 
  ETSI EN 300 468  6.2.xx
*/

void descriptorDVB_NVOD_Reference  (u_char *b)

{

 typedef struct  _descNVODRef {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N ...  Ref2
 } descNVODRef;

 typedef struct  _descNVODRef2 {
    u_int      transport_stream_id;
    u_int      original_network_id;
    u_int      service_id;
 } descNVODRef2;


 descNVODRef  d;
 descNVODRef2 d2;
 int          len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 b  += 2;
 len = d.descriptor_length;

 indent (+1);
 while (len > 0) {
    d2.transport_stream_id	 = getBits (b, 0, 0, 16);
    d2.original_network_id	 = getBits (b, 0, 16, 16);
    d2.service_id		 = getBits (b, 0, 32, 16);

    len -= 6;
    b   += 6;

    out_SW_NL  (4,"Transport_stream_ID: ",d2.transport_stream_id);
    out_S2W_NL (4,"Original_network_ID: ",d2.original_network_id,
	  dvbstrOriginalNetwork_ID(d2.original_network_id));
    out_S2W_NL (4,"Service_ID: ",d2.service_id,
        " --> refers to PMS program_number");
    out_NL (4);
 }
 indent (-1);

}







/*
  0x4C  Time Shifted Service   descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_TimeShiftedService  (u_char *b)

{

 typedef struct  _descTimShiftServ {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reference_service_id;

 } descTimShiftServ;


 descTimShiftServ  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reference_service_id		 = getBits (b, 0, 16, 16);

 out_SW_NL (4,"Reference_service_ID: ",d.reference_service_id);

}







/*
  0x4D  Short Event  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_ShortEvent  (u_char *b)

{

 typedef struct  _descShortEvent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_char     ISO639_2_language_code[4];
    u_int      event_name_length;

    // N   char event_name

    u_int      text_length;

    // N2  char  text char

 } descShortEvent;


 descShortEvent d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 getISO639_3 (d.ISO639_2_language_code, b+2);
 out_nl (4,"  ISO639_2_language_code:  %3.3s", d.ISO639_2_language_code);


 d.event_name_length		 = getBits (b, 5, 0, 8);
 b += 6;

 out_SB_NL (5,"Event_name_length: ",d.event_name_length);
 out       (4,"Event_name: ");
	print_name (4, b,d.event_name_length);
	out_NL (4);

 b += d.event_name_length;

 d.text_length			 = getBits (b, 0, 0, 8);
 b += 1;
 out_SB_NL (5,"Text_length: ",d.text_length);
 	out (4,"Text: ");
 	print_name (4, b,d.text_length);
	out_NL (4);

}






/*
  0x4E  Extended Event  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_ExtendedEvent  (u_char *b)

{

 typedef struct  _descExtendedEvent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      descriptor_number;
    u_int      last_descriptor_number;
    u_char     ISO639_2_language_code[4];
    u_int      length_of_items;

    // N   Ext. Events List

    u_int      text_length;
    // N4  char  text char
 } descExtendedEvent;


 typedef struct  _descExtendedEvent2 {
    u_int      item_description_length;
    //  N2   descriptors
    u_int      item_length;
    //  N3   chars
 } descExtendedEvent2;


 descExtendedEvent    d;
 descExtendedEvent2   d2;
 int                  len1, lenB;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.descriptor_number		 = getBits (b, 0, 16, 4);
 d.last_descriptor_number	 = getBits (b, 0, 20, 4);
 getISO639_3 (d.ISO639_2_language_code, b+3);	
 d.length_of_items		 = getBits (b, 0, 48, 8);


 out_SB_NL (4,"Descriptor_number: ",d.descriptor_number);
 out_SB_NL (4,"Last_descriptor_number: ",d.last_descriptor_number);
 out_nl    (4,"ISO639_2_language_code:  %3.3s", d.ISO639_2_language_code);
 out_SB_NL (5,"Length_of_items: ",d.length_of_items);


 b   += 7;
 lenB = d.descriptor_length - 5;
 len1 = d.length_of_items;

 indent (+1);
 while (len1 > 0) {
 
   d2.item_description_length	 = getBits (b, 0, 0, 8);
   out_NL (4);
   out_SB_NL (5,"Item_description_length: ",d2.item_description_length);
   out_nl (4,"Item_description: ");
      print_name (4, b+1, d2.item_description_length);
	out_NL (4);

   b += 1 + d2.item_description_length;
   

   d2.item_length	 	 = getBits (b, 0, 0, 8);
   out_SB_NL (5,"Item_length: ",d2.item_length);
   out (4,"Item: ");
      print_name (4, b+1, d2.item_length);
	out_NL (4);

   b += 1 + d2.item_length;

   len1 -= (2 + d2.item_description_length + d2.item_length);
   lenB -= (2 + d2.item_description_length + d2.item_length);

 }
 out_NL (4);
 indent (-1);



   d.text_length		 = getBits (b, 0, 0, 8);
   b += 1;
   lenB -= 1;


   out_SB_NL (5,"Text_length: ",d.text_length);
   out (4,"Text: ");
	print_name (4, b,d.text_length);
	out_NL (4);


}







/*
  0x4F  Time Shifted Event  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_TimeShiftedEvent  (u_char *b)

{

 typedef struct  _descTimeShiftedEvent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reference_service_id;
    u_int      reference_event_id;

 } descTimeShiftedEvent;


 descTimeShiftedEvent d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reference_service_id		 = getBits (b, 0, 16, 16);
 d.reference_event_id		 = getBits (b, 0, 32, 16);

 
 out_SW_NL (4,"Reference_service_id: ",d.reference_service_id);
 out_SW_NL (4,"Reference_event_id: ",d.reference_service_id);

}







/*
  0x50  Component descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_Component  (u_char *b)

{

 typedef struct  _descComponent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reserved_1;
    u_int      stream_content;
    u_int      component_type;
    u_int      component_tag;
    u_char     ISO639_2_language_code[4];

    // N2  char Text

 } descComponent;


 descComponent d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reserved_1			 = getBits (b, 0, 16, 4);
 d.stream_content		 = getBits (b, 0, 20, 4);
 d.component_type		 = getBits (b, 0, 24, 8);
 d.component_tag		 = getBits (b, 0, 32, 8);
 getISO639_3 (d.ISO639_2_language_code, b+5);	

 
 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out_SB_NL (4,"Stream_content: ",d.stream_content);
 out_SB_NL (4,"Component_type: ",d.component_type);
 out_nl    (4,"   == Content&Component: (= %s)",
      dvbstrStreamContent_Component_TYPE(
	(d.stream_content << 8) | d.component_type ) );

 out_SB_NL (4,"Component_tag: ",d.component_tag);
 out_nl    (4,"ISO639_2_language_code:  %3.3s", d.ISO639_2_language_code);

 out       (4,"Component-Description: ");
	print_name (4, b+8,d.descriptor_length - 6);
 	out_NL (4);

}








/*
  0x51  Mosaic  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_Mosaic  (u_char *b)

{

 typedef struct  _descMosaic {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      mosaic_entry_point;
    u_int      number_of_horizontal_elementary_cells;
    u_int      reserved_1;
    u_int      number_of_vertical_elementary_cells;

    // N    desc Mosaic2

 } descMosaic;

 typedef struct  _descMosaic2 {
    u_int      logical_cell_id;
    u_int      reserved_1;
    u_int      logical_cell_presentation_info;
    u_int      elementary_cell_field_length;

    // N2   desc Mosaic3

    u_int      cell_linkage_info;
    //  conditional data !! (cell_linkage_info)
    u_int   bouquet_id;
    u_int   original_network_id;
    u_int   transport_stream_id;
    u_int   service_id;
    u_int   event_id;
 } descMosaic2;

 typedef struct  _descMosaic3 {
    u_int      reserved_1;
    u_int      elementary_cell_id;
 } descMosaic3;




 descMosaic    d;
 descMosaic2   d2;
 descMosaic3   d3;
 int           len1,len2;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.mosaic_entry_point                    = getBits (b, 0, 16,  1);
 d.number_of_horizontal_elementary_cells = getBits (b, 0, 17,  3);
 d.reserved_1                            = getBits (b, 0, 20,  1);
 d.number_of_vertical_elementary_cells   = getBits (b, 0, 21,  3);

 
 out_SB_NL (4,"Mosaic_entry_point: ",d.mosaic_entry_point);
 out_nl    (4,"Number_of_horizontal_elementary_cells: %u  (= %d cells)",
	d.number_of_horizontal_elementary_cells,
	d.number_of_horizontal_elementary_cells + 1);
 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out_nl    (4,"  Number_of_vertical_elementary_cells: %u  (= %d cells)",
	d.number_of_vertical_elementary_cells,
	d.number_of_vertical_elementary_cells + 1);

 len1 = d.descriptor_length - 1;
 b   += 3;

 indent(+1);
 while (len1 > 0) {

    d2.logical_cell_id                       = getBits (b, 0,  0,  6);
    d2.reserved_1                            = getBits (b, 0,  6,  7);
    d2.logical_cell_presentation_info        = getBits (b, 0, 13,  3);
    d2.elementary_cell_field_length          = getBits (b, 0, 16,  8);


    out_NL (4);
    out_SB_NL  (4,"Logical_cell_ID: ",d2.logical_cell_id);
    out_SB_NL  (6,"reserved_1: ",d2.reserved_1);
    out_S2B_NL (4,"Logical_cell_presentation_info: ",
	d2.logical_cell_presentation_info,
	dvbstrLogCellPresInfo_TYPE(d2.logical_cell_presentation_info) );
    out_SB_NL (5,"Elementary_cell_field_length: ",
	d2.elementary_cell_field_length);

    b    += 3;
    len2  = d2.elementary_cell_field_length;
    len1 -= (len2 + 3);

    indent (+1);
    while (len2 > 0) {
       d3.reserved_1                         = getBits (b, 0,  0,  2);
       d3.elementary_cell_id                 = getBits (b, 0,  2,  6);

       len2 -= 1;
       b    += 1;

       out_NL (4);	
       out_SB_NL (6,"reserved_1: ",d3.reserved_1);
       out_SB_NL (4,"Elementary_cell_ID: ",d3.elementary_cell_id);

    }  //len2
    indent (-1);


    d2.cell_linkage_info                     = getBits (b, 0,  0,  8);
    b    += 1;
    len1 -= 1;
    out_S2B_NL (4,"Cell_linkage_info: ",
		d2.cell_linkage_info,
		dvbstrCellLinkageInfo_TYPE (d2.cell_linkage_info));


    /*   conditional fields!! */

    switch (d2.cell_linkage_info) {

      case 0x01:
	d2.bouquet_id		 = getBits (b, 0, 0, 16);
	b    += 2;
	len1 -= 2;
	out_S2W_NL (4,"Bouquet_ID: ",d2.bouquet_id,dvbstrBouquetTable_ID(d2.bouquet_id));
	break;

      case 0x02:
      case 0x03:
      case 0x04:
 	d2.transport_stream_id		 = getBits (b, 0, 0, 16);
	d2.original_network_id		 = getBits (b, 0, 16, 16);
	d2.service_id			 = getBits (b, 0, 32, 16);
	b    += 6;
	len1 -= 6;

 	out_SW_NL  (4,"Transport_stream_ID: ",d2.transport_stream_id);
	out_S2W_NL (4,"Original_network_ID: ",d2.original_network_id,
	    dvbstrOriginalNetwork_ID(d2.original_network_id));
	out_S2W_NL (4,"Service_ID: ",d2.service_id,
          " --> refers to PMS program_number");


	if (d2.cell_linkage_info == 0x03)
		out_nl (4,"  --> Service referce to mosaic service");

	if (d2.cell_linkage_info == 0x04) {
		out_nl (4,"  --> Service referce to event");

 		d2.event_id	 = getBits (b, 0, 0, 16);
		b    += 2;
		len1 -= 2;
 		out_SW_NL (4,"Event_ID: ",d2.event_id);
	}

	break;

    } // switch
    

 } // while len1
 indent(-1);

}









/*
  0x52  Stream Identifier descriptor
  ETSI EN 300 468  6.2.xx

*/

void descriptorDVB_StreamIdent (u_char *b)

{

 typedef struct  _descStreamIdent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      component_tag;		
 } descStreamIdent;


 descStreamIdent  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.component_tag		 = getBits (b,0,16,8);


 out_SB_NL (4,"Component_tag: ",d.component_tag);

}








/*
  0x53  CA Identifier  descriptor 
  ETSI EN 300 468   6.2.xx
*/

void descriptorDVB_CAIdentifier  (u_char *b)

{

 typedef struct  _descCAIdent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N   CA_SysIDs

 } descCAIdent;


 descCAIdent d;
 u_int       CA_system_ID;
 int         len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 len = d.descriptor_length; 
 b  += 2;

 indent (+1);
 while (len > 0) {

   CA_system_ID 		 = getBits (b,0,0,16);

   out_S2W_NL (4,"CA_system_ID: ",CA_system_ID,
      dvbstrCASystem_ID(CA_system_ID));

   b   += 2;
   len -= 2;
 }
 indent (-1);

}









/*
  0x54  Content  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_Content  (u_char *b)

{

 typedef struct  _descContent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		
 } descContent;

 typedef struct  _descContent2 {
    u_int      content_nibble_level_1;
    u_int      content_nibble_level_2;
    u_int      user_nibble_1;
    u_int      user_nibble_2;
 } descContent2;


 descContent   d;
 descContent2  d2;
 int           len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 len = d.descriptor_length;
 b  += 2;
 
 indent (+1);
 while ( len > 0) {
    d2.content_nibble_level_1	 = getBits (b,0, 0,4);
    d2.content_nibble_level_2	 = getBits (b,0, 4,4);
    d2.user_nibble_1		 = getBits (b,0, 8,4);
    d2.user_nibble_2		 = getBits (b,0,12,4);

    b   += 2;
    len -= 2;
 
    out_SB_NL (4,"Content_nibble_level_1: ", d2.content_nibble_level_1);
    out_SB_NL (4,"Content_nibble_level_2: ", d2.content_nibble_level_2);
    out_nl    (4,"   [= %s]", dvbstrContentNibble_TYPE (
	(d2.content_nibble_level_1 << 8) | d2.content_nibble_level_2) );

    out_SB_NL (4,"User_nibble_1: ", d2.user_nibble_1);
    out_SB_NL (4,"User_nibble_2: ", d2.user_nibble_2);
    out_NL (4);
 }
 indent (-1);

}











/*
  0x55  Parental Rating  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_ParentalRating (u_char *b)

{

 typedef struct  _descParentalRating {
    u_int      descriptor_tag;
    u_int      descriptor_length;		
 } descParentalRating;

 typedef struct  _descParentalRating2 {
    u_char     country_code[4];
    u_int      rating;		
 } descParentalRating2;



 descParentalRating   d;
 descParentalRating2  d2;
 int                  len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 len = d.descriptor_length;
 b  += 2;

 indent (+1);
 while (len > 0) {
    strncpy (d2.country_code, b, 3);	
    d2.rating			 = getBits (b,0,24,8);

    b += 4;
    len -= 4;

    out_nl     (4,"Country_code:  %3.3s", d2.country_code);
    out_S2B_NL (4,"Rating:  ", d2.rating,
	dvbstrParentalRating_TYPE (d2.rating));
    out_NL (4);

 }
 indent (-1);
 
}









/*
  -- 0x56 Teletext descriptor
*/

void descriptorDVB_Teletext (u_char *b)

{
 /* ETSI EN 300 468   6.2.xx */

 typedef struct  _descTeletext {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N TeleTextList desc

 } descTeletext;

 typedef struct  _descTeletextList {
    u_char     ISO_639_language_code[4];
    u_int      teletext_type;
    u_int      teletext_magazine_number;
    u_int      teletext_page_number;
 } descTeletextList;


 descTeletext      d;
 descTeletextList  d2;
 int               len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 len = d.descriptor_length;
 b  += 2;

 indent (+1);
 while ( len > 0) {
    getISO639_3 (d2.ISO_639_language_code, b);	
    d2.teletext_type		= getBits (b,0,24,5);
    d2.teletext_magazine_number	= getBits (b,0,29,3);
    d2.teletext_page_number	= getBits (b,0,32,8);

    b += 5;
    len -= 5;

    out_nl     (4,"ISO639_language_code:  %3.3s", d2.ISO_639_language_code);
    out_S2B_NL (4,"Teletext_tye: ", d2.teletext_type,
	dvbstrTeletext_TYPE (d2.teletext_type));

    out_SB_NL (4,"Teletext_magazine_number: ",d2.teletext_magazine_number);
    out_SB_NL (4,"Teletext_page_number: ",d2.teletext_page_number);
    out_NL (4);
 }
 indent (-1);

}





/*
  0x57  Telephone  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_Telephone  (u_char *b)

{

 typedef struct  _descTelephone {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reserved_1;
    // $$$ ToDO

 } descTelephone;


 descTelephone  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];



 descriptorDVB_any (b);
 out_nl (4," ==> ERROR: Telephone descriptor not implemented, Report!");


}







/*
  0x58  Local Time Offset  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_LocalTimeOffset  (u_char *b)

{

 typedef struct  _descLocalTimeOffset {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N  Descriptor
 } descLocalTimeOffset;

 typedef struct  _descLocalTimeOffset2 {
   u_char        country_code[4];
   u_int         country_region_id;
   u_int         reserved_1;
   u_int         local_time_offset_polarity;
   u_int         local_time_offset;
   u_int         time_of_change_MJD;
   u_int         time_of_change_UTC;
   u_int         next_time_offset;
 } descLocalTimeOffset2;


 descLocalTimeOffset   d;
 descLocalTimeOffset2  d2;
 int                   len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 len = d.descriptor_length;
 b  += 2;
 
 indent (+1);
 while (len > 0) {

    strncpy (d2.country_code, b, 3);	
    d2.country_region_id	 = getBits (b, 0, 24,  6);
    d2.reserved_1		 = getBits (b, 0, 30,  1);
    d2.local_time_offset_polarity = getBits (b, 0, 31,  1);
    d2.local_time_offset	 = getBits (b, 0, 32, 16);
    d2.time_of_change_MJD	 = getBits (b, 0, 48, 16);
    d2.time_of_change_UTC	 = getBits (b, 0, 64, 24);
    d2.next_time_offset		 = getBits (b, 0, 88, 16);

    len -= 13;
    b   += 13;

    out_nl    (4,"Country_code:  %3.3s", d2.country_code);
    out_SB_NL (4,"Country_region_ID: ",d2.country_region_id);
    out_SB_NL (6,"reserved_1: ",d2.reserved_1);

    out_nl    (4,"local_time_offset_polarity: %u  (= %s to UTC)",
	d2.local_time_offset_polarity,
	(d2.local_time_offset_polarity) ? "minus" : "plus");

    out_nl    (4,"Local_time_offset: %02x:%02x",
	d2.local_time_offset >> 8, d2.local_time_offset & 0xFF);

    out       (4,"Time_of_change: ");
      print_time40 (4, d2.time_of_change_MJD,d2.time_of_change_UTC);
      out_NL (4);

    out_nl    (4,"Next_time_offset: %02x:%02x ",
	d2.next_time_offset >> 8, d2.next_time_offset & 0xFF);

    out_NL(4);

 }
 indent (-1);


}






/*
  0x59  Subtitling  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_Subtitling  (u_char *b)

{

 typedef struct  _descSubTitling {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

 } descSubTitling;

 typedef struct  _descSubTitling2 {
    u_char     ISO_639_language_code[4];
    u_int      subtitling_type;
    u_int      composition_page_id;
    u_int      ancillary_page_id;
 } descSubTitling2;


 descSubTitling   d;
 descSubTitling2  d2;
 int              len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 len = d.descriptor_length;
 b  += 2;

 indent (+1);
 while ( len > 0) {
    getISO639_3 (d2.ISO_639_language_code, b);	
    d2.subtitling_type		= getBits (b,0,24, 8);
    d2.composition_page_id	= getBits (b,0,32,16);
    d2.ancillary_page_id	= getBits (b,0,48,16);

    b   += 8;
    len -= 8;

    out_nl  (4,"  ISO639_language_code:  %3.3s", d2.ISO_639_language_code);
    out_S2B_NL (4,"Subtitling_type: ", d2.subtitling_type,
	dvbstrStreamContent_Component_TYPE (
	    (0x03 << 8) | d2.subtitling_type));

    out_SW_NL (4,"Composition_page_id: ",d2.composition_page_id);
    out_SW_NL (4,"Ancillary_page_id: ",d2.ancillary_page_id);
    out_NL (4);
 }
 indent (-1);

}








/*
  0x5A TerrestDelivSys  descriptor  (Terrestrial delivery system descriptor)
*/

void descriptorDVB_TerrestDelivSys (u_char *b)

{
 /* ETSI 300 468    6.2.xx */

 typedef struct  _descCDS {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_long     centre_frequency;
    u_int      bandwidth;
    u_int      reserved_1;
    u_int      constellation;
    u_long     hierarchy_information;
    u_int      code_rate_HP_stream;
    u_int      code_rate_LP_stream;
    u_int      guard_interval;
    u_int      transmission_mode; 
    u_int      other_frequency_flag;
    u_int      reserved_2;

 } descTDS;

 descTDS  d;
 //int      i;




 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 d.centre_frequency		 = getBits (b, 0, 16, 32);
 d.bandwidth			 = getBits (b, 0, 48, 3);
 d.reserved_1			 = getBits (b, 0, 51, 5);
 d.constellation		 = getBits (b, 0, 56, 2);
 d.hierarchy_information	 = getBits (b, 0, 58, 3);
 d.code_rate_HP_stream		 = getBits (b, 0, 61, 3);
 d.code_rate_LP_stream		 = getBits (b, 0, 64, 3);
 d.guard_interval		 = getBits (b, 0, 67, 2);
 d.transmission_mode		 = getBits (b, 0, 69, 2);
 d.other_frequency_flag		 = getBits (b, 0, 71, 1);
 d.reserved_2			 = getBits (b, 0, 72, 32);


 out_nl (4,"Center frequency: 0x%08x (= %lu Hz)",d.centre_frequency,
	 d.centre_frequency * 10 );
 out_S2B_NL (4,"Bandwidth: ",d.bandwidth,
	 dvbstrTerrBandwidth_SCHEME (d.bandwidth));
 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out_S2B_NL (4,"Constellation: ",d.constellation,
	 dvbstrTerrConstellation_FLAG(d.constellation));
 out_S2B_NL (4,"Hierarchy information: ",d.hierarchy_information,
	 dvbstrTerrHierarchy_FLAG(d.hierarchy_information));
 out_S2B_NL (4,"Code_rate_HP_stream: ",d.code_rate_HP_stream,
	 dvbstrTerrCodeRate_FLAG(d.code_rate_HP_stream));
 out_S2B_NL (4,"Code_rate_LP_stream: ",d.code_rate_LP_stream,
	 dvbstrTerrCodeRate_FLAG(d.code_rate_LP_stream));
 out_S2B_NL (4,"Guard_interval: ",d.guard_interval,
	 dvbstrTerrGuardInterval_FLAG(d.guard_interval));
 out_S2B_NL (4,"Transmission_mode: ",d.transmission_mode,
	 dvbstrTerrTransmissionMode_FLAG(d.transmission_mode));
 out_SB_NL (4,"Other_frequency_flag: ",d.other_frequency_flag);
 out_SL_NL (6,"reserved_2: ",d.reserved_2);


}







/*
  0x5B  Multilingual Network Name  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_MultilingNetworkName (u_char *b)

{

 typedef struct  _descMultiNetName {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    //  N .. List2

 } descMultiNetName;

 typedef struct  _descMultiNetName2 {
    u_char     ISO639_2_language_code[4];
    u_int      network_name_length;

    //  N2 ..  char

 } descMultiNetName2;



 descMultiNetName   d;
 descMultiNetName2  d2;
 int                len1;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 b += 2;
 len1 = d.descriptor_length;

 indent (+1);
 while (len1 > 0 ) {

    getISO639_3 (d2.ISO639_2_language_code, b);
    d2.network_name_length	 = getBits (b, 0, 24, 8);

    out_nl    (4,"ISO639_2_language_code:  %3.3s", d2.ISO639_2_language_code);
    out_SB_NL (5,"Network_name_length: ",d2.network_name_length);
    out       (4,"Network_name: ");
	print_name (4, b+4,d2.network_name_length);
 	out_NL (4);
    out_NL (4);

    len1 -= (4 + d2.network_name_length);
    b    +=  4 + d2.network_name_length;

 }
 indent (-1);

}








/*
  0x5C  Multilingual Bouquet Name  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_MultilingBouquetName (u_char *b)

{

 typedef struct  _descMultiBouqName {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    //  N .. List2

 } descMultiBouqName;

 typedef struct  _descMultiBouqName2 {
    u_char     ISO639_2_language_code[4];
    u_int      bouquet_name_length;

    //  N2 ..  char

 } descMultiBouqName2;



 descMultiBouqName   d;
 descMultiBouqName2  d2;
 int                 len1;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 b += 2;
 len1 = d.descriptor_length;

 indent (+1);
 while (len1 > 0 ) {

    getISO639_3 (d2.ISO639_2_language_code, b);
    d2.bouquet_name_length	 = getBits (b, 0, 24, 8);

    out_nl    (4,"ISO639_2_language_code:  %3.3s", d2.ISO639_2_language_code);
    out_SB_NL (5,"Bouquet_name_length: ",d2.bouquet_name_length);
    out       (4,"Bouquet_name: ");
	print_name (4, b+4,d2.bouquet_name_length);
 	out_NL (4);
    out_NL (4);

    len1 -= (4 + d2.bouquet_name_length);
    b    +=  4 + d2.bouquet_name_length;

 }
 indent (-1);

}






/*
  0x5D  Multilingual Service Name  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_MultilingServiceName (u_char *b)

{

 typedef struct  _descMultiServiceName {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    //  N .. List2

 } descMultiServiceName;

 typedef struct  _descMultiServiceName2 {
    u_char     ISO639_2_language_code[4];
    u_int      service_provider_name_length;

    //  N2 ..  char

    u_int      service_name_length;

    //  N3 ..  char

 } descMultiServiceName2;



 descMultiServiceName   d;
 descMultiServiceName2  d2;
 int                    len1;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 b += 2;
 len1 = d.descriptor_length;

 indent (+1);
 while (len1 > 0 ) {

    getISO639_3 (d2.ISO639_2_language_code, b);
    d2.service_provider_name_length	 = getBits (b, 0, 24, 8);

    out_nl    (4,"ISO639_2_language_code:  %3.3s", d2.ISO639_2_language_code);
    out_SB_NL (5,"Service_provider_name_length: ",d2.service_provider_name_length);
    out       (4,"Service_provider_name: ");
	print_name (4, b+4,d2.service_provider_name_length);
 	out_NL (4);

    len1 -= (4 + d2.service_provider_name_length);
    b    +=  4 + d2.service_provider_name_length;


    d2.service_name_length	 = getBits (b, 0, 24, 8);
    out_SB_NL (5,"Service_name_length: ",d2.service_name_length);
    out       (4,"Service_name: ");
	print_name (4, b+4,d2.service_name_length);
 	out_NL (4);

    len1 -= (4 + d2.service_name_length);
    b    +=  4 + d2.service_name_length;

    out_NL (4);

 }
 indent (-1);

}







/*
  0x5E  Multilingual Component  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_MultilingComponent (u_char *b)

{

 typedef struct  _descMultiComponent {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      component_tag;

    //  N .. List2

 } descMultiComponent;

 typedef struct  _descMultiComponent2 {
    u_char     ISO639_2_language_code[4];
    u_int      text_description_length;

    //  N2 ..  char

 } descMultiComponent2;



 descMultiComponent   d;
 descMultiComponent2  d2;
 int                  len1;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 d.component_tag 		 = b[2];

 out_SB_NL (4,"Component_tag: ",d.component_tag);

 b += 3;
 len1 = d.descriptor_length + 1;

 indent (+1);
 while (len1 > 0 ) {

    getISO639_3 (d2.ISO639_2_language_code, b);
    d2.text_description_length	 = getBits (b, 0, 24, 8);

    out_nl    (4,"ISO639_2_language_code:  %3.3s", d2.ISO639_2_language_code);
    out_SB_NL (5,"Text_description_length: ",d2.text_description_length);
    out       (4,"Text_description: ");
	print_name (4, b+4,d2.text_description_length);
 	out_NL (4);
    out_NL (4);

    len1 -= (4 + d2.text_description_length);
    b    +=  4 + d2.text_description_length;

 }
 indent (-1);

}








/*
  0x5F  Private Data Specifier  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_PrivateDataSpecifier (u_char *b)

{

 typedef struct  _descPrivDataSpec {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_long     private_data_specifier;

 } descPrivDataSpec;


 descPrivDataSpec d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.private_data_specifier	 = getBits (b, 0, 16, 32);

 out_S2L_NL (4,"PrivateDataSpecifier: ",d.private_data_specifier,
	dvbstrPrivateDataSpecifier_ID( d.private_data_specifier) );

}







/*
  0x60  Service Move  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_ServiceMove  (u_char *b)

{

 typedef struct  _descServMove {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      new_original_network_id;
    u_int      new_transport_stream_id;
    u_int      new_service_id;

 } descServMove;


 descServMove    d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.new_original_network_id	 = getBits (b, 0, 16, 16);
 d.new_transport_stream_id	 = getBits (b, 0, 32, 16);
 d.new_service_id		 = getBits (b, 0, 48, 16);



  out_S2W_NL (4,"New_original_network_ID: ",d.new_original_network_id,
	dvbstrOriginalNetwork_ID(d.new_original_network_id));
  out_SW_NL  (4,"New_transport_stream_ID: ",d.new_transport_stream_id);
  out_SW_NL (4,"Service_ID: ",d.new_service_id);

}








/*
  0x61  Short Smoothing Buffer  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_ShortSmoothingBuffer  (u_char *b)

{

 typedef struct  _descSSBuf {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      sb_size;
    u_int      sb_leak_rate;

 } descSSBuf;


 descSSBuf  d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.sb_size			 = getBits (b, 0, 16, 2);
 d.sb_leak_rate			 = getBits (b, 0, 16, 6);

 
 out_S2B_NL (4,"sb_size: ", d.sb_size,
	dvbstrShortSmoothingBufSize_TYPE (d.sb_size) );
 out_S2B_NL (4,"sb_leak_rate: ", d.sb_leak_rate,
	dvbstrShortSmoothingBufLeakRate_TYPE (d.sb_leak_rate) );

 out_nl (6,"Reserved:");
   printhexdump_buf (6, b+3,d.descriptor_length-1);


}










/*
  0x62  Frequency List descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_FrequencyList  (u_char *b)

{

 typedef struct  _descFreqList {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reserved_1;
    u_int      coding_type;
 } descFreqList;

 typedef struct  _descFreqList2 {
    u_long     centre_frequency;
 } descFreqList2;


 descFreqList   d;
 descFreqList2  d2;
 int            len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reserved_1			 = getBits (b, 0, 16, 6);
 d.coding_type			 = getBits (b, 0, 22, 2);

 

 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out_S2B_NL (4,"coding_type: ",d.coding_type,
	dvbstrDelivSysCoding_TYPE(d.coding_type));


 b += 3;
 len = d.descriptor_length - 1;
 indent (+1);
 while (len > 0) {

    d2.centre_frequency		 = getBits (b, 0, 0, 32);

    out (4,"Centre_frequency: %08lx  ",d2.centre_frequency);
    switch (d.coding_type) {
	case 0x01:
 	  out_nl (4,"(= %3lx.%05lx GHz)",
	    d2.centre_frequency >> 20, d2.centre_frequency & 0x000FFFFF );
	  break;

	case 0x02:
 	  out_nl (4,"(= %3lx.%05lx MHz)",
	    d2.centre_frequency >> 16, d2.centre_frequency & 0x0000FFFF );
	  break;

	case 0x03:
 	  out_nl (4,"(= %lu Hz)", d2.centre_frequency * 10 );
	  break;
    }

    len -= 4;
    b   += 4;
 }
 indent (-1);


}







/*
  0x63  Partial Transport Stream Descriptor
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_PartialTransportStream  (u_char *b)

{

 typedef struct  _descPartTranspStream {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      reserved_1;
    u_long     peak_rate;
    u_int      reserved_2;
    u_long     minimum_overall_smoothing_rate;
    u_int      reserved_3;
    u_int      maximum_overall_smoothing_buffer;

 } descPartTranspStream;


 descPartTranspStream   d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reserved_1			 = getBits (b, 0, 16, 2);
 d.peak_rate			 = getBits (b, 0, 18, 22);
 d.reserved_2			 = getBits (b, 0, 40, 2);
 d.minimum_overall_smoothing_rate= getBits (b, 0, 42, 22);
 d.reserved_3			 = getBits (b, 0, 64, 2);
 d.maximum_overall_smoothing_buffer= getBits (b, 0, 66, 14);

 

 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out_nl    (4,"peak_rate: 0x%06lx  (= %lu bits/sec)",
	d.peak_rate,d.peak_rate/400);

 out_SB_NL (6,"reserved_2: ",d.reserved_2);
 out_nl    (4,"minimum_overall_smoothing_rate: 0x%06lx  (= %lu bits/sec)",
	d.minimum_overall_smoothing_rate,
	d.minimum_overall_smoothing_rate/400);

 out_SB_NL (6,"reserved_3: ",d.reserved_3);
 out_nl    (4,"maximum_overall_smoothing_buffer: 0x%04x  (= %lu bits/sec)",
	d.maximum_overall_smoothing_buffer,
	d.maximum_overall_smoothing_buffer/400);


}









/*
  0x64  DataBroadcast  descriptor 
  ETSI EN 300 468    6.2.xx
*/

void descriptorDVB_DataBroadcast (u_char *b)

{

 typedef struct  _descDataBroadcast {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      data_broadcast_id;
    u_int      component_tag;
    u_int      selector_length;

    // N   bytes

    u_char     ISO639_2_language_code[4];
    u_int      text_length;

    // N2  char 

 } descDataBroadcast;


 descDataBroadcast d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.data_broadcast_id		 = getBits (b, 0, 16, 16);
 d.component_tag		 = getBits (b, 0, 32, 8);
 d.selector_length		 = getBits (b, 0, 40, 8);
 b += 6;

 
 out_S2W_NL (4,"Data_broadcast_ID: ",d.data_broadcast_id,
	dvbstrDataBroadcast_ID(d.data_broadcast_id));

 out_SB_NL (4,"Component_tag: ",d.component_tag);
 out_SB_NL (5,"Selector_length: ",d.selector_length);


 if (d.data_broadcast_id == 0x0005) {
	 // -- EN 301 192 Multi-protocol-encapsulation!

	 {
 		typedef struct  _descMultiProtEncaps {
			u_int	MAC_address_range;
			u_int	MAC_ip_mapping_flag;
			u_int	alignment_indicator;
			u_int   reserved;
			u_int   max_sections_per_datagram;
 		} descMultiProtEncaps;


		descMultiProtEncaps e;

 		out_nl    (4,"Multi_Protocol_Encapsulation [EN 301 192]:");
		indent (+1);
 		e.MAC_address_range	 = getBits (b, 0,  0,  3);
 		e.MAC_ip_mapping_flag	 = getBits (b, 0,  3,  1);
 		e.alignment_indicator	 = getBits (b, 0,  4,  1);
 		e.reserved		 = getBits (b, 0,  5,  3);
 		e.max_sections_per_datagram = getBits (b, 0,  8,  8);

 		out_S2B_NL (5,"MAC_address_range: ",e.MAC_address_range,
			dvbstrMultiProtEncapsMACAddrRangeField (e.MAC_address_range) );
 		out_SB_NL (5,"MAC_ip_mapping_flag: ",e.MAC_ip_mapping_flag);
 		out_S2B_NL (5,"alignment_indicator: ",e.alignment_indicator,
					(e.alignment_indicator) ?"32 bit": "8 bit");
 		out_SB_NL (6,"reserved: ",e.reserved);
 		out_SB_NL (5,"max_sections_per_datagram: ",e.max_sections_per_datagram);
		indent (-1);

	 }

 } else if (d.data_broadcast_id == 0x0006) {
	 /* $$$ TODO EN 301 192 8.3.1 */
 			out_nl    (4,"TODO Data_Carousel_info:");
		 	printhexdump_buf (4,  b, d.selector_length);
 } else if (d.data_broadcast_id == 0x0007) {
	 /* $$$ TODO EN 301 192 9.3.2 */
 			out_nl    (4,"TODO Object_Carousel_Info:");
		 	printhexdump_buf (4,  b, d.selector_length);
 } else if (d.data_broadcast_id == 0x0009) {
	 /* $$$ TODO EN 301 192 10.2.1 */
 			out_nl    (4,"TODO higher_protocol_asynchronous_data_info:");
		 	printhexdump_buf (4,  b, d.selector_length);
 } else {
 	out_nl    (4,"Selector-Bytes:");
 	printhexdump_buf (4,  b, d.selector_length);
 }

 b += d.selector_length;
 getISO639_3 (d.ISO639_2_language_code, b);
 d.text_length			 = getBits (b, 0, 24, 8);

 out_nl    (4,"ISO639_2_language_code:  %3.3s", d.ISO639_2_language_code);
 out_SB_NL (5,"Text_length: ",d.text_length);
 out       (4,"Text: ");
	print_name (4, b+4,d.text_length);
 	out_NL (4);

}








/*
  0x65  CA System descriptor 
  ETSI EN 300 468     6.2.x
*/

void descriptorDVB_CASystem (u_char *b)

{


  descriptorDVB_any (b);

}






/*
  0x66  Data Broadcast ID  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_DataBroadcastID  (u_char *b)

{

 typedef struct  _descDataBroadcastID {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      data_broadcast_id;

    //  N ... id_selector bytes
 } descDataBroadcastID;


 descDataBroadcastID   d;
 int 		       len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.data_broadcast_id		 = getBits (b, 0, 16, 16);
 b+=4;
 len = d.descriptor_length -2;

 
 out_S2W_NL (4,"Data_broadcast_ID: ",d.data_broadcast_id,
	dvbstrDataBroadcast_ID(d.data_broadcast_id));

 // $$$ ID selector bytes may depend on databroadcast_id
 // $$$ do further more selection here
 // $$$ EN 301 192 Stuff TODO
 
 if (d.data_broadcast_id == 0x000B) {
	 // -- EN 301 192 PSI Signalling IP/MAC Notification Table

	 {
 		typedef struct  _descIPMAC_NOTIF_TABLE {
			u_int	platform_id_data_length;
			// inner loop
			u_long	platform_id;
			u_int	action_type;
			u_int	reserved;
			u_int	INT_versioning_flag;
			u_int	INT_version;
 		} descIPMAC_NOTIF_TABLE;


		descIPMAC_NOTIF_TABLE   d;
		int 		 	len2;


 		out_nl    (4,"IP/MAC Notification [EN 301 192]:");

 		d.platform_id_data_length = getBits (b, 0,  0,  8);
 		out_SW_NL (5,"Platform_id_data_length: ",d.platform_id_data_length);
		b++;
		len--;
		len2 = d.platform_id_data_length;

		indent (+1);
		while (len2 > 0) {
        	   d.platform_id  		= getBits (b, 0,  0, 24);
        	   d.action_type		= getBits (b, 0, 24,  8);
        	   d.reserved			= getBits (b, 0, 32,  2);
        	   d.INT_versioning_flag	= getBits (b, 0, 34,  1);
        	   d.INT_version		= getBits (b, 0, 35,  5);
		   b += 5;
		   len -= 5;
		   len2 -= 5;

 		   out_SL_NL  (5,"Platform_id: ",d.platform_id);	/* $$$$ TODO: platform_id_str nach EN 162 */
 		   out_SB_NL  (5,"Action_type: ",d.action_type);
 		   out_SB_NL  (5,"reserved: ",d.reserved);
 		   out_SB_NL  (5,"INT_versioning_flag: ",d.INT_versioning_flag);
 		   out_SB_NL  (5,"INT_version: ",d.INT_version);

		}
		indent (-1);
 	
		out_nl (5,"Private Data: ");
	     		printhexdump_buf (5, b, len);


	 }
	
 } else {

 	out_nl (4,"ID_selector_bytes: ");
     	printhexdump_buf (4, b, len);
 }

}





/*
  0x67  Transport Stream  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_TransportStream  (u_char *b)

{

 typedef struct  _descTransportStream {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    //   N ... bytes
 } descTransportStream;


 descTransportStream   d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 
 out_nl (4,"Transport-stream-bytes: ");
     printhexdump_buf (4, b+2, d.descriptor_length);

}








/*
  0x68  DSNG  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_DSNG  (u_char *b)

{

 typedef struct  _descDSNG {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    //  N ... bytes
 } descDSNG;


 descDSNG   d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 out_nl (4,"DSNG-bytes: ");
     printhexdump_buf (4, b+2, d.descriptor_length);

}



/*
  0x69  PDC  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_PDC  (u_char *b)

{

 typedef struct  _descPDC {
    u_int      descriptor_tag;
    u_int      descriptor_length;


    u_int      reserved_1;
    u_long     programme_identification_label;
    // ... splits in
    u_int     day;
    u_int     month;
    u_int     hour;
    u_int     minute;

 } descPDC;


 descPDC   d;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.reserved_1			 = getBits (b, 0, 16, 4);
 d.programme_identification_label = getBits (b, 0, 20, 20);

    d.day     = getBits (b,0,20,5);
    d.month   = getBits (b,0,25,4);
    d.hour    = getBits (b,0,29,5);
    d.minute  = getBits (b,0,34,6);


 out_SB_NL (6,"reserved_1: ",d.reserved_1);
 out       (4,"Programme_identification_label: 0x%05lx ",
	d.programme_identification_label);
 out       (4,"[= month=%d  day=%d   hour=%d  min=%d]",
	d.month, d.day, d.hour, d.minute);
 out_NL (4);

}





/*
  0x6A  AC-3  descriptor 
  ETSI EN 300 468    ANNEX E 
*/

void descriptorDVB_AC3  (u_char *b)

{

 typedef struct  _descAC3 {
    u_int      descriptor_tag;
    u_int      descriptor_length;

    u_int      AC3_type_flag;
    u_int      bsid_flag;
    u_int      mainid_flag;
    u_int      asvc_flag;
    u_int      reserved_1;

    // conditional vars
    u_int      AC3_type;
    u_int      bsid_type;
    u_int      mainid_type;
    u_int      asvc_type;

    // N ...  bytes add info

 } descAC3;


 descAC3   d;
 int       len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.AC3_type_flag		 = getBits (b, 0, 16, 1);
 d.bsid_flag			 = getBits (b, 0, 17, 1);
 d.mainid_flag			 = getBits (b, 0, 18, 1);
 d.asvc_flag			 = getBits (b, 0, 19, 1);
 d.reserved_1			 = getBits (b, 0, 20, 4);



 out_SB_NL (4,"AC3_type_flag: ",d.AC3_type_flag);
 out_SB_NL (4,"bsid_flag: ",d.bsid_flag);
 out_SB_NL (4,"mainid_flag: ",d.mainid_flag);
 out_SB_NL (4,"asvc_flag: ",d.asvc_flag);
 out_SB_NL (6,"reserved_1: ",d.reserved_1);

 b   += 3;
 len  = d.descriptor_length - 2;

 if (d.AC3_type_flag) {
     d.AC3_type			 = b[0];
     b++;
     len--;
     out_SB_NL (4,"AC3_type: ",d.AC3_type);
 }

 if (d.bsid_flag) {
     d.bsid_flag		 = b[0];
     b++;
     len--;
     out_SB_NL (4,"bsid_flag: ",d.bsid_flag);
 }

 if (d.mainid_flag) {
     d.mainid_flag		 = b[0];
     b++;
     len--;
     out_SB_NL (4,"mainid_flag: ",d.mainid_flag);
 }

 if (d.asvc_flag) {
     d.asvc_flag		 = b[0];
     b++;
     len--;
     out_SB_NL (4,"asvc_flag: ",d.asvc_flag);
 }

 out_nl (4,"Additional info:");
    printhexdump_buf (4, b, len);

}





/*
  0x6B  Ancillary Data  descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_AncillaryData  (u_char *b)

{

 typedef struct  _descAncillaryData {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      ancillary_data_identifier;

 } descAncillaryData;


 descAncillaryData  d;
 u_int              i;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.ancillary_data_identifier	 = b[2];

 out_SB_NL (4,"Ancillary_data_identifier: ",
	d.ancillary_data_identifier);


 // $$$ the following should normally in dvbStrAncillaryData...()

 i = d.ancillary_data_identifier;
 indent (+1);
   if (i & 0x01) out_nl (4,"[= DVD-Video Ancillary Data]");
   if (i & 0x02) out_nl (4,"[= Extended Ancillary Data]");
   if (i & 0x04) out_nl (4,"[= Announcement Switching Data]");
   if (i & 0x08) out_nl (4,"[= DAB Ancillary Data]");
   if (i & 0x10) out_nl (4,"[= Scale Factor Error Check]");
   if (i & 0xE0) out_nl (4,"[= reserved ]");
 indent (-1);


}




/*
  0x6C  Cell List descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_CellList  (u_char *b)

{

  // $$$ Todo  6.2.7

 descriptorDVB_any (b);
 out_nl (4," ==> ERROR: CellList descriptor not implemented, Report!");


}




/*
  0x6D  Cell Frequency Link descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_CellFrequencyLink  (u_char *b)

{
 // $$$$ todo 300468  6.2.6
 //
 descriptorDVB_any (b);
 out_nl (4," ==> ERROR: CellFrequencyLink descriptor not implemented, Report!");

}





/*
  0x6E  Announcement Support descriptor 
  ETSI EN 300 468     6.2.xx
*/

void descriptorDVB_AnnouncementSupport (u_char *b)

{

 typedef struct  _descAnnouncementSupport {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      announcement_support_indicator;
    // N .. Announcement 2

 } descAnnouncementSupport;

 typedef struct  _descAnnouncementSupport2 {
    u_int      announcement_type;
    u_int      reserved_1;
    u_int      reference_type;
    // conditional data
    u_int      original_network_id;
    u_int      transport_stream_id;
    u_int      service_id;
    u_int      component_tag;


 } descAnnouncementSupport2;



 descAnnouncementSupport   d;
 descAnnouncementSupport2  d2;
 int                       len;
 int                       i;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.announcement_support_indicator = getBits (b, 0, 16, 16);
 b   += 4;
 len = d.descriptor_length - 4;


 out_SW_NL (4,"Announcement_support_indicator: ",
	d.announcement_support_indicator);

    i = d.announcement_support_indicator;
    // $$$ should be in dvbstr...()
    indent (+1);
      if (i & 0x01) out_nl (4,"[= Emergency alarm]");
      if (i & 0x02) out_nl (4,"[= Road Traffic Flash]");
      if (i & 0x04) out_nl (4,"[= Public Transport Flash]");
      if (i & 0x08) out_nl (4,"[= Warning message]");
      if (i & 0x10) out_nl (4,"[= News flash]");
      if (i & 0x20) out_nl (4,"[= Weather flash]");
      if (i & 0x40) out_nl (4,"[= Event announcement]");
      if (i & 0x80) out_nl (4,"[= Personal call]");
      if (i & 0xFF00) out_nl (4,"[= reserved ]");
    indent (-1);
    out_NL (4);

    

 indent (+1);
 while (len > 0) {
    d2.announcement_type	 = getBits (b, 0,  0,  4);
    d2.reserved_1		 = getBits (b, 0,  4,  1);
    d2.reference_type		 = getBits (b, 0,  5,  3);

    b   += 1;
    len -= 1;

    out_S2B_NL (4,"Announcement_type: ",d2.announcement_type,
	dvbstrAnnouncement_TYPE (d2.announcement_type) );
    out_SB_NL  (6,"reserved_1: ",d2.reserved_1);
    out_S2B_NL (4,"reference_type: ",d2.reference_type,
	dvbstrAnnouncementReference_TYPE (d2.reference_type) );


    i = d2.reference_type;
    if (i == 1 || i == 2 || i == 3) {
       d2.original_network_id	 = getBits (b, 0,  0, 16);
       d2.transport_stream_id	 = getBits (b, 0, 16, 16);
       d2.service_id		 = getBits (b, 0, 32, 16);
       d2.component_tag		 = getBits (b, 0, 48,  8);

       b   += 7;
       len -= 7;

       out_S2W_NL (4,"Original_network_ID: ",d2.original_network_id,
           dvbstrOriginalNetwork_ID(d2.original_network_id));
       out_SW_NL  (4,"Transport_stream_ID: ",d2.transport_stream_id);
       out_SW_NL  (4,"Service_ID: ",d2.service_id);
       out_SB_NL  (4,"Component_tag: ",d2.component_tag);

    } // if


 } // while
 indent (-1); 

}





/*
  0x6F  Application Signalling descriptor 
  ETSI EN 300 468     6.2.x
*/

void descriptorDVB_ApplicationSignalling (u_char *b)

{

 typedef struct  _descApplSignalling {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // N  AppSignalling2

 } descApplSignalling;

 typedef struct  _descApplSignalling2 {
    u_int      application_type;	// 16
    u_int      reserved;		// 3
    u_int      AIT_version_nr;		// 5
 } descApplSignalling2;



 descApplSignalling        d;
 descApplSignalling2       d2;
 int                       len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 b   += 2;
 len = d.descriptor_length - 0;

 while ( len > 0) {
    d2.application_type          = getBits (b, 0, 0, 16);
    d2.reserved                  = getBits (b, 0,16,  3);
    d2.AIT_version_nr            = getBits (b, 0,19,  5);

    out_SW_NL (4,"Application type: ",d2.application_type);	// $$$ Application type text
    out_SB_NL (6,"reserved: ",d2.reserved);
    out_SB_NL (4,"AIT version nr.: ",d2.AIT_version_nr);

    b += 3;
    len -= 3;
 }

}




/*
  0x70  Adaption Field Data descriptor 
  ETSI EN 300 468     6.2.x
*/

void descriptorDVB_AdaptionFieldData (u_char *b)

{

 typedef struct  _descAdaptionFieldData {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      adaptionFieldDataIdentifier;

    // ? .. 

 } descAdaptionFieldData;



 descAdaptionFieldData     d;
 int                       len;
 int                       i;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.adaptionFieldDataIdentifier   = getBits (b, 0, 16, 8);

 b   += 3;
 len = d.descriptor_length - 1;


 out_SB_NL (4,"Adaption field data identifier: ",
        d.adaptionFieldDataIdentifier);
  indent (+1);
   i = d.adaptionFieldDataIdentifier;
   if (i & 0x01) out_nl (4,"[= announcement switching data field]");
   if (i & 0xFE) out_nl (4,"[= reserved]");
 indent (-1);

}





/*
  0x71  Service Identifier descriptor 
  ETSI EN 300 468     6.2.x
*/

void descriptorDVB_ServiceIdentifier (u_char *b)

{

 typedef struct  _descServiceIdentifier {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    // ? .. 

 } descServiceIdentifier;



 descServiceIdentifier     d;
 int                       len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];


 b   += 2;
 len = d.descriptor_length - 0;

//$$$
 out_nl (4,"Service Identifier:  [no encoding known ToDo, Report!] ");


}



/*
  0x72  Service Availability descriptor 
  ETSI EN 300 468     6.2.x
*/

void descriptorDVB_ServiceAvailability (u_char *b)

{

 typedef struct  _descServiceAvailability {
    u_int      descriptor_tag;
    u_int      descriptor_length;		

    u_int      availability_flag_1;
    u_int      reserved_7;

    //  0..N cell_ids 

 } descServiceAvailability;



 descServiceAvailability   d;
 int                       len;



 d.descriptor_tag		 = b[0];
 d.descriptor_length       	 = b[1];

 d.availability_flag_1           = getBits (b, 0, 16, 1);
 d.reserved_7                    = getBits (b, 0, 16, 7);

 b   += 3;
 len = d.descriptor_length - 1;

 out_SB_NL (4,"Availability flag: ", d.availability_flag_1);
 out_SB_NL (6,"reserved_1: ",d.reserved_7);


 indent (+1);
 while ( len > 0) {
    u_int cellid                 = getBits (b, 0, 0, 16);

    b += 2;
    len -= 2;

    out_S2B_NL (4,"Cell-ID: ",cellid, "[identifies terrestrial cell]");
 }
 indent (-1);

}






/*
 * $$$ TODO // Reminder
 *   EN 301 192 datagram section (Table 3) = 0x3E
 *
 */
