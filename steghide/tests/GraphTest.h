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

#ifndef SH_GRAPHTEST_H
#define SH_GRAPHTEST_H

#include "UnitTest.h"

class TestSuite ;
class CvrStgFile ;
class BitString ;
class Permutation ;
class Graph ;

class GraphTest : public UnitTest {
	public:
	GraphTest (TestSuite* s) ;
	~GraphTest (void) ;

	void testVertices (void) ;
	void testSampleValues (void) ;
	void testVertexContents (void) ;
	void testSampleOccurences (void) ;
	void testSampleValueOppNeighs (void) ;
	
	private:
	std::string* datadir ;
	CvrStgFile *f1, *f2, *f3, *f4, *f5, *f6, *f7, *f8, *f_f ;
	BitString *bs1, *bs2, *bs3, *bs4, *bs5, *bs6, *bs7, *bs8, *bs_f ;
	Permutation *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p_f ;
	Graph *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *g_f ;
} ;

#endif // ndef SH_GRAPHTEST_H
