#ifndef SISECTIONS_HPP
#define SISECTIONS_HPP
//
//    $Id: SIsections.hpp,v 1.15 2003/03/03 03:43:58 obi Exp $
//
//    classes for SI sections (dbox-II-project)
//
//    Homepage: http://dbox2.elxsi.de
//
//    Copyright (C) 2001 fnbrd (fnbrd@gmx.de)
//    Copyright (C) 2003 Andreas Oberritter <obi@tuxbox.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <endian.h>

struct SI_section_SDT_header {
	unsigned table_id			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned section_syntax_indicator	: 1;
	unsigned reserved_future_use		: 1;
	unsigned reserved1			: 2;
	unsigned section_length_hi		: 4;
#else
	unsigned section_length_hi		: 4;
	unsigned reserved1			: 2;
	unsigned reserved_future_use		: 1;
	unsigned section_syntax_indicator	: 1;
#endif
	unsigned section_length_lo		: 8;
	unsigned transport_stream_id_hi		: 8;
	unsigned transport_stream_id_lo		: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved2			: 2;
	unsigned version_number			: 5;
	unsigned current_next_indicator		: 1;
#else
	unsigned current_next_indicator		: 1;
	unsigned version_number			: 5;
	unsigned reserved2			: 2;
#endif
	unsigned section_number			: 8;
	unsigned last_section_number		: 8;
	unsigned original_network_id_hi		: 8;
	unsigned original_network_id_lo		: 8;
	unsigned reserved_future_use2		: 8;
} __attribute__ ((packed)) ; // 11 bytes

struct SI_section_NIT_header {
	unsigned table_id			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned section_syntax_indicator	: 1;
	unsigned reserved_future_use		: 1;
	unsigned reserved1			: 2;
	unsigned section_length_hi		: 4;
#else
	unsigned section_length_hi		: 4;
	unsigned reserved1			: 2;
	unsigned reserved_future_use		: 1;
	unsigned section_syntax_indicator	: 1;
#endif
	unsigned section_length_lo		: 8;
	unsigned network_id_hi			: 8;
	unsigned network_id_lo			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved2			: 2;
	unsigned version_number			: 5;
	unsigned current_next_indicator		: 1;
#else
	unsigned current_next_indicator		: 1;
	unsigned version_number			: 5;
	unsigned reserved2			: 2;
#endif
	unsigned section_number			: 8;
	unsigned last_section_number		: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved_future_use2		: 4;
	unsigned network_descriptors_length_hi	: 4;
#else
	unsigned network_descriptors_length_hi	: 4;
	unsigned reserved_future_use2		: 4;
#endif
	unsigned network_descriptors_length_lo	: 8;
} __attribute__ ((packed)) ; // 10 bytes

struct SI_section_BAT_header {
	unsigned table_id			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned section_syntax_indicator	: 1;
	unsigned reserved_future_use		: 1;
	unsigned reserved1			: 2;
	unsigned section_length_hi		: 4;
#else
	unsigned section_length_hi		: 4;
	unsigned reserved1			: 2;
	unsigned reserved_future_use		: 1;
	unsigned section_syntax_indicator	: 1;
#endif
	unsigned section_length_lo		: 8;
	unsigned bouquet_id_hi			: 8;
	unsigned bouquet_id_lo			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved2			: 2;
	unsigned version_number			: 5;
	unsigned current_next_indicator		: 1;
#else
	unsigned current_next_indicator		: 1;
	unsigned version_number			: 5;
	unsigned reserved2			: 2;
#endif
	unsigned section_number			: 8;
	unsigned last_section_number		: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved_future_use2		: 4;
	unsigned bouquet_descriptors_length_hi	: 4;
#else
	unsigned bouquet_descriptors_length_hi	: 4;
	unsigned reserved_future_use2		: 4;
#endif
	unsigned bouquet_descriptors_length_lo	: 8;
} __attribute__ ((packed)) ; // 10 bytes

