SUBDIRS = data
TESTS = unittests systemtests.pl
AM_CPPFLAGS = -I../src
check_PROGRAMS = unittests
EXTRA_DIST = systemtests.pl STSupport.pm st_fileformats.pl
noinst_HEADERS = \
AuFileTest.h BitStringTest.h BmpWinFileTest.h BmpOS2FileTest.h CvrStgFileTest.h \
ColorPaletteTest.h DummyFile.h DummySampleValue.h MatchingTest.h MHashPPTest.h Test.h TestCategory.h \
GraphTest.h TestCategoryCaller.h TestSuite.h UnitTest.h utcommon.h \
BmpPaletteSampleValueTest.h SampleValueOppositeNeighbourhoodTest.h SampleValueTest.h SelectorTest.h \
VertexContentTest.h WavFileTest.h BmpRGBSampleValueTest.h AugmentingPathHeuristicTest.h
unittests_SOURCES = \
AuFileTest.cc BitStringTest.cc BmpWinFileTest.cc BmpOS2FileTest.cc CvrStgFileTest.cc \
ColorPaletteTest.cc DummyFile.cc DummySampleValue.cc MatchingTest.cc GraphTest.cc MHashPPTest.cc TestSuite.cc UnitTest.cc \
SampleValueTest.cc BmpPaletteSampleValueTest.cc BmpRGBSampleValueTest.cc SelectorTest.cc VertexContentTest.cc WavFileTest.cc \
SampleValueOppositeNeighbourhoodTest.cc AugmentingPathHeuristicTest.cc main.cc
unittests_LDADD = \
../src/Arg.o ../src/Arguments.o ../src/AssertionFailed.o ../src/AuFile.o \
../src/AugmentingPathHeuristic.o \
../src/BinaryIO.o ../src/BitString.o ../src/BmpFile.o \
../src/BmpPaletteSampleValue.o ../src/BmpRGBSampleValue.o \
../src/BmpSampleValue.o ../src/ColorPalette.o ../src/ConstructionHeuristic.o \
../src/CvrStgFile.o ../src/Edge.o ../src/EdgeIterator.o ../src/EmbData.o \
../src/Embedder.o ../src/EncryptionAlgorithm.o ../src/EncryptionMode.o \
../src/Extractor.o ../src/Graph.o ../src/JpegFile.o ../src/JpegSampleValue.o \
../src/MCryptPP.o ../src/MHashKeyGen.o ../src/MHashPP.o ../src/Matching.o \
../src/ProgressOutput.o ../src/PseudoRandomSource.o ../src/RGBTriple.o \
../src/MatchingAlgorithm.o ../src/RandomSource.o ../src/SampleValue.o \
../src/SampleValueOppositeNeighbourhood.o ../src/Selector.o ../src/Session.o \
../src/SteghideError.o ../src/Terminal.o ../src/Vertex.o \
../src/VertexContent.o ../src/WavChunk.o ../src/WavChunkHeader.o \
../src/WavChunkUnused.o ../src/WavFile.o ../src/WavFormatChunk.o \
../src/WavPCMSampleValue.o ../src/error.o ../src/msg.o
MAINTAINERCLEANFILES = Makefile.in
