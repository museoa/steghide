/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <cstdlib>

#include "common.h"

#include "TestSuite.h"
#include "AudioSampleValueTest.h"
#include "AuFileTest.h"
#include "AugmentingPathHeuristicTest.h"
#include "BitStringTest.h"
#include "BmpRGBSampleValueTest.h"
#include "BmpPaletteSampleValueTest.h"
#include "BmpOS2FileTest.h"
#include "BmpWinFileTest.h"
#include "EdgeIteratorTest.h"
#include "EmbDataTest.h"
#include "ConstructionHeuristicTest.h"
#include "ColorPaletteTest.h"
#include "GraphTest.h"
#include "JpegFileTest.h"
#include "JpegSampleValueTest.h"
#include "MatchingTest.h"
#include "MCryptPPTest.h"
#include "MHashKeyGenTest.h"
#include "MHashPPTest.h"
#include "SampleValueOppositeNeighbourhoodTest.h"
#include "SelectorTest.h"
#include "VertexContentTest.h"
#include "WavFileTest.h"
#include "WavPCMSampleValueTest.h"

bool ArgVerbose ;

int main (int argc, char *argv[])
{
	ArgVerbose = false ;

	unsigned short argi = 1 ;
	while (argi < argc) {
		if (std::string(argv[argi]) == "-v" || std::string(argv[argi]) == "--verbose") {
			ArgVerbose = true ;
		}
#ifdef DEBUG
		else if (std::string(argv[argi]) == "--debuglevel") {
			argi++ ;
			unsigned int tmp = 0 ;
			sscanf (argv[argi], "%u", &tmp) ;
			Args.DebugLevel.setValue (tmp) ;
		}
#endif
		else {
			std::cerr << "unknown argument: " << argv[argi] << std::endl ;
			exit (EXIT_FAILURE) ;
		}
		argi++ ;
	}

	TestSuite ts ;

	// basic classes
	BitStringTest bst (&ts) ;
	ts.addUnitTest (&bst) ;

	SelectorTest st (&ts) ;
	ts.addUnitTest (&st) ;

	ColorPaletteTest cpt  (&ts) ;
	ts.addUnitTest (&cpt) ;

	EmbDataTest edt (&ts) ;
	ts.addUnitTest (&edt) ;

	// mlibs
	MHashPPTest mht (&ts) ;
	ts.addUnitTest (&mht) ;

	MHashKeyGenTest mhkgt (&ts) ;
	ts.addUnitTest (&mhkgt) ;

	MCryptPPTest mct (&ts) ;
	ts.addUnitTest (&mct) ;

	// SampleValues
	AudioSampleValueTest asvt (&ts) ;
	ts.addUnitTest (&asvt) ;

	BmpRGBSampleValueTest rgbsvt (&ts) ;
	ts.addUnitTest (&rgbsvt) ;

	BmpPaletteSampleValueTest palsvt (&ts) ;
	ts.addUnitTest (&palsvt) ;

	JpegSampleValueTest jsvt (&ts) ;
	ts.addUnitTest (&jsvt) ;

	WavPCMSampleValueTest wavsvt (&ts) ;
	ts.addUnitTest (&wavsvt) ;

	// CvrStgFiles
	AuFileTest aut (&ts) ;
	ts.addUnitTest (&aut) ;

	BmpOS2FileTest bmpot (&ts) ;
	ts.addUnitTest (&bmpot) ;

	BmpWinFileTest bmpwt (&ts) ;
	ts.addUnitTest (&bmpwt) ;

	JpegFileTest jft (&ts) ;
	ts.addUnitTest (&jft) ;

	WavFileTest wt (&ts) ;
	ts.addUnitTest (&wt) ;

	// graph-theoretic stuff
	VertexContentTest vct (&ts) ;
	ts.addUnitTest (&vct) ;

	SampleValueOppositeNeighbourhoodTest svont (&ts) ;
	ts.addUnitTest (&svont) ;

	GraphTest gt (&ts) ;
	ts.addUnitTest (&gt) ;

	EdgeIteratorTest eitt (&ts) ;
	ts.addUnitTest (&eitt) ;

	MatchingTest mt (&ts) ;
	ts.addUnitTest (&mt) ;

	ConstructionHeuristicTest cht (&ts) ;
	ts.addUnitTest (&cht) ;

	AugmentingPathHeuristicTest apht (&ts) ;
	ts.addUnitTest (&apht) ;

	ts.run() ;

	return (ts.getResult() ? 0 : -1) ;
}
