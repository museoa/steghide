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

#include "SampleValueTest.h"

#include "SampleValue.h"

bool SampleValueTest::areOpposite (SampleValue* s1, SampleValue* s2)
{
	return (s1->getBit() != s2->getBit()) ;
}

bool SampleValueTest::genericTestIsNeighbour (SampleValue* s1, SampleValue* s2, bool res)
{
	return ((s1->isNeighbour(s2) == s2->isNeighbour(s1)) && (s1->isNeighbour(s2) == res)) ;
}

bool SampleValueTest::genericTestDistance (SampleValue* s1, SampleValue* s2, float dist)
{
	return ((s1->calcDistance(s2) == s2->calcDistance(s1)) &&
			(s1->calcDistance(s2) == dist)) ;
}
