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

#include "ausample.h"
#include "common.h"
#include "randomsource.h"

unsigned char AuSample::getValue () const
{
	return Value ;
}

bool AuSample::isNeighbour (CvrStgSample *s) const
{
	return (calcDistance (s) <= Radius) ;
}

list<CvrStgSample*> *AuSample::getOppositeNeighbours() const
{
	// FIXME - in this function it is assumed that the neighbourhood radius is 1
	list<CvrStgSample*> *retval = new list<CvrStgSample*> ;
	if (Value != 0) {
		retval->push_back ((CvrStgSample *) new AuSample (getFile(), Value - 1)) ;
	}
	if (Value != 255) {
		retval->push_back ((CvrStgSample *) new AuSample (getFile(), Value + 1)) ;
	}
	return retval ;
}

CvrStgSample *AuSample::getNearestOppositeSample() const
{
	unsigned char n_value = 0 ;
	if (Value == 0) {
		n_value = 1 ;
	}
	else if (Value == 255) {
		n_value = 254 ;
	}
	else {
		if (RndSrc.getBool()) {
			n_value = Value - 1 ;
		}
		else {
			n_value = Value + 1 ;
		}
	}
	return ((CvrStgSample *) new AuSample (getFile(), n_value)) ;
}

float AuSample::calcDistance (CvrStgSample *s) const
{
	AuSample *sample = dynamic_cast<AuSample*> (s) ;
	assert (sample != NULL) ;
	return (abs (((float) Value) - ((float) sample->getValue()))) ;
}

float AuSample::Radius = 1.0 ;
