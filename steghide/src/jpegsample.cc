/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@teleweb.at>
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
#include "jpegsample.h"

JpegSample::JpegSample (CvrStgFile *f, int c)
	: CvrStgSample(f), DctCoeff (c)
{
	int dctcoeff = ((DctCoeff >= 0) ? DctCoeff : -DctCoeff) ;
	SBit = (Bit) (dctcoeff % 2) ;
	Key = (unsigned long) DctCoeff ;
}

int JpegSample::getDctCoeff() const
{
	return DctCoeff ;
}

bool JpegSample::isNeighbour (CvrStgSample *s) const
{
	return (calcDistance (s) <= Radius) ;
}

list<CvrStgSample*> *JpegSample::getOppositeNeighbours() const
{
	list<CvrStgSample*> *retval = new list<CvrStgSample*> ;
	retval->push_back ((CvrStgSample *) new JpegSample (getFile(), DctCoeff - 1)) ;
	retval->push_back ((CvrStgSample *) new JpegSample (getFile(), DctCoeff + 1)) ;
	return retval ;
}

// FIXME - it is assumed that the maximum and the minimum values are never touched (also in getOppositeNeighbours)
CvrStgSample *JpegSample::getNearestOppositeSample() const
{
	int n_coeff = 0 ;
	if (RndSrc.getBool()) {
		n_coeff = DctCoeff - 1 ;
	}
	else {
		n_coeff = DctCoeff + 1 ;
	}
	return ((CvrStgSample *) new JpegSample (getFile(), n_coeff)) ;
}

// FIXME - for jpeg samples: implement a bias towards zero in calcDistance and getNearestOppositeSample ?
float JpegSample::calcDistance (CvrStgSample *s) const
{
	JpegSample *sample = dynamic_cast<JpegSample*> (s) ;
	assert (sample != NULL) ;
	return (abs (((float) DctCoeff) - ((float) sample->getDctCoeff()))) ;
}

float JpegSample::Radius = 1.0 ;
