INCLUDES = \
	-I$(top_srcdir)/lib \
	@EVENTSERVER_CFLAGS@ \
	-I@DRIVER@

bin_PROGRAMS = bat epg epgMini nit sectionsd sdt

bat_SOURCES = bat.cpp dmxapi.cpp SIsections.cpp SIevents.cpp SIutils.cpp
epg_SOURCES = epg.cpp dmxapi.cpp SIsections.cpp SIevents.cpp SIutils.cpp
epgMini_SOURCES = epgMini.cpp dmxapi.cpp SIsections.cpp SIevents.cpp SIutils.cpp
nit_SOURCES = nit.cpp dmxapi.cpp SIsections.cpp SIevents.cpp SIutils.cpp
sectionsd_SOURCES = sectionsd.cpp dmxapi.cpp dmx.cpp SIsections.cpp SIevents.cpp SIutils.cpp
sdt_SOURCES = sdt.cpp dmxapi.cpp SIsections.cpp SIevents.cpp SIutils.cpp

sectionsd_LDADD = \
	@EVENTSERVER_LIBS@ \
	-lpthread