struct SI_section_EIT_header {
	unsigned table_id			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned section_syntax_indicator	: 1;
	unsigned reserved_future_use		: 1;
	unsigned reserved1			: 2;
	unsigned section_length_hi		: 4;
#else
	unsigned section_length_hi		: 4;
	unsigned reserved1			: 2;
	unsigned reserved_future_use		: 1;
	unsigned section_syntax_indicator	: 1;
#endif
	unsigned section_length_lo		: 8;
	unsigned service_id_hi			: 8;
	unsigned service_id_lo			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved2			: 2;
	unsigned version_number			: 5;
	unsigned current_next_indicator		: 1;
#else
	unsigned current_next_indicator		: 1;
	unsigned version_number			: 5;
	unsigned reserved2			: 2;
#endif
	unsigned section_number			: 8;
	unsigned last_section_number		: 8;
	unsigned transport_stream_id_hi		: 8;
	unsigned transport_stream_id_lo		: 8;
	unsigned original_network_id_hi		: 8;
	unsigned original_network_id_lo		: 8;
	unsigned segment_last_section_number	: 8;
	unsigned last_table_id			: 8;
} __attribute__ ((packed)) ; // 14 bytes

// Muss evtl. angepasst werden falls damit RST, TDT und TOT gelesen werden sollen
// ^^^
//   RST usw. haben section_syntax_indicator == 0, andere == 1 (obi)
struct SI_section_header {
	unsigned table_id			: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned section_syntax_indicator	: 1;
	unsigned reserved_future_use		: 1;
	unsigned reserved1			: 2;
	unsigned section_length_hi		: 4;
#else
	unsigned section_length_hi		: 4;
	unsigned reserved1			: 2;
	unsigned reserved_future_use		: 1;
	unsigned section_syntax_indicator	: 1;
#endif
	unsigned section_length_lo		: 8;
	unsigned table_id_extension_hi		: 8;
	unsigned table_id_extension_lo		: 8;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned reserved2			: 2;
	unsigned version_number			: 5;
	unsigned current_next_indicator		: 1;
#else
	unsigned current_next_indicator		: 1;
	unsigned version_number			: 5;
	unsigned reserved2			: 2;
#endif
	unsigned section_number			: 8;
	unsigned last_section_number		: 8;
} __attribute__ ((packed)) ; // 8 bytes

class SIsection
{
public:
	SIsection(void) { buffer = 0; bufferLength = 0;}

	// Kopierte den Puffer in eigenen Puffer
	SIsection(const char *buf, unsigned bufLength) {
		buffer = 0; bufferLength = 0;
		if ((buf) && (bufLength >= sizeof(struct SI_section_header))) {
			buffer = new char[bufLength];
			if (buffer) {
				bufferLength = bufLength;
				memcpy(buffer, buf, bufLength);
			}
		}
	}

	// Benutzt den uebergebenen Puffer (sollte mit new char[n] allokiert sein)
	SIsection(unsigned bufLength, char *buf) {
		buffer = 0; bufferLength = 0;
		if ((buf) && (bufLength >= sizeof(struct SI_section_header))) {
			buffer = buf;
			bufferLength = bufLength;
		}
	}

	// Konstruktor um eine (leere) SIsection mit den fuer Vergleiche
	// noetigen Inhalte (s. key) zu erstellen
	SIsection(const struct SI_section_header *header) {
		bufferLength = 0;
		buffer = new char[sizeof(struct SI_section_header)];
		if (buffer) {
			memcpy(buffer, header, sizeof(struct SI_section_header));
			bufferLength = sizeof(struct SI_section_header);
		}
	}

	// Std-Copy
	SIsection(const SIsection &s) {
		buffer = 0; bufferLength = 0;
		if (s.buffer) {
			buffer = new char[s.bufferLength];
			if (buffer) {
				bufferLength = s.bufferLength;
				memcpy(buffer, s.buffer, bufferLength);
			}
		}
	}

	// Destruktor
	virtual ~SIsection(void) {
		if (buffer) {
			delete[] buffer;
			buffer = 0;
			bufferLength = 0;
		}
	}

