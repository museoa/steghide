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

int JpegSample::getDctCoeff()
{
	return DctCoeff ;
}

// FIXME - for jpeg samples: implement a bias towards zero in calcDistance and getNearestOppositeNeighbour ?
float JpegSample::calcDistance (CvrStgSample *s)
{
	JpegSample *sample = dynamic_cast<JpegSample*> (s) ;
	assert (sample != NULL) ;
	return (abs (((float) DctCoeff) - ((float) sample->getDctCoeff()))) ;
}

// FIXME - it is assumed that MAX_INT and MIN_INT are never touched
CvrStgSample *JpegSample::getNearestOppositeNeighbour()
{
	CvrStgSample *retval = NULL ;
	if (RndSrc.getBit()) {
		retval = (CvrStgSample *) new JpegSample (DctCoeff - 1) ;
	}
	else {
		retval = (CvrStgSample *) new JpegSample (DctCoeff + 1) ;
	}
	return retval ;
}
