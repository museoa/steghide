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

#include "BitStringTest.h"
#include "BmpRGBSampleValueTest.h"
#include "BmpOS2FileTest.h"
#include "BmpWinFileTest.h"
#include "GraphTest.h"
#include "MHashTest.h"
#include "PermutationTest.h"

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

	MHashTest mht (&ts) ;
	ts.addUnitTest (&mht) ;

	PermutationTest pt (&ts) ;
	ts.addUnitTest (&pt) ;

	// SampleValues
	BmpRGBSampleValueTest rgbsvt (&ts) ;
	ts.addUnitTest (&rgbsvt) ;

	// CvrStgFiles
	BmpOS2FileTest bmpot (&ts) ;
	ts.addUnitTest (&bmpot) ;

	BmpWinFileTest bmpwt (&ts) ;
	ts.addUnitTest (&bmpwt) ;

	// graph-theoretic stuff
	GraphTest gt (&ts) ;
	ts.addUnitTest (&gt) ;

	ts.run() ;

	return (ts.getResult() ? 0 : -1) ;
}
