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
#include "wavfile.h"
#include "wavsample.h"

//DEBUG
WavPCMSample::WavPCMSample (CvrStgFile *f, int v)
	: CvrStgSample(f), Value(v)
{
	WavFile *wavfile = dynamic_cast<WavFile*> (f) ;
	assert (wavfile != NULL) ;
	unsigned short samplesize = wavfile->getBitsPerSample() ;

	int maxvalue = 1 ;
	int minvalue = 1 ;
	if ((samplesize >= 1) && (samplesize <= 8)) {
		// maxvalue is 2^samplesize
		for (unsigned short i = 0 ; i < samplesize ; i++) {
			maxvalue *= 2 ;
		}

		minvalue = 0 ;
	}
	else {
		// maxvalue is (2^(samplesize - 1)) - 1
		for (unsigned short i = 0 ; i < (samplesize - 1) ; i++) {
			maxvalue *= 2 ;
		}
		maxvalue-- ;

		// minvalue is -(2^(SampleSize - 1))
		for (unsigned short i = 0 ; i < (samplesize - 1) ; i++) {
			minvalue *= 2 ;
		}
		minvalue = -minvalue ;
	}

	MaxValue = maxvalue ;
	MinValue = minvalue ;

	assert (MinValue <= Value) ;
	if (Value > MaxValue) {
		cerr << "samplesize: " << samplesize << endl ;
		cerr << "Value: " << Value << endl ;
		cerr << "MaxValue: " << MaxValue << endl ;
	}
	assert (Value <= MaxValue) ;
}

Bit WavPCMSample::getBit()
{
	return ((Bit) (Value & 1)) ;
}
	
float WavPCMSample::calcDistance (CvrStgSample *s)
{
	WavPCMSample *sample = dynamic_cast<WavPCMSample*> (s) ;
	assert (sample != NULL) ;
	return (abs ((float) Value - (float) sample->getValue())) ;
}

int WavPCMSample::getValue()
{
	return Value ;
}

CvrStgSample *WavPCMSample::getNearestOppositeNeighbour()
{
	int n_value = 0 ;
	if (Value == MinValue) {
		n_value = MinValue + 1 ;
	}
	else if (Value == MaxValue) {
		n_value = MaxValue - 1 ;
	}
	else {
		if (RndSrc.getBit()) {
			n_value = Value - 1 ;
		}
		else {
			n_value = Value + 1 ;
		}
	}
	return ((CvrStgSample *) new WavPCMSample (getFile(), n_value)) ;
}
