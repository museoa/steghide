AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
SUBDIRS = m4 intl po doc src tests
docdir = $(prefix)/doc/$(PACKAGE)
EXTRA_DIST = config.rpath mkinstalldirs  ABOUT-NLS BUGS CREDITS HISTORY depcomp steghide.doxygen
doc_DATA = ABOUT-NLS BUGS COPYING CREDITS HISTORY INSTALL README TODO
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.sub configure depcomp install-sh missing mkinstalldirs

ACLOCAL_AMFLAGS = -I m4
