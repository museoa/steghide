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

#include "WavFileTest.h"
#include "utcommon.h"

WavFileTest::WavFileTest (TestSuite* s)
	: CvrStgFileTest("WavFile", s)
{
	datadir = new std::string (DATADIR) ;
	bs1 = new BitString (std::string ("this is a test string for the WavFile")) ;
	bs2 = new BitString (std::string ("this is another test - this time a little longer, but also for the WavFile unit test")) ;
	f1 = CvrStgFile::readFile (*datadir + "pcm8_std.wav") ;
	f2 = CvrStgFile::readFile (*datadir + "pcm16_std.wav") ;

	ADDTESTCATEGORY (WavFileTest, testReadWrite) ;
	ADDTESTCATEGORY (WavFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (WavFileTest, testReadEmbedWriteReadExtract) ;
	// TODO ADDTESTCATEGORY (WavFileTest, testPosition) ;
}

WavFileTest::~WavFileTest()
{
	delete datadir ;
	delete bs1 ; delete bs2 ;
	delete f1 ; delete f2 ;
}

void WavFileTest::testReadWrite()
{
	addTestResult (genericTestReadWrite (*datadir + "pcm8_std.wav")) ;
	addTestResult (genericTestReadWrite (*datadir + "pcm16_std.wav")) ;
}

void WavFileTest::testReadEmbedExtract()
{
	addTestResult (genericTestReadEmbedExtract (*datadir + "pcm8_std.wav", *bs1)) ;
	addTestResult (genericTestReadEmbedExtract (*datadir + "pcm16_std.wav", *bs2)) ;
}

void WavFileTest::testReadEmbedWriteReadExtract()
{
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "pcm8_std.wav", *bs2)) ;
	addTestResult (genericTestReadEmbedWriteReadExtract (*datadir + "pcm16_std.wav", *bs1)) ;
}

void WavFileTest::testPosition()
{
	// TODO
}
