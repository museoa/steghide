TESTS = unittests
AM_CPPFLAGS = -I../src
check_PROGRAMS = unittests
noinst_HEADERS = \
BitStringTest.h BmpWinFileTest.h BmpOS2FileTest.h CvrStgFileTest.h \
MHashPPTest.h Test.h TestCategory.h \
GraphTest.h TestCategoryCaller.h TestSuite.h UnitTest.h utcommon.h \
SampleValueOppositeNeighbourhoodTest.h SampleValueTest.h SelectorTest.h \
VertexContentTest.h WavFileTest.h BmpRGBSampleValueTest.h
unittests_SOURCES = \
BitStringTest.cc BmpWinFileTest.cc BmpOS2FileTest.cc CvrStgFileTest.cc \
GraphTest.cc MHashPPTest.cc TestSuite.cc UnitTest.cc \
SampleValueTest.cc BmpRGBSampleValueTest.cc SelectorTest.cc VertexContentTest.cc WavFileTest.cc \
SampleValueOppositeNeighbourhoodTest.cc main.cc
unittests_LDADD = \
../src/Arg.o ../src/Arguments.o ../src/AssertionFailed.o ../src/AuFile.o \
../src/AuSampleValue.o ../src/AugmentingPathHeuristic.o ../src/BinaryIO.o \
../src/BitString.o ../src/BmpFile.o ../src/BmpPaletteSampleValue.o \
../src/BmpRGBSampleValue.o ../src/BmpSampleValue.o ../src/ColorPalette.o \
../src/ConstructionHeuristic.o ../src/CvrStgFile.o ../src/Edge.o \
../src/EdgeIterator.o ../src/EmbData.o ../src/Embedder.o \
../src/EncryptionAlgorithm.o ../src/EncryptionMode.o ../src/Extractor.o \
../src/Graph.o ../src/JpegFile.o ../src/JpegSampleValue.o ../src/MCryptPP.o \
../src/MHashKeyGen.o ../src/MHashPP.o ../src/Matching.o \
../src/PseudoRandomSource.o ../src/RGBTriple.o ../src/RandomSource.o \
../src/SampleValue.o ../src/SampleValueOppositeNeighbourhood.o \
../src/Selector.o ../src/Session.o ../src/SteghideError.o ../src/Terminal.o \
../src/ProgressOutput.o ../src/Vertex.o ../src/VertexContent.o ../src/WavChunk.o \
../src/WavChunkHeader.o ../src/WavChunkUnused.o ../src/WavFile.o \
../src/WavFormatChunk.o ../src/WavPCMSampleValue.o ../src/error.o \
../src/jpegbase.o ../src/jpegentropycoded.o ../src/jpegframe.o \
../src/jpegframehdr.o ../src/jpeghufftable.o ../src/jpegjfifapp0.o \
../src/jpegrestart.o ../src/jpegscan.o ../src/jpegscanhdr.o \
../src/jpegunusedseg.o ../src/msg.o
MAINTAINERCLEANFILES = Makefile.in
