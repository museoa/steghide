INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"
bin_PROGRAMS = steghide
EXTRA_DIST = \
arg.h        bmpfile.h     cvrstgobject.h  jpegcomment.h       jpegframehdr.h   jpegscan.h      msg.h      wavfile.h \
arguments.h  bufmanag.h    error.h         jpegentropycoded.h  jpeghufftable.h  jpegscanhdr.h   plnfile.h \
aufile.h     crypto.h      hash.h          jpegfile.h          jpegjfifapp0.h   jpegunsupseg.h  stegano.h \
binaryio.h   cvrstgfile.h  jpegbase.h      jpegframe.h         jpegrestart.h    main.h          support.h
steghide_SOURCES = \
arg.cc        bmpfile.cc     error.cc        jpegentropycoded.cc  jpeghufftable.cc  jpegscanhdr.cc   plnfile.cc \
arguments.cc  bufmanag.cc    hash.cc         jpegfile.cc          jpegjfifapp0.cc   jpegunsupseg.cc  stegano.cc \
aufile.cc     crypto.cc      jpegbase.cc     jpegframe.cc         jpegrestart.cc    main.cc          support.cc \
binaryio.cc   cvrstgfile.cc  jpegcomment.cc  jpegframehdr.cc      jpegscan.cc       msg.cc           wavfile.cc
localedir = $(datadir)/locale
LIBTOOL = $(SHELL) libtool
