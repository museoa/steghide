man_MANS = steghide.1
EXTRA_DIST = steghide.1 doxygen.cfg
MAINTAINERCLEANFILES = Makefile.in
if HAVE_DOXYGEN
noinst_DATA = doxygen
CLEANFILES = doxygen
doxygen:
	doxygen doxygen.cfg
endif
