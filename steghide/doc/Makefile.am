man_MANS = steghide.1
EXTRA_DIST = steghide.1 doxygen.cfg
# FIXME
# doxygen is not deleted by make maintainer-clean nor by make clean
# how can automake be told to use rm -rf, not rm -f ?
MAINTAINERCLEANFILES = doxygen Makefile.in
if HAVE_DOXYGEN
noinst_DATA = doxygen
CLEANFILES = doxygen
doxygen:
	doxygen doxygen.cfg
endif