	unsigned char tableID(void) const {
		return buffer ? ((struct SI_section_header *)buffer)->table_id : (unsigned char) -1;
	}

	unsigned short tableIDextension(void) const {
		return buffer ? ((((struct SI_section_header *)buffer)->table_id_extension_hi << 8) | 
			((struct SI_section_header *)buffer)->table_id_extension_lo) : (unsigned short) -1;
	}

	unsigned char sectionNumber(void) const {
		return buffer ? ((struct SI_section_header *)buffer)->section_number : (unsigned char) -1;
	}

	unsigned char versionNumber(void) const {
		return buffer ? ((struct SI_section_header *)buffer)->version_number : (unsigned char) -1;
	}

	unsigned char currentNextIndicator(void) const {
		return buffer ? ((struct SI_section_header *)buffer)->current_next_indicator : (unsigned char) -1;
	}

	unsigned char lastSectionNumber(void) const {
		return buffer ? ((struct SI_section_header *)buffer)->last_section_number : (unsigned char) -1;
	}

	struct SI_section_header const *header(void) const {
		return (struct SI_section_header *)buffer;
	}

	static unsigned long long key(const struct SI_section_header *header) {
		// Der eindeutige Key einer SIsection besteht aus 1 Byte Table-ID,
		// 2 Byte Table-ID-extension, 1 Byte Section number
		// 1 Byte Version number und 1 Byte current_next_indicator
		if (!header)
			return (unsigned long long) -1;
		return 	(((unsigned long long)header->table_id) << 40) +
			(((unsigned long long)header->table_id_extension_hi) << 32) +
			(((unsigned long long)header->table_id_extension_lo) << 24) +
			(((unsigned long long)header->section_number) << 16) +
			(((unsigned long long)header->version_number) << 8) +
			(((unsigned long long)header->current_next_indicator));
	}

	unsigned long long key(void) const {
		return buffer ? key(header()) : (unsigned long long) -1;
	}

	// Der Operator zum sortieren
	bool operator < (const SIsection& s) const {
		return key() < s.key();
	}

	static void dumpSmallSectionHeader(const struct SI_section_header *header) {
		if (!header)
			return;
		printf("\ntable_id: 0x%02x ", header->table_id);
		printf("table_id_extension: 0x%02x%02x", header->table_id_extension_hi, header->table_id_extension_lo);
		printf("section_number: 0x%02x\n", header->section_number);
	}

	static void dumpSmallSectionHeader(const SIsection &s) {
		dumpSmallSectionHeader((struct SI_section_header *)s.buffer);
	}

	void dumpSmallSectionHeader(void) const {
		dumpSmallSectionHeader((struct SI_section_header *)buffer);
	}

	int saveBufferToFile(FILE *file) const {
		if (!file)
			return 1;
		return (fwrite(buffer, bufferLength, 1, file) != 1);
	}

	int saveBufferToFile(const char *filename) const {
		if (!filename)
			return 2;

		FILE *file = fopen(filename, "wb");

		if (file) {
			int rc = saveBufferToFile(file);
			fclose(file);
			return rc;
		}

		return 2;
	}

	static void dump1(const struct SI_section_header *header) {
		if (!header)
			return;
		printf("\ntable_id: 0x%02x\n", header->table_id);
		printf("section_syntax_indicator: 0x%02x\n", header->section_syntax_indicator);
		printf("section_length: %hu\n", (header->section_length_hi << 8) | header->section_length_lo);
	}

	static void dump2(const struct SI_section_header *header) {
		if (!header)
			return;
		printf("version_number: 0x%02x\n", header->version_number);
		printf("current_next_indicator: 0x%02x\n", header->current_next_indicator);
		printf("section_number: 0x%02x\n", header->section_number);
		printf("last_section_number: 0x%02x\n", header->last_section_number);
	}

