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
#include <cstdlib>

#include "common.h"
#include "wavfile.h"
#include "wavsamplevalue.h"

// TODO - ? derive WavPCMsmallSampleValue and WavPCMlargeSampleValue from WavPCMSampleValueValue

WavPCMSampleValue::WavPCMSampleValue (CvrStgFile *f, int v)
	: SampleValue(f), Value(v)
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
	assert (Value <= MaxValue) ;

	SBit = (Bit) (Value & 1) ;
	Key = (unsigned long) Value ;
	if (Radius == 0.0) {
		Radius = getRadius() ;
	}
}

bool WavPCMSampleValue::isNeighbour (const SampleValue *s) const
{
	return (calcDistance (s) <= Radius) ;
}

SampleValue *WavPCMSampleValue::getNearestOppositeSampleValue () const
{
	int n_value = 0 ;
	if (Value == MinValue) {
		n_value = MinValue + 1 ;
	}
	else if (Value == MaxValue) {
		n_value = MaxValue - 1 ;
	}
	else {
		if (RndSrc.getBool()) {
			n_value = Value - 1 ;
		}
		else {
			n_value = Value + 1 ;
		}
	}
	return ((SampleValue *) new WavPCMSampleValue (getFile(), n_value)) ;
}

float WavPCMSampleValue::calcDistance (const SampleValue *s) const
{
	const WavPCMSampleValue *sample = dynamic_cast<const WavPCMSampleValue*> (s) ;
	assert (sample != NULL) ;
	return (abs ((float) Value - (float) sample->getValue())) ;
}

int WavPCMSampleValue::getValue() const
{
	return Value ;
}

float WavPCMSampleValue::getRadius () const
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

float WavPCMSampleValue::Radius = 0.0 ;
