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

#include "ausamplevalue.h"
#include "common.h"
#include "randomsource.h"

unsigned char AuSampleValue::getValue () const
{
	return Value ;
}

bool AuSampleValue::isNeighbour (const SampleValue *s) const
{
	return (calcDistance (s) <= Radius) ;
}

SampleValue *AuSampleValue::getNearestOppositeSampleValue() const
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
	return ((SampleValue *) new AuSampleValue (getFile(), n_value)) ;
}

float AuSampleValue::calcDistance (const SampleValue *s) const
{
	const AuSampleValue* sample = dynamic_cast<const AuSampleValue*> (s) ;
	assert (sample != NULL) ;
	return (fabs (((float) Value) - ((float) sample->getValue()))) ;
}

float AuSampleValue::getRadius () const
{
	float retval ;
	if (Args.Radius.is_set()) {
		retval = Args.Radius.getValue() ;
	}
	else {
		retval = DefaultRadius ;
	}
	return retval ;
}

float AuSampleValue::Radius = AuSampleValue::DefaultRadius ;
