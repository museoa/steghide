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

#ifndef SH_AUDIOSAMPLEVALUE_H
#define SH_AUDIOSAMPLEVALUE_H

#include "SampleValue.h"
#include "common.h"

/**
 * An enumeration containing all audio sample types. This enumeration
 * is used as template parameter for AudioSampleValue and AudioData to
 * identify the correct implementation for a given audio format.
 **/
enum AUDIOSAMPLETYPE {
	/// au 8 Bit mu-law
	AuMuLaw,
	/// au 8 Bit linear pcm
	AuPCM8,
	/// au 16 Bit linear pcm
	AuPCM16,
	/// au 32 Bit linear pcm
	AuPCM32
} ;

/**
 * \class AudioSampleValue
 * \brief a class representing an audio sample
 *
 * The template parameter T is the type of the sample (e.g. SWORD16
 * for 16-Bit signed audio data).
 **/
template<AUDIOSAMPLETYPE Type, class ValueType>
class AudioSampleValue : public SampleValue {
    public:
    AudioSampleValue (const CvrStgFile* f, ValueType v) ;

    ValueType getValue (void) const { return Value ; } ;

	SampleValue* getNearestOppositeSampleValue (void) const ;
    UWORD32 calcDistance (const SampleValue* s) const ;

    private:
    ValueType Value ;
	static const ValueType MinValue ;
	static const ValueType MaxValue ;
    static const UWORD32 DefaultRadius ;
} ;

template<AUDIOSAMPLETYPE Type, class ValueType>
AudioSampleValue<Type,ValueType>::AudioSampleValue (const CvrStgFile* f, ValueType v)
	: SampleValue(f), Value(v)
{
	Key = (UWORD32) (v - MinValue) ;
	SBit = (BIT) (Key % 2) ;
	setRadius (DefaultRadius) ;
}

template<AUDIOSAMPLETYPE Type, class ValueType>
UWORD32 AudioSampleValue<Type,ValueType>::calcDistance (const SampleValue* s) const
{
	const AudioSampleValue<Type,ValueType>* sample = (const AudioSampleValue<Type,ValueType>*) s ;
	/* If s is not a correct AudioSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a correct AudioSampleValue as s anyway. */

    if (sample->Value > Value) {
        return sample->Value - Value ;
    }
    else {
        return Value - sample->Value ;
    }
}

template<AUDIOSAMPLETYPE Type, class ValueType>
SampleValue* AudioSampleValue<Type,ValueType>::getNearestOppositeSampleValue (void) const
{
	ValueType newval ;
	if (Value == MinValue) {
		newval = MinValue + 1 ;
	}
	else if (Value == MaxValue) {
		newval = MaxValue - 1 ;
	}
	else {
		if (RndSrc.getBool()) {
			newval = Value - 1 ;
		}
		else {
			newval = Value + 1 ;
		}
	}
	return ((SampleValue *) new AudioSampleValue<Type,ValueType> (getFile(), newval)) ;
}

#endif // ndef SH_AUDIOSAMPLEVALUE_H
