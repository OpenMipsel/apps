INCLUDES = \
	-I$(top_srcdir)/include

bin_PROGRAMS = enigma

enigma_SOURCES = \
	enigma.cpp enigma_dyn.cpp enigma_event.cpp enigma_lcd.cpp \
	enigma_main.cpp enigma_plugins.cpp enigma_setup.cpp enigma_xmlrpc.cpp scan.cpp \
	setupnetwork.cpp setup_audio.cpp setupvideo.cpp showbnversion.cpp sselect.cpp streaminfo.cpp \
	enigma_mainmenu.cpp satconfig.cpp epgwindow.cpp setup_language.cpp enigma_info.cpp \
	enigma_scan.cpp setupskin.cpp setup_osd.cpp timer.cpp setup_lcd.cpp enigma_vcr.cpp \
	setup_rc.cpp channelinfo.cpp setup_harddisk.cpp enigma_ci.cpp upgrade.cpp satfind.cpp \
	rotorconfig.cpp setup_rfmod.cpp wizard_language.cpp wizard_scan.cpp

enigma_TEST = \
	-Wl,--whole-archive \
	$(top_builddir)/lib/driver/libenigma_driver.a \
	$(top_builddir)/lib/base/libenigma_base.a \
	$(top_builddir)/lib/codecs/libenigma_codecs.a \
	$(top_builddir)/lib/dvb/libenigma_dvb.a \
	$(top_builddir)/lib/dvb/lowlevel/libenigma_dvb_lowlevel.a \
	$(top_builddir)/lib/gui/libenigma_gui.a \
	$(top_builddir)/lib/gdi/libenigma_gdi.a \
	$(top_builddir)/lib/socket/libenigma_socket.a \
	$(top_builddir)/lib/system/libenigma_system.a \
	-Wl,--no-whole-archive -Wl,--export-dynamic

enigma_SUCKS = \
	$(top_builddir)/lib/driver/libenigma_driver.a \
	$(top_builddir)/lib/base/libenigma_base.a \
	$(top_builddir)/lib/codecs/libenigma_codecs.a \
	$(top_builddir)/lib/dvb/libenigma_dvb.a \
	$(top_builddir)/lib/dvb/lowlevel/libenigma_dvb_lowlevel.a \
	$(top_builddir)/lib/gui/libenigma_gui.a \
	$(top_builddir)/lib/gdi/libenigma_gdi.a \
	$(top_builddir)/lib/socket/libenigma_socket.a \
	$(top_builddir)/lib/system/libenigma_system.a

enigma_LDADD = \
	@SIGC_LIBS@ \
	@XMLTREE_LIBS@ \
	@FREETYPE_LIBS@ \
	@PNG_LIBS@ \
	-ldl -lpthread -lcrypt -lresolv -lmd5sum -lid3tag -lmad

#\
#	/dbox2/cdkroot/lib/libmpatrolmt.a -lbfd -liberty

enigma$(EXEEXT): $(enigma_OBJECTS) $(enigma_DEPENDENCIES) $(enigma_SUCKS)
	@rm -f enigma$(EXEEXT)
	$(CXXLINK) $(enigma_LDFLAGS) $(enigma_OBJECTS) $(enigma_TEST) $(enigma_LDADD) $(LIBS)
