desktopdir = $(datadir)/applications
desktop_DATA = pkg/io.github.tsmetana.mpk3-settings.desktop

metainfodir = $(datadir)/metainfo
metainfo_DATA = pkg/io.github.tsmetana.mpk3-settings.metainfo.xml

iconsdir = $(datadir)/icons/hicolor/scalable/apps
icons_DATA = pkg/io.github.tsmetana.mpk3-settings.svg

SUBDIRS = src

EXTRA_DIST = \
	README.md \
	LICENSE \
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

RPMDIR = ${PWD}
RPMRESULTDIR = rpmbuild
srpm: dist-bzip2
	rpmbuild \
		--clean \
		--define "_sourcedir $(RPMDIR)" \
		--define "_specdir $(RPMDIR)" \
		--define "_builddir $(RPMDIR)" \
		--define "_srcrpmdir $(RPMDIR)" \
		--define "_rpmdir $(RPMDIR)/$(RPMRESULTDIR)" \
		-bs $(PACKAGE).spec
