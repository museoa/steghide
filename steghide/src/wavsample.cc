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

#include <cmath>
#include <cstdlib>

#include "common.h"
#include "wavsample.h"

//
// class WavPCMsmallSample
//
float WavPCMsmallSample::calcDistance (CvrStgSample *s)
{
	WavPCMsmallSample *sample = dynamic_cast<WavPCMsmallSample*> (s) ;
	assert (sample != NULL) ;
	return (abs ((float) Value - (float) sample->getValue())) ;
}

unsigned char WavPCMsmallSample::getValue()
{
	return Value ;
}

CvrStgSample *WavPCMsmallSample::getNearestOppositeNeighbour()
{
	CvrStgSample *retval = NULL ;
	if (Value == 0) {
		retval = (CvrStgSample *) new WavPCMsmallSample (1) ;
	}
	else if (Value == 255) {
		retval = (CvrStgSample *) new WavPCMsmallSample (254) ;
	}
	else {
		if (RndSrc.getBit()) {
			retval = (CvrStgSample *) new WavPCMsmallSample (Value - 1) ;
		}
		else {
			retval = (CvrStgSample *) new WavPCMsmallSample (Value + 1) ;
		}
	}
	return retval ;
}

//
// class WavPCMbigSample
//
float WavPCMbigSample::calcDistance (CvrStgSample *s)
{
	WavPCMbigSample *sample = dynamic_cast<WavPCMbigSample*> (s) ;
	assert (sample != NULL) ;
	return (abs ((float) Value - (float) sample->getValue())) ;
}

int WavPCMbigSample::getValue()
{
	return Value ;
}

// FIXME - it is assumed that the maximum and minimum are never touched
CvrStgSample *WavPCMbigSample::getNearestOppositeNeighbour()
{
	CvrStgSample *retval = NULL ;
	if (RndSrc.getBit()) {
		retval = (CvrStgSample *) new WavPCMbigSample (Value - 1) ;
	}
	else {
		retval = (CvrStgSample *) new WavPCMbigSample (Value + 1) ;
	}
	return retval ;
}
