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

#ifndef SH_WAVSAMPLE_H
#define SH_WAVSAMPLE_H

#include<functional>

#include "samplevalue.h"
#include "common.h"

class WavPCMSampleValue : public SampleValue {
	public:
	WavPCMSampleValue (void) : SampleValue(NULL) {} ;
	WavPCMSampleValue (CvrStgFile *f, int v) ;

	bool isNeighbour (const SampleValue *s) const ;
	SampleValue* getNearestOppositeSampleValue (void) const ;
	float calcDistance (const SampleValue *s) const ;

	float getRadius (void) const ;

	int getValue (void) const ;

	private:
	/**
	 * every pair of wav samples whose distance is smaller than this constant are neighbours
	 **/
	static const float DefaultRadius = 1.0 ;
	static float Radius ;

	int Value ;
	int MaxValue ;
	int MinValue ;
} ;

class WavPCMSmaller : public binary_function<WavPCMSampleValue*,WavPCMSampleValue*,bool> {
	public:
	bool operator() (const WavPCMSampleValue *v, const WavPCMSampleValue *w) { return v->getValue() < w->getValue() ; } ;
} ;

#endif // ndef SH_WAVSAMPLE_H