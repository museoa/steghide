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


#include "utcommon.h"
#include "JpegFileTest.h"

JpegFileTest::JpegFileTest (TestSuite* s)
	: CvrStgFileTest("JpegFile", s)
{
	ADDTESTCATEGORY (JpegFileTest, testReadWrite) ;
	ADDTESTCATEGORY (JpegFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (JpegFileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (JpegFileTest, testPosition) ;
}

void JpegFileTest::setup ()
{
	UnitTest::setup() ;

#if 0
	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;
	bs1 = new BitString (std::string ("a test passphrase")) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "pcm8_std.au") ;
	bs2 = new BitString (std::string ("another test passphrase")) ;
	gl2 = Globs ;

	Globs.reset() ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.au") ;
	bs3 = new BitString (std::string ("inventing test passphrases is boring")) ;
	gl3 = Globs ;
#endif
}

void JpegFileTest::cleanup ()
{
	UnitTest::cleanup() ;

#if 0
	delete bs1 ; delete bs2 ; delete bs3 ;
	delete f1 ; delete f2 ; delete f3 ;
#endif
}

void JpegFileTest::testReadWrite()
{
#if 0
	Globs = gl1 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "mulaw_std.au")) ;
	Globs = gl2 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm8_std.au")) ;
	Globs = gl3 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm16_std.au")) ;
#endif
}

void JpegFileTest::testReadEmbedExtract()
{
#if 0
	Globs = gl1 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "mulaw_std.au", *bs1)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm8_std.au", *bs2)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm16_std.au", *bs3)) ;
#endif
}

void JpegFileTest::testReadEmbedWriteReadExtract()
{
#if 0
	Globs = gl1 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "mulaw_std.au", *bs2)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm8_std.au", *bs3)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm16_std.au", *bs1)) ;
#endif
}

void JpegFileTest::testPosition()
{
#if 0
	Globs = gl1 ;

	Globs = gl2 ;

	Globs = gl3 ;
#endif
}

void JpegFileTest::testReadExtractCompare ()
{
	// TODO
}
