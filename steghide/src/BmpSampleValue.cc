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

#include <cmath>

#include "BmpSampleValue.h"
#include "common.h"

BmpSampleValue::BmpSampleValue (void)
		: SampleValue()
{
	if (Radius == 0.0) {
		Radius = getRadius() ;
		RadiusSquared = Radius * Radius ;
	}
}

BmpSampleValue::BmpSampleValue (const CvrStgFile* f)
		: SampleValue (f)
{
	if (Radius == 0.0) {
		Radius = getRadius() ;
		RadiusSquared = Radius * Radius ;
	}
}

bool BmpSampleValue::isNeighbour (const SampleValue *s) const
{
	const BmpSampleValue *sample = dynamic_cast<const BmpSampleValue*> (s) ;
	myassert (sample) ;
	int dr = (int) getRed() - (int) sample->getRed() ;
	int dg = (int) getGreen() - (int) sample->getGreen() ;
	int db = (int) getBlue() - (int) sample->getBlue() ;
	return ((dr*dr + dg*dg + db*db) <= RadiusSquared) ;
}

float BmpSampleValue::calcDistance (const SampleValue *s) const
{
	const BmpSampleValue *sample = dynamic_cast<const BmpSampleValue*> (s) ;
	myassert (sample) ;
	float dr = (float) getRed() - (float) sample->getRed() ;
	float dg = (float) getGreen() - (float) sample->getGreen() ;
	float db = (float) getBlue() - (float) sample->getBlue() ;
	return (sqrt (dr*dr + dg*dg + db*db)) ;
}

float BmpSampleValue::getRadius () const
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

float BmpSampleValue::Radius = 0.0 ;
float BmpSampleValue::RadiusSquared = 0.0 ;
