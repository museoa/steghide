AM_CPPFLAGS = -I../src
TESTS = testall
check_PROGRAMS = testall
noinst_HEADERS = BitStringTest.h
# testall_DEPENDENCIES = all in ../src
# FIXME - how can a cross-directory dependency be coded with automake ?
testall_SOURCES = main.cc BitStringTest.cc
testall_LDADD = \
../src/AssertionFailed.o ../src/SteghideError.o ../src/arg.o ../src/arguments.o \
../src/aufile.o ../src/augheur.o ../src/ausamplevalue.o ../src/binaryio.o \
../src/bitstring.o ../src/bmpfile.o ../src/bmpsamplevalue.o ../src/colorpalette.o \
../src/constrheur.o ../src/cvrstgfile.o ../src/edge.o ../src/edgeiterator.o \
../src/embdata.o ../src/embedder.o ../src/encalgo.o ../src/encmode.o \
../src/error.o ../src/extractor.o ../src/graph.o ../src/graphaccess.o \
../src/jpegbase.o ../src/jpegentropycoded.o ../src/jpegfile.o ../src/jpegframe.o \
../src/jpegframehdr.o ../src/jpeghufftable.o ../src/jpegjfifapp0.o ../src/jpegrestart.o \
../src/jpegsamplevalue.o ../src/jpegscan.o ../src/jpegscanhdr.o ../src/jpegunusedseg.o \
../src/matching.o ../src/mcryptpp.o ../src/mhashkeygenpp.o \
../src/mhashpp.o ../src/msg.o ../src/permutation.o ../src/randomsource.o \
../src/rgbtriple.o ../src/samplevalue.o ../src/session.o ../src/svalueoppneigh.o \
../src/terminal.o ../src/vertex.o ../src/vertexcontent.o ../src/wavchunk.o \
../src/wavchunkheader.o ../src/wavchunkunused.o ../src/wavfile.o ../src/wavformatchunk.o \
../src/wavsamplevalue.o \
-lcppunit

