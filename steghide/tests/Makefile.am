SUBDIRS = data
TESTS = unittests systemtests.pl
AM_CPPFLAGS = -I../src
check_PROGRAMS = unittests
EXTRA_DIST = systemtests.pl STSupport.pm st_fileformats.pl
noinst_HEADERS = \
AudioSampleValueTest.h AuFileTest.h AugmentingPathHeuristicTest.h \
BitStringTest.h BmpOS2FileTest.h BmpPaletteSampleValueTest.h \
BmpRGBSampleValueTest.h BmpWinFileTest.h ColorPaletteTest.h \
ConstructionHeuristicTest.h CvrStgFileTest.h DummyFile.h DummySampleValue.h \
EmbDataTest.h GraphTest.h JpegFileTest.h JpegSampleValueTest.h MatchingTest.h \
MCryptPPTest.h MHashKeyGenTest.h MHashPPTest.h \
SampleValueOppositeNeighbourhoodTest.h SampleValueTest.h SelectorTest.h \
Test.h TestCategory.h TestCategoryCaller.h TestSuite.h UnitTest.h utcommon.h \
VertexContentTest.h WavFileTest.h WavPCMSampleValueTest.h
unittests_SOURCES = \
AuFileTest.cc AudioSampleValueTest.cc AugmentingPathHeuristicTest.cc \
BitStringTest.cc BmpOS2FileTest.cc BmpPaletteSampleValueTest.cc \
BmpRGBSampleValueTest.cc BmpWinFileTest.cc ColorPaletteTest.cc \
ConstructionHeuristicTest.cc CvrStgFileTest.cc DummyFile.cc \
DummySampleValue.cc EdgeIteratorTest.cc EmbDataTest.cc GraphTest.cc \
JpegFileTest.cc JpegSampleValueTest.cc MCryptPPTest.cc MHashKeyGenTest.cc \
MHashPPTest.cc MatchingTest.cc SampleValueOppositeNeighbourhoodTest.cc \
SampleValueTest.cc SelectorTest.cc TestSuite.cc UnitTest.cc \
VertexContentTest.cc WavFileTest.cc WavPCMSampleValueTest.cc main.cc
unittests_LDADD = \
../src/Arg.o ../src/Arguments.o ../src/AssertionFailed.o ../src/AuFile.o \
../src/AugmentingPathHeuristic.o ../src/AuSampleValues.o \
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
