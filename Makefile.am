desktopdir = $(datadir)/applications
desktop_DATA = pkg/io.github.tsmetana.mpk3-settings.desktop

metainfodir = $(datadir)/metainfo
metainfo_DATA = pkg/io.github.tsmetana.mpk3-settings.metainfo.xml

iconsdir = $(datadir)/icons/hicolor/scalable/apps
icons_DATA = pkg/io.github.tsmetana.mpk3-settings.svg

SUBDIRS = src

#docdir = ${prefix}/doc/exp
#doc_DATA = \
#	README\
#	COPYING\
#	AUTHORS\
#	ChangeLog\
#	INSTALL\
#	NEWS\
#	TODO

EXTRA_DIST = \
	$(desktop_DATA) \
	$(metainfo_DATA) \
	$(icons_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done