	static void dump(const struct SI_section_header *header) {
		if (!header)
			return;
		dump1(header);
		printf("table_id_extension: 0x%02x%02x\n", header->table_id_extension_hi, header->table_id_extension_lo);
		dump2(header);
	}

	static void dump(const SIsection &s) {
		dump((struct SI_section_header *)s.buffer);
	}

	void dump(void) const {
		dump((struct SI_section_header *)buffer);
	}

protected:
	char *buffer;
	unsigned bufferLength;
};

// Fuer for_each
struct printSIsection : public std::unary_function<SIsection, void>
{
	void operator() (const SIsection &s) { s.dump();}
};

// Fuer for_each
struct printSmallSIsectionHeader : public std::unary_function<SIsection, void>
{
	void operator() (const SIsection &s) { s.dumpSmallSectionHeader();}
};

class SIsections : public std::set <SIsection, std::less<SIsection> >
{
public:
	// Liefert 0 falls kein Fehler
	// Algo:
	// (1) Segment lesen (wird zum ersten Segment deklariert)
	// (2) Falls Segmentnummer = letze Segmentnummer = 0 dann fertig, sonst
	// (3) alle Segment lesen bis erstes wieder kommt
	// (4) fehlende Segmente (s. last_section_number) versuchen zu lesen
	// Der Timeout gilt fuer jeden der 3 Abschnitte, d.h. maximal dauert
	// es 3 x timeout.
	// Mit readNext=0 werden nur aktuelle Sections gelesen (current_next_indicator = 1)
	int readSections(unsigned short pid, unsigned char filter, unsigned char mask, int readNext=0, unsigned timeoutInSeconds=10);
};

class SIsectionEIT : public SIsection
{
public:
	SIsectionEIT(const SIsection &s) : SIsection(s) {
		parsed = 0;
		parse();
	}

	// Std-Copy
	SIsectionEIT(const SIsectionEIT &s) : SIsection(s) {
		evts = s.evts;
		parsed = s.parsed;
	}

	unsigned short serviceID(void) const {
		return buffer ? ((((struct SI_section_EIT_header *)buffer)->service_id_hi << 8) |
				((struct SI_section_EIT_header *)buffer)->service_id_lo): (unsigned short) -1;
	}

	unsigned short originalNetworkID(void) const {
		return buffer ? ((((struct SI_section_EIT_header *)buffer)->original_network_id_hi << 8) |
				((struct SI_section_EIT_header *)buffer)->original_network_id_lo) : (unsigned short) -1;
	}

	struct SI_section_EIT_header const *header(void) const {
		return (struct SI_section_EIT_header *)buffer;
	}

	static void dump(const struct SI_section_EIT_header *header) {
		if (!header)
			return;
		SIsection::dump1((const struct SI_section_header *)header);
		printf("service_id: 0x%02x%02x\n", header->service_id_hi, header->service_id_lo);
		SIsection::dump2((const struct SI_section_header *)header);
		printf("transport_stream_id 0x%02x%02x\n", header->transport_stream_id_hi, header->transport_stream_id_lo);
		printf("original_network_id 0x%02x%02x\n", header->original_network_id_hi, header->original_network_id_lo);
		printf("segment_last_section_number: 0x%02x\n", header->segment_last_section_number);
		printf("last_table_id 0x%02x\n", header->last_table_id);
	}

	static void dump(const SIsectionEIT &s) {
		dump((struct SI_section_EIT_header *)s.buffer);
		for_each(s.evts.begin(), s.evts.end(), printSIevent());
	}

	void dump(void) const {
		dump((struct SI_section_EIT_header *)buffer);
		for_each(evts.begin(), evts.end(), printSIevent());
	}

	const SIevents &events(void) const {
		//if(!parsed)
		//	parse(); -> nicht const
		return evts;
	}

protected:
	SIevents evts;
	int parsed;
	void parse(void);
	void parseDescriptors(const char *desc, unsigned len, SIevent &e);
	void parseShortEventDescriptor(const char *buf, SIevent &e, unsigned maxlen);
	void parseExtendedEventDescriptor(const char *buf, SIevent &e, unsigned maxlen);
	void parseContentDescriptor(const char *buf, SIevent &e, unsigned maxlen);
	void parseComponentDescriptor(const char *buf, SIevent &e, unsigned maxlen);
	void parseParentalRatingDescriptor(const char *buf, SIevent &e, unsigned maxlen);
	void parseLinkageDescriptor(const char *buf, SIevent &e, unsigned maxlen);
};

