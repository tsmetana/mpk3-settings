dnl Process this file with auto(re)conf to produce a configure script.
AC_INIT([mpk3-settings],[0.1],[https://github.com/tsmetana/mpk3-settings/issues],[])
AM_INIT_AUTOMAKE([foreign])
AC_PREREQ([2.69])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
CFLAGS=""
AC_SUBST([CFLAGS])
AC_PROG_CC

PKG_CHECK_MODULES([GTK], [gtk+-3.0], [], [])
PKG_CHECK_MODULES([RTMIDI5], [rtmidi >= 5.0.0], [dnl
		AC_DEFINE([HAVE_RTMIDI5], [1], [Use RTMIDI version 5])
		AC_SUBST([RTMIDI_CFLAGS], [$RTMIDI5_CFLAGS])
		AC_SUBST([RTMIDI_LIBS],   [$RTMIDI5_LIBS])
	], [dnl
		PKG_CHECK_MODULES([RTMIDI4], [rtmidi >= 4.0.0], [dnl
			AC_DEFINE([HAVE_RTMIDI4], [1], [Use RTMIDI version 4])
			AC_SUBST([RTMIDI_CFLAGS], [$RTMIDI4_CFLAGS])
			AC_SUBST([RTMIDI_LIBS],   [$RTMIDI4_LIBS])
		], [])
	]
)

AC_SUBST([BUILD_DATE], [$(LC_ALL=C date +"%a %b %d %Y")])

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
