AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
SUBDIRS = intl po doc src tests
docdir = $(prefix)/doc/$(PACKAGE)
EXTRA_DIST = ABOUT-NLS BUGS CREDITS HISTORY depcomp steghide.doxygen
doc_DATA = ABOUT-NLS BUGS COPYING CREDITS HISTORY INSTALL README TODO
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.sub configure depcomp install-sh missing mkinstalldirs

all-local:
	@echo "********************"
	@echo "*  \"make\" finished. If there were no errors please run \"make check\""
	@echo "*  now to ensure that steghide works on your system."
	@echo "********************"
