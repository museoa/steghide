man_MANS = steghide.1
EXTRA_DIST = steghide.1 doxygen.cfg
MAINTAINERCLEANFILES = Makefile.in

if HAVE_DOXYGEN
noinst_DATA = doxygen

clean-local:
	rm -rf doxygen

maintainer-clean-local:
	rm -rf doxygen

doxygen:
	doxygen doxygen.cfg
endif
