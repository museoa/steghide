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

#ifndef SH_SAMPLEVALUEOPPOSITENEIGHBOURHOODTEST_H
#define SH_SAMPLEVALUEOPPOSITENEIGHBOURHOODTEST_H

#define private public
#define protected public
#include "SampleValueOppositeNeighbourhood.h"
#undef private
#undef protected

#include "UnitTest.h"
#include "TestSuite.h"

class SampleValueOppositeNeighbourhoodTest : public UnitTest {
	public:
	SampleValueOppositeNeighbourhoodTest (TestSuite* s) ;

	void setup (void) ;

	void testQuicksort (void) ;

	private:
	SampleValueOppositeNeighbourhood DummySVON ;
	std::vector<SampleValue*> SVEmpty ;
} ;

#endif // ndef SH_SAMPLEVALUEOPPOSITENEIGHBOURHOODTEST_H
