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

#include "BmpRGBSampleValue.h"

BmpRGBSampleValue::BmpRGBSampleValue (const CvrStgFile* f, BYTE r, BYTE g, BYTE b)
	: BmpSampleValue(f), Color(r, g, b)
{
	SBit = (BIT) ((r & 1) ^ (g & 1) ^ (b & 1)) ;
	Key = ((unsigned long) r << 16) | ((unsigned long) g << 8) | ((unsigned long) b) ;
}

BmpRGBSampleValue::BmpRGBSampleValue (const CvrStgFile* f, RGBTriple t)
	: BmpSampleValue(f), Color(t)
{
	SBit = (BIT) ((t.Red & 1) ^ (t.Green & 1) ^ (t.Blue & 1)) ;
	Key = ((unsigned long) t.Red << 16) | ((unsigned long) t.Green << 8) | ((unsigned long) t.Blue) ;
}

SampleValue *BmpRGBSampleValue::getNearestOppositeSampleValue () const
{
	std::vector<RGBTriple> candidates ;

	addNOSCandidate (candidates, plus(Color.Red, 1), Color.Green, Color.Blue) ;
	addNOSCandidate (candidates, minus(Color.Red, 1), Color.Green, Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, plus(Color.Green, 1), Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, minus(Color.Green, 1), Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, Color.Green, plus(Color.Blue, 1)) ;
	addNOSCandidate (candidates, Color.Red, Color.Green, minus(Color.Blue, 1)) ;

	unsigned int rnd = (unsigned int) RndSrc.getValue (candidates.size()) ;
	return ((SampleValue*) new BmpRGBSampleValue (getFile(), candidates[rnd])) ;
}

void BmpRGBSampleValue::addNOSCandidate (std::vector<RGBTriple>& cands, BYTE r, BYTE g, BYTE b) const
{
	RGBTriple cand (r, g, b) ;
	if (cand != Color) {
		cands.push_back (cand) ;
	}
}

BYTE BmpRGBSampleValue::minus (BYTE a, BYTE b) const
{
	BYTE retval = 0 ;
	if (a > b) {
		retval = a - b ;
	}
	return retval ;
}

BYTE BmpRGBSampleValue::plus (BYTE a, BYTE b) const
{
	unsigned int sum = a + b ;
	return ((sum <= 255) ? sum : 255) ;
}
