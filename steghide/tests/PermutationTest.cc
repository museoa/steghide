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

#include <iostream>

#include "PermutationTest.h"

PermutationTest::PermutationTest (TestSuite* s)
	: UnitTest ("Permutation", s)
{
	p1 = new Permutation (10, "a test") ;
	p2 = new Permutation (150, "another test") ;
	p2 = new Permutation (1, "a small test") ;
	p3 = new Permutation (15, "test near power of two") ;
	p4 = new Permutation (16, "another one") ;
	p5 = new Permutation (127, "and another one") ;
	p6 = new Permutation (128, "and the last") ;
	p7 = new Permutation (2000, "it even works with large") ;
	p8 = new Permutation (25000, "and really large widths") ;
	p9 = new Permutation (31) ;
	p10 = new Permutation (32) ;
	p11 = new Permutation (33) ;

	ADDTESTCATEGORY (PermutationTest, testIsPermutation) ;
	ADDTESTCATEGORY (PermutationTest, testIsIdPermutation) ;
}

PermutationTest::~PermutationTest()
{
	delete p1 ;
	delete p2 ;
	delete p3 ;
	delete p4 ;
	delete p5 ;
	delete p6 ;
	delete p7 ;
	delete p8 ;
	delete p9 ;
	delete p10 ;
	delete p11 ;
}

void PermutationTest::testIsPermutation()
{
	addTestResult (genericTestIsPermutation (*p1)) ;
	addTestResult (genericTestIsPermutation (*p2)) ;
	addTestResult (genericTestIsPermutation (*p3)) ;
	addTestResult (genericTestIsPermutation (*p4)) ;
	addTestResult (genericTestIsPermutation (*p5)) ;
	addTestResult (genericTestIsPermutation (*p6)) ;
	addTestResult (genericTestIsPermutation (*p7)) ;
	addTestResult (genericTestIsPermutation (*p8)) ;
}

void PermutationTest::testIsIdPermutation()
{
	addTestResult (genericTestIsIdPermutation (*p9)) ;
	addTestResult (genericTestIsIdPermutation (*p10)) ;
	addTestResult (genericTestIsIdPermutation (*p11)) ;
}

bool PermutationTest::genericTestIsPermutation (Permutation& p)
{
	std::vector<bool> hasoccured (p.getWidth()) ; // all set to false

	bool range_ok = true ;
	bool inj = true ;
	for (unsigned long i = 0 ; i < p.getWidth() ; i++) {
		UWORD32 value = p[i] ;
		range_ok = range_ok && (value < p.getWidth()) ;
		inj = inj && !hasoccured[value];
		hasoccured[value] = true ;
	}

	bool surj = true ;
	for (unsigned long i = 0 ; i < p.getWidth() ; i++) {
		surj = surj && hasoccured[i] ;
	}

	return range_ok && inj && surj ;
}

bool PermutationTest::genericTestIsIdPermutation (Permutation& p)
{
	bool ok = true ;
	for (UWORD32 i = 0 ; i < p.getWidth() ; i++) {
		ok = ok && (p[i] == i) ;
	}
	return ok ;
}
