AC_INIT(tuxbox-plugins,0.0.1)
AM_INIT_AUTOMAKE(tuxbox-plugins,0.0.1)

TUXBOX_APPS
TUXBOX_APPS_DIRECTORY

AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL

TUXBOX_APPS_DVB
TUXBOX_APPS_DRIVER

TUXBOX_APPS_LIB_CONFIG(FREETYPE,freetype-config)
TUXBOX_APPS_LIB_PKGCONFIG(PNG,libpng)
TUXBOX_APPS_LIB_PKGCONFIG(SIGC,sigc++-1.2)
TUXBOX_APPS_LIB_PKGCONFIG(XMLTREE,tuxbox-xmltree)

CPPFLAGS="$CPPFLAGS $FREETYPE_CFLAGS $SIGC_CFLAGS $XMLTREE_CFLAGS -fno-exceptions -fno-rtti"

AC_OUTPUT([
Makefile
include/Makefile
fx2/Makefile
fx2/lemm/Makefile
fx2/master/Makefile
fx2/pac/Makefile
fx2/tank/Makefile
fx2/yahtzee/Makefile
fx2/bouquet/Makefile
fx2/lib/Makefile
fx2/mines/Makefile
fx2/snake/Makefile
fx2/tetris/Makefile
tuxtxt/Makefile
tuxtxt/font/Makefile
enigma/Makefile
enigma/demo/Makefile
tuxbox-plugins.pc
])

