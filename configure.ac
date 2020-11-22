dnl Process this file with autoconf to produce a configure script.
AC_INIT(mpk3-settings,0.1,,)
AM_INIT_AUTOMAKE([foreign])
AC_PREREQ(2.69)
AC_CONFIG_HEADERS(config.h)
AC_GNU_SOURCE
AM_INIT_AUTOMAKE
CFLAGS=""
AC_SUBST(CFLAGS)
AC_PROG_CC
AC_HEADER_STDC

PKG_CHECK_MODULES(GTK, gtk+-3.0,,)
PKG_CHECK_MODULES(RTMIDI, rtmidi >= 4.0.0,,)

AC_SUBST([BUILD_DATE], $(LC_ALL=C date +"%a %b %d %Y"))

AC_OUTPUT([
Makefile
src/Makefile
])
