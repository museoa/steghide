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
#include "Selector.h"

#include "utcommon.h"
#include "GraphTest.h"

GraphTest::GraphTest (TestSuite* s)
	: UnitTest ("Graph", s)
{
	ADDTESTCATEGORY (GraphTest, testVertices) ;
	ADDTESTCATEGORY (GraphTest, testSampleValues) ;
	ADDTESTCATEGORY (GraphTest, testVertexContents) ;
	ADDTESTCATEGORY (GraphTest, testSampleValueOppNeighs) ;
	ADDTESTCATEGORY (GraphTest, testSampleOccurences) ;
}

void GraphTest::setup ()
{
	// FIXME - do this tests with other files too (maybe the same situation in other unit tests)
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "win3x1_std.bmp") ;
	bs1 = new BitString (std::string ("this BitString will be embedded")) ;
	s1 = new Selector (bs1->getLength() * f1->getSamplesPerEBit(), std::string ("a passphrase")) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "win3x4_std.bmp") ;
	bs2 = new BitString (std::string ("this is a test")) ;
	s2 = new Selector (bs2->getLength() * f2->getSamplesPerEBit(), std::string ("another passphrase")) ;
	g2 = new Graph (f2, *bs2, *s2) ;
	gl2 = Globs ;

	Globs.reset() ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
	bs3 = new BitString (std::string ("this is another test")) ;
	s3 = new Selector (bs3->getLength() * f3->getSamplesPerEBit(), std::string ("yet another passphrase")) ;
	g3 = new Graph (f3, *bs3, *s3) ;
	gl3 = Globs ;

	Globs.reset() ;
	f4 = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs4 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s4 = new Selector (bs4->getLength() * f4->getSamplesPerEBit(), std::string ("a true-color passphrase ;-)")) ;
	g4 = new Graph (f4, *bs4, *s4) ;
	gl4 = Globs ;

	Globs.reset() ;
	f5 = CvrStgFile::readFile (std::string(DATADIR) + "os21x1_std.bmp") ;
	bs5 = new BitString (std::string ("this BitString will be embedded")) ;
	s5 = new Selector (bs5->getLength() * f5->getSamplesPerEBit(), std::string ("a passphrase")) ;
	g5 = new Graph (f5, *bs5, *s5) ;
	gl5 = Globs ;

	Globs.reset() ;
	f6 = CvrStgFile::readFile (std::string(DATADIR) + "os21x4_std.bmp") ;
	bs6 = new BitString (std::string ("this is a test")) ;
	s6 = new Selector (bs6->getLength() * f6->getSamplesPerEBit(), std::string ("another passphrase")) ;
	g6 = new Graph (f6, *bs6, *s6) ;
	gl6 = Globs ;

	Globs.reset() ;
	f7 = CvrStgFile::readFile (std::string(DATADIR) + "os21x8_std.bmp") ;
	bs7 = new BitString (std::string ("this is another test")) ;
	s7 = new Selector (bs7->getLength() * f7->getSamplesPerEBit(), std::string ("yet another passphrase")) ;
	g7 = new Graph (f7, *bs7, *s7) ;
	gl7 = Globs ;

	Globs.reset() ;
	f8 = CvrStgFile::readFile (std::string(DATADIR) + "os21x24_std.bmp") ;
	bs8 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s8 = new Selector (bs8->getLength() * f8->getSamplesPerEBit(), std::string ("a true-color passphrase ;-)")) ;
	g8 = new Graph (f8, *bs8, *s8) ;
	gl8 = Globs ;

	Globs.reset() ;
	f_f = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs_f = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s_f = new Selector (bs_f->getLength() * f_f->getSamplesPerEBit(), std::string ("a true-color passphrase ;-)")) ;
	g_f = new Graph (f_f, *bs_f, *s_f) ;
	gl_f = Globs ;
}

void GraphTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete f1 ; delete bs1 ; delete s1 ; delete g1 ;
	delete f2 ; delete bs2 ; delete s2 ; delete g2 ;
	delete f3 ; delete bs3 ; delete s3 ; delete g3 ;
	delete f4 ; delete bs4 ; delete s4 ; delete g4 ;
	delete f5 ; delete bs5 ; delete s5 ; delete g5 ;
	delete f6 ; delete bs6 ; delete s6 ; delete g6 ;
	delete f7 ; delete bs7 ; delete s7 ; delete g7 ;
	delete f8 ; delete bs8 ; delete s8 ; delete g8 ;
	delete f_f ; delete bs_f ; delete s_f ; delete g_f ;
}

void GraphTest::testVertices()
{
	Globs = gl1 ; addTestResult (g1->check_Vertices(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_Vertices(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_Vertices(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_Vertices(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_Vertices(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_Vertices(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_Vertices(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_Vertices(ArgVerbose)) ;

	// violate label consistency
	Globs = gl_f ;
	Vertex* tmp = g_f->Vertices[10] ;
	g_f->Vertices[10] = g_f->Vertices[11] ;
	g_f->Vertices[11] = tmp ;
	addTestResult (!g_f->check_Vertices()) ;
}

void GraphTest::testSampleValues()
{
	Globs = gl1 ; addTestResult (g1->check_SampleValues(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_SampleValues(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_SampleValues(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_SampleValues(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_SampleValues(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_SampleValues(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_SampleValues(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_SampleValues(ArgVerbose)) ;

	// violate uniqueness
	Globs = gl_f ;
	g_f->SampleValues[0] = g_f->SampleValues[1] ;
	addTestResult (!g_f->check_SampleValues()) ;
}

void GraphTest::testVertexContents()
{
	Globs = gl1 ; addTestResult (g1->check_VertexContents(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_VertexContents(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_VertexContents(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_VertexContents(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_VertexContents(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_VertexContents(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_VertexContents(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_VertexContents(ArgVerbose)) ;
}

void GraphTest::testSampleOccurences()
{
	Globs = gl1 ; addTestResult (g1->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_SampleOccurences(ArgVerbose)) ;
}

void GraphTest::testSampleValueOppNeighs()
{
	Globs = gl1 ; addTestResult (g1->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->SampleValueOppNeighs.check(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->SampleValueOppNeighs.check(ArgVerbose)) ;
}
