INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"
bin_PROGRAMS = steghide
EXTRA_DIST = \
arg.h        bmpfile.h     cvrstgobject.h  jpegentropycoded.h  jpeghufftable.h  jpegscanhdr.h    plnfile.h \
arguments.h  bufmanag.h    error.h         jpegfile.h          jpegjfifapp0.h   jpegunusedseg.h  stegano.h \
aufile.h     crypto.h      hash.h          jpegframe.h         jpegrestart.h    main.h           support.h \
binaryio.h   cvrstgfile.h  jpegbase.h      jpegframehdr.h      jpegscan.h       msg.h            wavfile.h
steghide_SOURCES = \
arg.cc        binaryio.cc  crypto.cc      hash.cc              jpegfile.cc      jpeghufftable.cc  jpegscan.cc       main.cc     stegano.cc \
arguments.cc  bmpfile.cc   cvrstgfile.cc  jpegbase.cc          jpegframe.cc     jpegjfifapp0.cc   jpegscanhdr.cc    msg.cc      support.cc \
aufile.cc     bufmanag.cc  error.cc       jpegentropycoded.cc  jpegframehdr.cc  jpegrestart.cc    jpegunusedseg.cc  plnfile.cc  wavfile.cc
localedir = $(datadir)/locale
LIBTOOL = $(SHELL) libtool
