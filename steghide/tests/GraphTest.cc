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

#include "BitString.h"
#include "CvrStgFile.h"
#define private public
#define protected public
#include "Graph.h"
#undef private
#undef protected
#include "Permutation.h"

#include "utcommon.h"
#include "GraphTest.h"

GraphTest::GraphTest (TestSuite* s)
	: UnitTest ("GraphTest", s)
{
	datadir = new std::string (DATADIR) ;

	f1 = CvrStgFile::readFile (*datadir + "win3x1_std.bmp") ;
	bs1 = new BitString (std::string ("this BitString will be embedded")) ;
	p1 = new Permutation (bs1->getLength() * f1->getSamplesPerEBit(), std::string ("a passphrase")) ;
	g1 = new Graph (f1, *bs1, *p1) ;

	f2 = CvrStgFile::readFile (*datadir + "win3x4_std.bmp") ;
	bs2 = new BitString (std::string ("this is a test")) ;
	p2 = new Permutation (bs2->getLength() * f2->getSamplesPerEBit(), std::string ("another passphrase")) ;
	g2 = new Graph (f2, *bs2, *p2) ;

	f3 = CvrStgFile::readFile (*datadir + "win3x8_std.bmp") ;
	bs3 = new BitString (std::string ("this is another test")) ;
	p3 = new Permutation (bs3->getLength() * f3->getSamplesPerEBit(), std::string ("yet another passphrase")) ;
	g3 = new Graph (f3, *bs3, *p3) ;

	f4 = CvrStgFile::readFile (*datadir + "win3x24_std.bmp") ;
	bs4 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	p4 = new Permutation (bs4->getLength() * f4->getSamplesPerEBit(), std::string ("a true-color passphrase ;-)")) ;
	g4 = new Graph (f4, *bs4, *p4) ;

	f5 = CvrStgFile::readFile (*datadir + "os21x1_std.bmp") ;
	bs5 = new BitString (std::string ("this BitString will be embedded")) ;
	p5 = new Permutation (bs5->getLength() * f5->getSamplesPerEBit(), std::string ("a passphrase")) ;
	g5 = new Graph (f5, *bs5, *p5) ;

	f6 = CvrStgFile::readFile (*datadir + "os21x4_std.bmp") ;
	bs6 = new BitString (std::string ("this is a test")) ;
	p6 = new Permutation (bs6->getLength() * f6->getSamplesPerEBit(), std::string ("another passphrase")) ;
	g6 = new Graph (f6, *bs6, *p6) ;

	f7 = CvrStgFile::readFile (*datadir + "os21x8_std.bmp") ;
	bs7 = new BitString (std::string ("this is another test")) ;
	p7 = new Permutation (bs7->getLength() * f7->getSamplesPerEBit(), std::string ("yet another passphrase")) ;
	g7 = new Graph (f7, *bs7, *p7) ;

	f8 = CvrStgFile::readFile (*datadir + "os21x24_std.bmp") ;
	bs8 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	p8 = new Permutation (bs8->getLength() * f8->getSamplesPerEBit(), std::string ("a true-color passphrase ;-)")) ;
	g8 = new Graph (f8, *bs8, *p8) ;

	ADDTESTCATEGORY (GraphTest, testSampleValueOppNeighs) ;
}

GraphTest::~GraphTest()
{
	delete datadir ;
	delete f1 ; delete bs1 ; delete p1 ; delete g1 ;
	delete f2 ; delete bs2 ; delete p2 ; delete g2 ;
	delete f3 ; delete bs3 ; delete p3 ; delete g3 ;
	delete f4 ; delete bs4 ; delete p4 ; delete g4 ;
	delete f5 ; delete bs5 ; delete p5 ; delete g5 ;
	delete f6 ; delete bs6 ; delete p6 ; delete g6 ;
	delete f7 ; delete bs7 ; delete p7 ; delete g7 ;
	delete f8 ; delete bs8 ; delete p8 ; delete g8 ;
}

void GraphTest::testSampleValueOppNeighs()
{
	addTestResult (g1->SampleValueOppNeighs.check()) ;
	addTestResult (g2->SampleValueOppNeighs.check()) ;
	addTestResult (g3->SampleValueOppNeighs.check()) ;
	addTestResult (g4->SampleValueOppNeighs.check()) ;
	addTestResult (g5->SampleValueOppNeighs.check()) ;
	addTestResult (g6->SampleValueOppNeighs.check()) ;
	addTestResult (g7->SampleValueOppNeighs.check()) ;
	addTestResult (g8->SampleValueOppNeighs.check()) ;
}
