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

#include "SampleValueOppositeNeighbourhoodTest.h"
#include "utcommon.h"

SampleValueOppositeNeighbourhoodTest::SampleValueOppositeNeighbourhoodTest (TestSuite* s)
	: UnitTest ("SampleValueOppositeNeighbourhood", s)
{
	SVEmpty = std::vector<SampleValue*> (100) ;

	ADDTESTCATEGORY (SampleValueOppositeNeighbourhoodTest, testQuicksort) ;
}

void SampleValueOppositeNeighbourhoodTest::testQuicksort ()
{
	{
		float* d = new float[2] ;
		d[0] = 3 ;
		d[1] = 5 ;
		DummySVON.quicksort (SVEmpty, d, 0, 1) ;
		addTestResult ((d[0] == 3) && (d[1] == 5)) ;
		delete[] d ;
	}

	{
		float* d = new float[2] ;
		d[0] = 5 ;
		d[1] = 3 ;
		DummySVON.quicksort (SVEmpty, d, 0, 1) ;
		addTestResult ((d[0] == 3) && (d[1] == 5)) ;
		delete[] d ;
	}

	{
		float* d = new float[3] ;
		d[0] = 2 ;
		d[1] = 2 ;
		d[2] = 2 ;
		DummySVON.quicksort (SVEmpty, d, 0, 2) ;
		addTestResult ((d[0] == 2) && (d[1] == 2) && (d[2] == 2)) ; // asserting termination
		delete[] d ;
	}

	{
		float* d = new float[10] ;
		d[0] = 3 ; d[1] = 12 ; d[2] = 8 ; d[3] = 2.5 ; d[4] = 22 ;
		d[5] = 7 ; d[6] = 8 ; d[7] = 4 ; d[8] = 2 ; d[9] = 17 ;
		DummySVON.quicksort (SVEmpty, d, 0, 9) ;
		addTestResult ((d[0] == 2) && (d[1] == 2.5) && (d[2] == 3) && (d[3] == 4) && (d[4] == 7) &&
					   (d[5] == 8) && (d[6] == 8) && (d[7] == 12) && (d[8] == 17) && (d[9] == 22)) ;
		delete[] d ;
	}
}