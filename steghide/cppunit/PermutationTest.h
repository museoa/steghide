/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl.at>
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

#ifndef SH_PERMUTATIONTEST_H
#define SH_PERMUTATIONTEST_H

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#define private public
#define protected public
#include "Permutation.h"
#undef private
#undef protected

class PermutationTest : public CppUnit::TestCase {
	public:
	PermutationTest (std::string name) : CppUnit::TestCase (name) {} ;

	void setUp (void) ;
	void tearDown (void) ;
	void registerTests (CppUnit::TestSuite* suite) ;

	void testIsPermutation (void) ;

	private:
	Permutation *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8 ;

	/**
	 * CPPUNIT_ASSERTs that p is a permutation, i.e. that
	 * every i \in {0...Width-1} occures exactly once in p and
	 * that no i >= Width occures in p.
	 **/
	void genericTestIsPermutation (Permutation& p) ;
} ;

#endif // ndef SH_PERMUTATIONTEST_H
