AM_CPPFLAGS = -I../intl -DLOCALEDIR=\"$(localedir)\"
bin_PROGRAMS = steghide
noinst_HEADERS = \
Arg.h Arguments.h AssertionFailed.h AuData.h AuFile.h AuSampleValues.h \
AudioData.h AudioSampleValue.h AugmentingPathHeuristic.h BinaryIO.h \
BitString.h BmpFile.h BmpPaletteSampleValue.h BmpRGBSampleValue.h \
BmpSampleValue.h ColorPalette.h ConstructionHeuristic.h CvrStgFile.h \
CvrStgObject.h Edge.h EdgeIterator.h EmbData.h Embedder.h \
EncryptionAlgorithm.h EncryptionMode.h Extractor.h Graph.h GraphAccess.h \
JpegFile.h JpegSampleValue.h MCryptPP.h MHashKeyGen.h MHashPP.h Matching.h \
ProgressOutput.h PseudoRandomSource.h RGBTriple.h RandomSource.h \
SampleOccurence.h SampleValue.h SampleValueOppositeNeighbourhood.h Selector.h \
Session.h SteghideError.h Terminal.h Vertex.h VertexContent.h WavChunk.h \
WavChunkHeader.h WavChunkUnused.h WavFile.h WavFormatChunk.h \
WavPCMSampleValue.h common.h error.h jpegbase.h jpegentropycoded.h jpegframe.h \
jpegframehdr.h jpeghufftable.h jpegjfifapp0.h jpegrestart.h jpegscan.h \
jpegscanhdr.h jpegunusedseg.h msg.h wrapper_hash_map.h wrapper_hash_set.h
steghide_SOURCES = \
Arg.cc Arguments.cc AssertionFailed.cc AuFile.cc \
AugmentingPathHeuristic.cc BinaryIO.cc BitString.cc \
BmpFile.cc BmpPaletteSampleValue.cc BmpRGBSampleValue.cc BmpSampleValue.cc \
ColorPalette.cc ConstructionHeuristic.cc CvrStgFile.cc Edge.cc EdgeIterator.cc \
EmbData.cc Embedder.cc EncryptionAlgorithm.cc EncryptionMode.cc Extractor.cc \
Graph.cc JpegFile.cc JpegSampleValue.cc MCryptPP.cc MHashKeyGen.cc MHashPP.cc \
Matching.cc ProgressOutput.cc PseudoRandomSource.cc RGBTriple.cc \
RandomSource.cc SampleValue.cc SampleValueOppositeNeighbourhood.cc Selector.cc \
Session.cc SteghideError.cc Terminal.cc Vertex.cc VertexContent.cc WavChunk.cc \
WavChunkHeader.cc WavChunkUnused.cc WavFile.cc WavFormatChunk.cc \
WavPCMSampleValue.cc error.cc jpegbase.cc jpegentropycoded.cc jpegframe.cc \
jpegframehdr.cc jpeghufftable.cc jpegjfifapp0.cc jpegrestart.cc jpegscan.cc \
jpegscanhdr.cc jpegunusedseg.cc main.cc msg.cc
localedir = $(datadir)/locale
LIBTOOL = $(SHELL) libtool
MAINTAINERCLEANFILES = Makefile.in
