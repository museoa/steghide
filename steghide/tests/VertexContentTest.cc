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

#include "VertexContentTest.h"
#include "utcommon.h"

VertexContentTest::VertexContentTest (TestSuite* s)
	: UnitTest("VertexContent", s)
{
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
	bs1 = new BitString (std::string ("this is another test")) ;
	s1 = new Selector (bs1->getLength() * f1->getSamplesPerEBit(), std::string ("yet another passphrase")) ;
	g1 = new Graph (f1, *bs1, *s1) ;

	ADDTESTCATEGORY (VertexContentTest, testConstruction) ;
}

void VertexContentTest::testConstruction (void)
{
	{
		SamplePos sposs[] = { 0, 1 } ;
		SamplePos dest_sposs[] = { 1, 0 } ;
		addTestResult (genericTestConstruction (f1, g1, sposs, dest_sposs)) ;
	}

	{
		SamplePos sposs[] = { 49, 10 } ;
		SamplePos dest_sposs[] = { 49, 10 } ;
		addTestResult (genericTestConstruction (f1, g1, sposs, dest_sposs)) ;
	}

	{
		SamplePos sposs[] = { 20, 65 } ;
		SamplePos dest_sposs[] = { 65, 20 } ;
		addTestResult (genericTestConstruction (f1, g1, sposs, dest_sposs)) ;
	}

	{
		SamplePos sposs[] = { 10, 30 } ;
		SamplePos dest_sposs[] = { 30, 10 } ;
		addTestResult (genericTestConstruction (f1, g1, sposs, dest_sposs)) ;
	}

	{ // violate correct order
		SamplePos sposs[] = { 10, 30 } ;
		SamplePos dest_sposs[] = { 10, 30 } ;
		addTestResult (!genericTestConstruction (f1, g1, sposs, dest_sposs)) ;
	}
}

bool VertexContentTest::genericTestConstruction (CvrStgFile* f, Graph* g, SamplePos* sposs, SamplePos* dest_sposs)
{
	SampleValue **svs = new SampleValue*[g->getSamplesPerVertex()] ;
	for (unsigned short k = 0 ; k < g->getSamplesPerVertex() ; k++) {
		svs[k] = f->getSampleValue(sposs[k]) ;
	}
	VertexContent vc (g, svs, sposs) ;

	// check
	bool retval = true ;
	for (unsigned short k = 0 ; k < g->getSamplesPerVertex() ; k++) {
		retval = (sposs[k] == dest_sposs[k]) && retval ;
	}
	return retval ;
}