// Fuer for_each
struct printSIsectionEIT : public std::unary_function<SIsectionEIT, void>
{
	void operator() (const SIsectionEIT &s) { s.dump();}
};

/*
// Fuer for_each
struct parseSIsectionEIT : public std::unary_function<SIsectionEIT, void>
{
	void operator() (const SIsectionEIT &s) { s.parse();}
};
*/

// Menge aller present/following EITs (actual TS)
class SIsectionsEIT : public std::set <SIsectionEIT, std::less<SIsectionEIT> >
{
public:
	int readSections(void) {
		SIsections sections;
		int rc=sections.readSections(0x12, 0x4e, 0xff);

		for (SIsections::iterator k=sections.begin(); k!=sections.end(); k++)
			insert(*k);

		return rc;
	}
};

// Menge aller schedule EITs (actual TS)
class SIsectionsEITschedule : public std::set <SIsectionEIT, std::less<SIsectionEIT> >
{
public:
	int readSections(void) {
		SIsections sections;
		int rc=sections.readSections(0x12, 0x50, 0xf0);

		for (SIsections::iterator k=sections.begin(); k!=sections.end(); k++)
			insert(*k);

		return rc;
	}
};

class SIsectionSDT : public SIsection
{
public:
	SIsectionSDT(const SIsection &s) : SIsection(s) {
		parsed = 0;
		parse();
	}

	// Std-Copy
	SIsectionSDT(const SIsectionSDT &s) : SIsection(s) {
		svs = s.svs;
		parsed = s.parsed;
	}

	unsigned short transportStreamID(void) const {
		return buffer ? ((((struct SI_section_SDT_header *)buffer)->transport_stream_id_hi << 8) |
				((struct SI_section_SDT_header *)buffer)->transport_stream_id_lo) : (unsigned short) -1;
	}

	struct SI_section_SDT_header const *header(void) const {
		return (struct SI_section_SDT_header *)buffer;
	}

	unsigned short originalNetworkID(void) const {
		return buffer ? ((((struct SI_section_SDT_header *)buffer)->original_network_id_hi << 8) |
				((struct SI_section_SDT_header *)buffer)->original_network_id_lo) : (unsigned short) -1;
	}

	static void dump(const struct SI_section_SDT_header *header) {
		if (!header)
			return;
		SIsection::dump1((const struct SI_section_header *)header);
		printf("transport_stream_id: 0x%02x%02x\n", header->transport_stream_id_hi, header->transport_stream_id_lo);
		SIsection::dump2((const struct SI_section_header *)header);
		printf("original_network_id: 0x%02x%02x\n", header->original_network_id_hi, header->original_network_id_lo);
	}

	static void dump(const SIsectionSDT &s) {
		dump((struct SI_section_SDT_header *)s.buffer);
		for_each(s.svs.begin(), s.svs.end(), printSIservice());
	}

	void dump(void) const {
		dump((struct SI_section_SDT_header *)buffer);
		for_each(svs.begin(), svs.end(), printSIservice());
	}

	const SIservices &services(void) const {
		//if(!parsed)
		//	parse(); -> nicht const
		return svs;
	}

private:
	SIservices svs;
	int parsed;
	void parse(void);
	void parseDescriptors(const char *desc, unsigned len, SIservice &s);
	void parseServiceDescriptor(const char *buf, SIservice &s);
	void parseNVODreferenceDescriptor(const char *buf, SIservice &s);
};

// Fuer for_each
struct printSIsectionSDT : public std::unary_function<SIsectionSDT, void>
{
	void operator() (const SIsectionSDT &s) { s.dump();}
};

