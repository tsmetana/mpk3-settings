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

#EXTRA_DIST = $(doc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done
