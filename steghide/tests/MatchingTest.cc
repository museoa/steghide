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
#include "Edge.h"
#include "Graph.h"
#include "Selector.h"

#include "DummyFile.h"
#include "MatchingTest.h"

MatchingTest::MatchingTest (TestSuite* s)
	: UnitTest ("Matching", s)
{
	std::vector<std::list<UWORD16> > adjlist1 (4) ;
	adjlist1[0].push_back(1) ; adjlist1[0].push_back(2) ;
	adjlist1[1].push_back(2) ; adjlist1[1].push_back(3) ;
	DummyFile::createGraph (adjlist1, &bs1, &f1, &s1) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	m1 = new Matching (g1) ;
	
	ADDTESTCATEGORY (MatchingTest, testAddRemoveEdge) ;
}

MatchingTest::~MatchingTest ()
{
#if 0
	// FIXME - causes unittests to crash at end of destructor
	delete m1 ;
	delete g1 ;
	delete s1 ;
	delete bs1 ;
	delete f1 ;
#endif
}

void MatchingTest::testAddRemoveEdge ()
{
	{ // test Matching m1
		Edge *e1 = new Edge (g1->getVertex(0), 0, g1->getVertex(2), 1) ;
		m1->addEdge (e1) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isExposed((VertexLabel) 1) &&
						m1->isMatched((VertexLabel) 2) &&
						m1->isExposed((VertexLabel) 3)) ;

		Edge *e2 = new Edge (g1->getVertex(1), 0, g1->getVertex(3), 1) ;
		m1->addEdge (e2) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isMatched((VertexLabel) 2) &&
						m1->isMatched((VertexLabel) 3)) ;

		m1->removeEdge (e1) ;
		addTestResult (	m1->isExposed((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isExposed((VertexLabel) 2) &&
						m1->isMatched((VertexLabel) 3)) ;

		m1->removeEdge (e2) ;
		addTestResult (	m1->isExposed((VertexLabel) 0) &&
						m1->isExposed((VertexLabel) 1) &&
						m1->isExposed((VertexLabel) 2) &&
						m1->isExposed((VertexLabel) 3)) ;

		delete e1 ;
		delete e2 ;
	}
}
