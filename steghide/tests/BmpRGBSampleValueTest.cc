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

#include "BmpRGBSampleValueTest.h"
#include "utcommon.h"

#include "BmpRGBSampleValue.h"
#include "CvrStgFile.h"

BmpRGBSampleValueTest::BmpRGBSampleValueTest (TestSuite* s)
	: SampleValueTest ("BmpRGBSampleValue", s)
{
	datadir = new std::string (DATADIR) ;
	f_win = CvrStgFile::readFile (*datadir + "win3x24_std.bmp") ;
	f_os2 = CvrStgFile::readFile (*datadir + "os21x24_std.bmp") ;

	sv_0_0_0 = new BmpRGBSampleValue (f_win, 0, 0, 0) ;
	sv_1_1_1 = new BmpRGBSampleValue (f_win, 1, 1, 1) ;
	sv_0_3_4 = new BmpRGBSampleValue (f_win, 0, 3, 4) ;
	sv_12_13_14 = new BmpRGBSampleValue (f_win, 12, 13, 14) ;
	sv_128_128_128 = new BmpRGBSampleValue (f_win, 128, 128, 128) ;
	sv_210_0_120 = new BmpRGBSampleValue (f_win, 210, 0, 120) ;
	sv_255_255_255 = new BmpRGBSampleValue (f_win, 255, 255, 255) ;

	ADDTESTCATEGORY (BmpRGBSampleValueTest, testDistance) ;
}

BmpRGBSampleValueTest::~BmpRGBSampleValueTest()
{
	delete datadir ;
	delete f_win ; delete f_os2 ;
	delete sv_0_0_0 ;
	delete sv_1_1_1 ;
	delete sv_0_3_4 ;
	delete sv_12_13_14 ;
	delete sv_128_128_128 ;
	delete sv_210_0_120 ;
	delete sv_255_255_255 ;
}

void BmpRGBSampleValueTest::testDistance()
{
	addTestResult (genericTestDistance (sv_0_0_0, sv_0_0_0, 0)) ;
	addTestResult (genericTestDistance (sv_0_0_0, sv_0_3_4, 25)) ;
	addTestResult (genericTestDistance (sv_0_3_4, sv_12_13_14, 344)) ;
	addTestResult (genericTestDistance (sv_1_1_1, sv_128_128_128, 48387)) ;
	addTestResult (genericTestDistance (sv_210_0_120, sv_12_13_14, 50609)) ;
	addTestResult (genericTestDistance (sv_0_0_0, sv_255_255_255, 195075)) ;
}
