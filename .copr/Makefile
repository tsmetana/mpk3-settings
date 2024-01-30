# This Makefile is intended to be called from outside of the repository:
# make -f <cloned_repodir>/.copr/Makefile srpm outdir="<outdir>" spec="<spec_path>"
#
# See: https://docs.pagure.org/copr.copr/user_documentation.html#make-srpm
#
outdir?="."
spec?="."

srpm: $(spec)/mpk3-settings.spec
	cd $(spec) && \
	make srpm
	mv $(spec)/*.src.rpm $(outdir)

$(spec)/mpk3-settings.spec: $(spec)/mpk3-settings.spec.in
	-dnf -y install autoconf automake gtk3-devel rtmidi-devel gcc
	cd $(spec) && \
	autoreconf -ivf && \
	./configure
