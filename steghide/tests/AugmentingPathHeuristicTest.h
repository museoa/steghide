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

#ifndef SH_AUGMENTINGPATHHEURISTICTEST_H
#define SH_AUGMENTINGPATHHEURISTICTEST_H

class BitString ;
class CvrStgFile ;
#include "AugmentingPathHeuristic.h"
class Selector ;

#include "UnitTest.h"
#include "TestSuite.h"

class AugmentingPathHeuristicTest : public UnitTest {
	public:
	AugmentingPathHeuristicTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testAlgorithm (void) ;

	private:
	BitString *bs1, *bs2, *bs3, *bs4 ;
	CvrStgFile *f1, *f2, *f3, *f4 ;
	Selector *s1, *s2, *s3, *s4 ;
	Graph *g1, *g2, *g3, *g4 ;
	Matching *m1, *m2, *m3, *m4 ;
	AugmentingPathHeuristic *aph1, *aph2, *aph3, *aph4 ;
	Globals gl1, gl2, gl3, gl4 ;
} ;

#endif // ndef SH_AUGMENTINGPATHHEURISTICTEST_H
