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

#include "TestSuite.h"

#include "AuFileTest.h"
#include "AugmentingPathHeuristicTest.h"
#include "BitStringTest.h"
#include "BmpRGBSampleValueTest.h"
#include "BmpPaletteSampleValueTest.h"
#include "BmpOS2FileTest.h"
#include "BmpWinFileTest.h"
#include "EmbDataTest.h"
#include "ColorPaletteTest.h"
#include "GraphTest.h"
#include "MatchingTest.h"
#include "MHashPPTest.h"
#include "SampleValueOppositeNeighbourhoodTest.h"
#include "SelectorTest.h"
#include "VertexContentTest.h"
#include "WavFileTest.h"

bool ArgVerbose ;

int main (int argc, char *argv[])
{
	ArgVerbose = false ;
	if (argc > 1) {
		if (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--verbose") {
			ArgVerbose = true ;
		}
	}

	TestSuite ts ;

	// basic classes
	BitStringTest bst (&ts) ;
	ts.addUnitTest (&bst) ;

	MHashPPTest mht (&ts) ;
	ts.addUnitTest (&mht) ;

	SelectorTest st (&ts) ;
	ts.addUnitTest (&st) ;

	ColorPaletteTest cpt  (&ts) ;
	ts.addUnitTest (&cpt) ;

	EmbDataTest edt (&ts) ;
	ts.addUnitTest (&edt) ;

	// SampleValues
	BmpRGBSampleValueTest rgbsvt (&ts) ;
	ts.addUnitTest (&rgbsvt) ;

	BmpPaletteSampleValueTest palsvt (&ts) ;
	ts.addUnitTest (&palsvt) ;

	// CvrStgFiles
	AuFileTest aut (&ts) ;
	ts.addUnitTest (&aut) ;

	BmpOS2FileTest bmpot (&ts) ;
	ts.addUnitTest (&bmpot) ;

	BmpWinFileTest bmpwt (&ts) ;
	ts.addUnitTest (&bmpwt) ;

	WavFileTest wt (&ts) ;
	ts.addUnitTest (&wt) ;

	// graph-theoretic stuff
	VertexContentTest vct (&ts) ;
	ts.addUnitTest (&vct) ;

	SampleValueOppositeNeighbourhoodTest svont (&ts) ;
	ts.addUnitTest (&svont) ;

	GraphTest gt (&ts) ;
	ts.addUnitTest (&gt) ;

	MatchingTest mt (&ts) ;
	ts.addUnitTest (&mt) ;

	AugmentingPathHeuristicTest apht (&ts) ;
	ts.addUnitTest (&apht) ;

	ts.run() ;

	return (ts.getResult() ? 0 : -1) ;
}