// Menge aller SDTs (actual TS)
class SIsectionsSDT : public std::set <SIsectionSDT, std::less<SIsectionSDT> >
{
public:
	int readSections(void) {
		SIsections sections;
		int rc=sections.readSections(0x11, 0x42, 0xff);

		for (SIsections::iterator k=sections.begin(); k!=sections.end(); k++)
			insert(*k);

		return rc;
	}
};

class SIsectionBAT : public SIsection
{
public:
	SIsectionBAT(const SIsection &s) : SIsection(s) {}

	unsigned short bouquetID(void) const {
		return buffer ? ((((struct SI_section_BAT_header *)buffer)->bouquet_id_hi << 8) |
				((struct SI_section_BAT_header *)buffer)->bouquet_id_lo) : (unsigned short) -1;
	}

	struct SI_section_BAT_header const *header(void) const {
		return (struct SI_section_BAT_header *)buffer;
	}

	static void dump(const struct SI_section_BAT_header *header) {
		if (!header)
			return;
		SIsection::dump1((const struct SI_section_header *)header);
		printf("bouquet_id: 0x%02x%02x\n", header->bouquet_id_hi, header->bouquet_id_lo);
		SIsection::dump2((const struct SI_section_header *)header);
		printf("bouquet_descriptors_length %hu\n",
			(header->bouquet_descriptors_length_hi << 8) | header->bouquet_descriptors_length_lo);
	}

	static void dump(const SIsectionBAT &s) {
		dump((struct SI_section_BAT_header *)s.buffer);
	}

	void dump(void) const {
		dump((struct SI_section_BAT_header *)buffer);
	}
};

// Fuer for_each
struct printSIsectionBAT : public std::unary_function<SIsectionBAT, void>
{
	void operator() (const SIsectionBAT &s) { s.dump();}
};

// Menge aller BATs
class SIsectionsBAT : public std::set <SIsectionBAT, std::less<SIsectionBAT> >
{
public:
	int readSections(void) {
		SIsections sections;
		int rc=sections.readSections(0x11, 0x4a, 0xff);

		for (SIsections::iterator k=sections.begin(); k!=sections.end(); k++)
			insert(*k);

		return rc;
	}
};

class SIsectionNIT : public SIsection
{
public:
	SIsectionNIT(const SIsection &s) : SIsection(s) {}

	unsigned short networkID(void) const {
		return buffer ? ((((struct SI_section_NIT_header *)buffer)->network_id_hi << 8) |
				((struct SI_section_NIT_header *)buffer)->network_id_lo) : (unsigned short) -1;
	}

	struct SI_section_NIT_header const *header(void) const {
		return (struct SI_section_NIT_header *)buffer;
	}

	static void dump(const struct SI_section_NIT_header *header) {
		if (!header)
			return;
		SIsection::dump1((const struct SI_section_header *)header);
		printf("network_id: 0x%02x%02x\n", header->network_id_hi, header->network_id_lo);
		SIsection::dump2((const struct SI_section_header *)header);
		printf("network_descriptors_length %hu\n",
				(header->network_descriptors_length_hi << 8) | header->network_descriptors_length_lo);
	}

	static void dump(const SIsectionNIT &s) {
		dump((struct SI_section_NIT_header *)s.buffer);
	}

	void dump(void) const {
		dump((struct SI_section_NIT_header *)buffer);
	}
};

// Fuer for_each
struct printSIsectionNIT : public std::unary_function<SIsectionNIT, void>
{
	void operator() (const SIsectionNIT &s) { s.dump();}
};

// Menge aller NITs (actual network)
class SIsectionsNIT : public std::set <SIsectionNIT, std::less<SIsectionNIT> >
{
public:
	int readSections(void) {
		SIsections sections;
		int rc=sections.readSections(0x10, 0x40, 0xff);

		for (SIsections::iterator k=sections.begin(); k!=sections.end(); k++)
			insert(*k);

		return rc;
	}
};

#endif // SISECTIONS_HPP
