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

#ifndef SH_MATCHINGTEST_H
#define SH_MATCHINGTEST_H

#define private public
#define protected public
#include "Matching.h"
#undef private
#undef protected
class BitString ;
class CvrStgFile ;
class Selector ;

#include "UnitTest.h"
#include "TestSuite.h"

class MatchingTest : public UnitTest {
	public:
	MatchingTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testAddRemoveEdge (void) ;

	private:
	BitString *bs1 ;
	CvrStgFile *f1 ;
	Selector *s1 ;
	Graph *g1 ;
	Matching *m1 ;
	Globals gl1 ;
} ;

#endif // ndef SH_MATCHINGTEST_H
