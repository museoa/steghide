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

#include "JpegSampleValueTest.h"
#include "utcommon.h"

#include "JpegSampleValue.h"
#include "CvrStgFile.h"

JpegSampleValueTest::JpegSampleValueTest (TestSuite* s)
	: SampleValueTest ("JpegSampleValue", s)
{
	ADDTESTCATEGORY (JpegSampleValueTest, testDistance) ;
	ADDTESTCATEGORY (JpegSampleValueTest, testIsNeighbour) ;
}

void JpegSampleValueTest::setup ()
{
	UnitTest::setup() ;

#if 0
	// TODO - write these tests
	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "???.jpg") ;
	gl1 = Globs ;
#endif
}

void JpegSampleValueTest::cleanup ()
{
	UnitTest::cleanup() ;
#if 0
	delete f1 ;
#endif
}

void JpegSampleValueTest::testDistance ()
{
#if 0
	Globs = gl_AuMuLaw ;
	addTestResult (genericTestDistance (sv_AuMuLaw_0, sv_AuMuLaw_1, 1)) ;
	addTestResult (genericTestDistance (sv_AuMuLaw_1, sv_AuMuLaw_45, 44)) ;

	Globs = gl_AuPCM16 ;
	addTestResult (genericTestDistance (sv_AuPCM16_0, sv_AuPCM16_15, 15)) ;
	addTestResult (genericTestDistance (sv_AuPCM16_15, sv_AuPCM16_32767, 32752)) ;
	addTestResult (genericTestDistance (sv_AuPCM16_m32768, sv_AuPCM16_32767, 65535)) ;
#endif
}

void JpegSampleValueTest::testIsNeighbour ()
{
#if 0
	Globs = gl_AuMuLaw ;
	addTestResult (genericTestIsNeighbour (sv_AuMuLaw_0, sv_AuMuLaw_1, true)) ;
	addTestResult (genericTestIsNeighbour (sv_AuMuLaw_0, sv_AuMuLaw_45, false)) ;

	Globs = gl_AuPCM16 ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_0, sv_AuPCM16_15, true)) ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_m200, sv_AuPCM16_0, false)) ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_m32767, sv_AuPCM16_32767, false)) ;
#endif
}
