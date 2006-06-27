SUBDIRS = driver

INCLUDES = \
	-I$(top_srcdir)/lib \
	@CONNECTION_CFLAGS@ \
	@EVENTSERVER_CFLAGS@ \
	@TUXBOX_CFLAGS@ \
	@ZAPIT_CFLAGS@

bin_PROGRAMS = controld

controld_LDADD = \
	driver/libcontrold_driver.a \
	$(top_builddir)/lib/timerdclient/libtimerdclient.la \
	$(top_builddir)/lib/irsend/libirsend.la \
	@CONFIGFILE_LIBS@ \
	@CONNECTION_LIBS@ \
	@EVENTSERVER_LIBS@ \
	@TUXBOX_LIBS@ \
	@ZAPIT_LIBS@ \
	@LIRCDCLIENT_LIBS@ \
	-lpthread

controld_SOURCES = controld.cpp eventwatchdog.cpp

install-data-local:
	install -d $(CONFIGDIR)
	install -m 644 $(srcdir)/scart.conf $(CONFIGDIR)

if MAINTAINER_MODE
# Metamorphosis can be downloaded from http://www.ovidius.com
# Of course, configure should locate mmx ;-)
MMX = /usr/local/mm/bin/mmx

avs_settings.cpp: io-config.xml do-inits.mm
	$(MMX) -M do-inits.mm -F xml -S $< -F txt -T $@

scart.conf: io-config.xml do-scart-conf.mm
	$(MMX) -M do-scart-conf.mm -F xml -S $< -F txt -T $@

endif
