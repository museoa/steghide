INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"
bin_PROGRAMS = steghide
EXTRA_DIST = arg.h arguments.h aufile.h binaryio.h bmpfile.h bufmanag.h \
	crypto.h cvrstgfile.h cvrstgobject.h error.h hash.h jpegbase.h \
	jpegcomment.h jpegentropycoded.h jpegfile.h jpegframe.h jpegframehdr.h jpeghufftable.h \
	jpegjfifapp0.h jpegquanttable.h jpegscan.h jpegscanhdr.h main.h msg.h \
	plnfile.h stegano.h support.h wavfile.h
steghide_SOURCES = arg.cc arguments.cc aufile.cc binaryio.cc bmpfile.cc \
	bufmanag.cc crypto.cc cvrstgfile.cc error.cc hash.cc jpegbase.cc \
	jpegcomment.cc jpegentropycoded.cc jpegfile.cc jpegframe.cc jpegframehdr.cc jpeghufftable.cc \
	jpegjfifapp0.cc jpegquanttable.cc jpegscan.cc jpegscanhdr.cc main.cc \
	msg.cc plnfile.cc stegano.cc support.cc wavfile.cc
localedir = $(datadir)/locale
LIBTOOL = $(SHELL) libtool
