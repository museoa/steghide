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

#ifndef SH_VERTEXCONTENTTEST_H
#define SH_VERTEXCONTENTTEST_H

#include "common.h"
class BitString ;
class CvrStgFile ;
class Graph ;
class Selector ;

#include "UnitTest.h"

class VertexContentTest : public UnitTest {
	public:
	VertexContentTest (TestSuite* s) ;

	void testConstruction (void) ;

	private:
	CvrStgFile *f1 ;
	BitString *bs1 ;
	Selector *s1 ;
	Graph *g1 ;

	bool genericTestConstruction (CvrStgFile* f, Graph* g, SamplePos* sposs, SamplePos* dest_sposs) ;
} ;

#endif // ndef SH_VERTEXCONTENTTEST_H
