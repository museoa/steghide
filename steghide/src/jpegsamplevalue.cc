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

#include <cstdlib>
#include <cmath>

#include "common.h"
#include "jpegsamplevalue.h"

JpegSampleValue::JpegSampleValue (const CvrStgFile* f, int c)
	: SampleValue(f), DctCoeff (c)
{
	int dctcoeff = ((DctCoeff >= 0) ? DctCoeff : -DctCoeff) ;
	SBit = (Bit) (dctcoeff % 2) ;
	Key = (unsigned long) DctCoeff ;
}

int JpegSampleValue::getDctCoeff() const
{
	return DctCoeff ;
}

bool JpegSampleValue::isNeighbour (const SampleValue *s) const
{
	return (calcDistance (s) <= Radius) ;
}

// FIXME - it is assumed that the maximum and the minimum values are never touched (also in getOppositeNeighbours)
SampleValue *JpegSampleValue::getNearestOppositeSampleValue() const
{
	int n_coeff = 0 ;
	if (RndSrc.getBool()) {
		n_coeff = DctCoeff - 1 ;
	}
	else {
		n_coeff = DctCoeff + 1 ;
	}
	return ((SampleValue *) new JpegSampleValue (getFile(), n_coeff)) ;
}

float JpegSampleValue::calcDistance (const SampleValue *s) const
{
	const JpegSampleValue *sample = dynamic_cast<const JpegSampleValue*> (s) ;
	assert (sample != NULL) ;
	return (fabs (((float) DctCoeff) - ((float) sample->getDctCoeff()))) ;
}

float JpegSampleValue::getRadius () const
{
	return Radius ;
}

float JpegSampleValue::Radius = JpegSampleValue::DefaultRadius ;
