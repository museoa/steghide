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

#include <iostream>

#include "utcommon.h"
#include "BmpWinFileTest.h"

#include "BmpPaletteSampleValue.h"
#include "BmpRGBSampleValue.h"

BmpWinFileTest::BmpWinFileTest (TestSuite* s)
	: CvrStgFileTest("BmpWinFile", s)
{
	datadir = new std::string (DATADIR) ;
	bs1 = new BitString (std::string ("this is a test")) ;
	bs2 = new BitString (std::string ("this is another test - this time a little longer")) ;
	bs3 = new BitString (std::string ("yet another test which is even longer than the previous test bitstring")) ;
	bs4 = new BitString (std::string ("the last test")) ;
	f1 = CvrStgFile::readFile (*datadir + "win3x1_std.bmp") ;
	f2 = CvrStgFile::readFile (*datadir + "win3x4_std.bmp") ;
	f3 = CvrStgFile::readFile (*datadir + "win3x8_std.bmp") ;
	f4 = CvrStgFile::readFile (*datadir + "win3x24_std.bmp") ;

	ADDTESTCATEGORY (BmpWinFileTest, testReadWrite) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (BmpWinFileTest, testPosition) ;
}

BmpWinFileTest::~BmpWinFileTest()
{
	delete datadir ;
	delete bs1 ; delete bs2 ; delete bs3 ; delete bs4 ;
	delete f1 ; delete f2 ; delete f3 ; delete f4 ;
}

void BmpWinFileTest::testReadWrite()
{
	addTestResult (genericTestReadWrite (*datadir + "win3x1_std.bmp")) ;
	addTestResult (genericTestReadWrite (*datadir + "win3x4_std.bmp")) ;
	addTestResult (genericTestReadWrite (*datadir + "win3x8_std.bmp")) ;
	addTestResult (genericTestReadWrite (*datadir + "win3x24_std.bmp")) ;
}

void BmpWinFileTest::testReadEmbedExtract()
{
	addTestResult (genericTestReadEmbedExtract (*datadir + "win3x1_std.bmp", *bs1)) ;
	addTestResult (genericTestReadEmbedExtract (*datadir + "win3x4_std.bmp", *bs2)) ;
	addTestResult (genericTestReadEmbedExtract (*datadir + "win3x8_std.bmp", *bs3)) ;
	addTestResult (genericTestReadEmbedExtract (*datadir + "win3x24_std.bmp", *bs4)) ;
}

void BmpWinFileTest::testReadEmbedWriteReadExtract()
{
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "win3x1_std.bmp", *bs2)) ;
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "win3x4_std.bmp", *bs3)) ;
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "win3x8_std.bmp", *bs4)) ;
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "win3x24_std.bmp", *bs1)) ;
}

void BmpWinFileTest::testPosition()
{
	addTestResult (genericTestPosition (f1, 0, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 1, new BmpPaletteSampleValue (f1, 0))) ;
	addTestResult (genericTestPosition (f1, 143, new BmpPaletteSampleValue (f1, 0))) ;
	addTestResult (genericTestPosition (f1, 144, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 2303, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 2302, new BmpPaletteSampleValue (f1, 1))) ;

	addTestResult (genericTestPosition (f2, 0, new BmpPaletteSampleValue (f2, 6))) ;
	addTestResult (genericTestPosition (f2, 20, new BmpPaletteSampleValue (f2, 8))) ;
	addTestResult (genericTestPosition (f2, 47, new BmpPaletteSampleValue (f2, 3))) ;
	addTestResult (genericTestPosition (f2, 48, new BmpPaletteSampleValue (f2, 4))) ;
	addTestResult (genericTestPosition (f2, 673, new BmpPaletteSampleValue (f2, 10))) ;
	addTestResult (genericTestPosition (f2, 2303, new BmpPaletteSampleValue (f2, 13))) ;

	addTestResult (genericTestPosition (f3, 0, new BmpPaletteSampleValue (f3, 81))) ;
	addTestResult (genericTestPosition (f3, 4, new BmpPaletteSampleValue (f3, 242))) ;
	addTestResult (genericTestPosition (f3, 47, new BmpPaletteSampleValue (f3, 145))) ;
	addTestResult (genericTestPosition (f3, 48, new BmpPaletteSampleValue (f3, 35))) ;
	addTestResult (genericTestPosition (f3, 2302, new BmpPaletteSampleValue (f3, 192))) ;

	addTestResult (genericTestPosition (f4, 0, new BmpRGBSampleValue (f4, 81, 105, 16))) ;
	addTestResult (genericTestPosition (f4, 1, new BmpRGBSampleValue (f4, 110, 151, 26))) ;
	addTestResult (genericTestPosition (f4, 12, new BmpRGBSampleValue (f4, 159, 160, 37))) ;
	addTestResult (genericTestPosition (f4, 191, new BmpRGBSampleValue (f4, 127, 68, 96))) ;
	addTestResult (genericTestPosition (f4, 192, new BmpRGBSampleValue (f4, 69, 132, 161))) ;
	addTestResult (genericTestPosition (f4, 2303, new BmpRGBSampleValue (f4, 109, 169, 133))) ;
}
