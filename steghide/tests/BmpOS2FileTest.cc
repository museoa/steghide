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

#include "BmpPaletteSampleValue.h"
#include "BmpRGBSampleValue.h"

#include "utcommon.h"
#include "BmpOS2FileTest.h"

BmpOS2FileTest::BmpOS2FileTest (TestSuite* s)
	: CvrStgFileTest("BmpOS2File", s)
{
	datadir = new std::string (DATADIR) ;
	bs1 = new BitString (std::string ("a test")) ;
	bs2 = new BitString (std::string ("another test - this time a little longer")) ;
	bs3 = new BitString (std::string ("yet another test")) ;
	bs4 = new BitString (std::string ("last test")) ;

	f1 = CvrStgFile::readFile (*datadir + "os21x1_std.bmp") ;
	f2 = CvrStgFile::readFile (*datadir + "os21x4_std.bmp") ;
	f3 = CvrStgFile::readFile (*datadir + "os21x8_std.bmp") ;
	f4 = CvrStgFile::readFile (*datadir + "os21x24_std.bmp") ;

	ADDTESTCATEGORY (BmpOS2FileTest, testReadWrite) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testPosition) ;
}

BmpOS2FileTest::~BmpOS2FileTest()
{
	delete datadir ;
	delete bs1 ; delete bs2 ; delete bs3 ; delete bs4 ;
	delete f1 ; delete f2 ; delete f3 ; delete f4 ;
}

void BmpOS2FileTest::testReadWrite (void)
{
	addTestResult(genericTestReadWrite(*datadir + "os21x1_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "os21x4_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "os21x8_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "os21x24_std.bmp")) ;
}

void BmpOS2FileTest::testReadEmbedExtract (void)
{
	addTestResult(genericTestReadEmbedExtract(*datadir + "os21x1_std.bmp", *bs1)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "os21x4_std.bmp", *bs2)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "os21x8_std.bmp", *bs3)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "os21x24_std.bmp", *bs4)) ;
}

void BmpOS2FileTest::testReadEmbedWriteReadExtract (void)
{
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "os21x1_std.bmp", *bs2)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "os21x4_std.bmp", *bs3)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "os21x8_std.bmp", *bs4)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "os21x24_std.bmp", *bs1)) ;
}

void BmpOS2FileTest::testPosition()
{
	addTestResult (genericTestPosition (f1, 0, new BmpPaletteSampleValue (f1, 0))) ;
	addTestResult (genericTestPosition (f1, 1, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 2, new BmpPaletteSampleValue (f1, 0))) ;
	addTestResult (genericTestPosition (f1, 47, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 48, new BmpPaletteSampleValue (f1, 1))) ;
	addTestResult (genericTestPosition (f1, 2299, new BmpPaletteSampleValue (f1, 0))) ;
	addTestResult (genericTestPosition (f1, 2303, new BmpPaletteSampleValue (f1, 1))) ;

	addTestResult (genericTestPosition (f2, 0, new BmpPaletteSampleValue (f2, 3))) ;
	addTestResult (genericTestPosition (f2, 10, new BmpPaletteSampleValue (f2, 1))) ;
	addTestResult (genericTestPosition (f2, 95, new BmpPaletteSampleValue (f2, 5))) ;
	addTestResult (genericTestPosition (f2, 96, new BmpPaletteSampleValue (f2, 2))) ;
	addTestResult (genericTestPosition (f2, 2208, new BmpPaletteSampleValue (f2, 7))) ;
}
