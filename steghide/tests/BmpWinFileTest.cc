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

BmpWinFileTest::BmpWinFileTest (TestSuite* s)
	: CvrStgFileTest("BmpWinFile", s)
{
	datadir = new std::string (DATADIR) ;
	bs1 = new BitString (std::string ("this is a test")) ;
	bs2 = new BitString (std::string ("this is another test - this time a little longer")) ;
	bs3 = new BitString (std::string ("yet another test which is even longer than the previous test bitstring")) ;
	bs4 = new BitString (std::string ("the last test")) ;

	ADDTESTCATEGORY (BmpWinFileTest, testReadWrite) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedWriteReadExtract) ;
}

BmpWinFileTest::~BmpWinFileTest()
{
	delete datadir ;
	delete bs1 ;
	delete bs2 ;
	delete bs3 ;
	delete bs4 ;
}

void BmpWinFileTest::testReadWrite (void)
{
	addTestResult(genericTestReadWrite(*datadir + "win3x1_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "win3x4_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "win3x8_std.bmp")) ;
	addTestResult(genericTestReadWrite(*datadir + "win3x24_std.bmp")) ;
}

void BmpWinFileTest::testReadEmbedExtract (void)
{
	addTestResult(genericTestReadEmbedExtract(*datadir + "win3x1_std.bmp", *bs1)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "win3x4_std.bmp", *bs2)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "win3x8_std.bmp", *bs3)) ;
	addTestResult(genericTestReadEmbedExtract(*datadir + "win3x24_std.bmp", *bs4)) ;
}

void BmpWinFileTest::testReadEmbedWriteReadExtract (void)
{
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "win3x1_std.bmp", *bs2)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "win3x4_std.bmp", *bs3)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "win3x8_std.bmp", *bs4)) ;
	addTestResult(genericTestReadEmbedWriteReadExtract(*datadir + "win3x24_std.bmp", *bs1)) ;
}
