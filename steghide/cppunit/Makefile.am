if HAVE_CPPUNIT
AM_CPPFLAGS = -I../src
TESTS = testall
check_PROGRAMS = testall
noinst_HEADERS = BitStringTest.h PermutationTest.h MHashTest.h
# testall_DEPENDENCIES = all in ../src
# FIXME - how can a cross-directory dependency be coded with automake ?
testall_SOURCES = main.cc BitStringTest.cc PermutationTest.cc MHashTest.cc
testall_LDADD = \
../src/Arg.o ../src/Arguments.o \
../src/AssertionFailed.o ../src/AuFile.o \
../src/AuSampleValue.o ../src/AugmentingPathHeuristic.o \
../src/BinaryIO.o ../src/BitString.o \
../src/BmpFile.o ../src/BmpPaletteSampleValue.o \
../src/BmpRGBSampleValue.o ../src/BmpSampleValue.o \
../src/ConstructionHeuristic.o \
../src/CvrStgFile.o ../src/Edge.o \
../src/EdgeIterator.o ../src/EmbData.o \
../src/Embedder.o ../src/EncryptionAlgorithm.o \
../src/EncryptionMode.o ../src/Extractor.o \
../src/Graph.o ../src/GraphAccess.o \
../src/JpegFile.o ../src/JpegSampleValue.o \
../src/MCrypt.o ../src/MHash.o \
../src/MHashKeyGen.o ../src/Matching.o \
../src/Permutation.o ../src/RGBTriple.o \
../src/RandomSource.o ../src/SampleValue.o \
../src/SampleValueOppositeNeighbourhood.o ../src/Session.o \
../src/SteghideError.o ../src/Terminal.o \
../src/Vertex.o ../src/VertexContent.o \
../src/WavChunk.o ../src/WavChunkHeader.o \
../src/WavChunkUnused.o ../src/WavFile.o \
../src/WavFormatChunk.o ../src/WavPCMSampleValue.o \
../src/error.o ../src/jpegbase.o \
../src/jpegentropycoded.o ../src/jpegframe.o \
../src/jpegframehdr.o ../src/jpeghufftable.o \
../src/jpegjfifapp0.o ../src/jpegrestart.o \
../src/jpegscan.o ../src/jpegscanhdr.o \
../src/jpegunusedseg.o ../src/msg.o \
-lcppunit
endif
MAINTAINERCLEANFILES = Makefile.in
