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

#include <cfloat>
#include <cmath>
#include <cstdlib>

#include "bmpfile.h"
#include "bmpsamplevalue.h"
#include "common.h"

//
// class BmpSampleValue
//
BmpSampleValue::BmpSampleValue (void)
		: SampleValue()
{
	if (Radius == 0.0) {
		Radius = getRadius() ;
		RadiusSquared = Radius * Radius ;
	}
}

BmpSampleValue::BmpSampleValue (CvrStgFile *f)
		: SampleValue (f)
{
	if (Radius == 0.0) {
		Radius = getRadius() ;
		RadiusSquared = Radius * Radius ;
	}
}

bool BmpSampleValue::isNeighbour (SampleValue *s) const
{
	assert (s != NULL) ; // FIXME - delme
	BmpSampleValue *sample = dynamic_cast<BmpSampleValue*> (s) ;
	assert (sample != NULL) ;
	int dr = (int) getRed() - (int) sample->getRed() ;
	int dg = (int) getGreen() - (int) sample->getGreen() ;
	int db = (int) getBlue() - (int) sample->getBlue() ;
	return ((dr*dr + dg*dg + db*db) <= RadiusSquared) ;
}

float BmpSampleValue::calcDistance (SampleValue *s) const
{
	BmpSampleValue *sample = dynamic_cast<BmpSampleValue*> (s) ;
	assert (sample != NULL) ;
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

//
// class BmpPaletteSampleValue
//
BmpPaletteSampleValue::BmpPaletteSampleValue (CvrStgFile *f, unsigned char i)
	: BmpSampleValue(f), Index(i)
{
	BmpFile *bmpfile = dynamic_cast<BmpFile*> (f) ;
	assert (bmpfile != NULL) ;
	Palette = bmpfile->getPalette() ;
	SBit = (Bit) (Index & 1) ;
	Key = ((unsigned long) getIndex() << 24) | ((unsigned long) getRed() << 16) |
		  ((unsigned long) getGreen() << 8) | ((unsigned long) getBlue()) ;
}

SampleValue *BmpPaletteSampleValue::getNearestOppositeSampleValue () const
{
	RGBTriple thistriple = Palette->getEntry (Index) ;

	unsigned int idx_nearest = 0 ;
	float mindist = FLT_MAX ;
	unsigned int n = Palette->getSize() ;
	for (unsigned int i = ((getBit() == 1) ? 0 : 1) ; i < n ; i += 2) {
		RGBTriple curtriple = Palette->getEntry (i) ;
		float curdist = thistriple.calcDistance (curtriple) ;
		if (curdist < mindist) {
			mindist = curdist ;
			idx_nearest = i ;
		}
	}

	return ((SampleValue *) new BmpPaletteSampleValue (getFile(), idx_nearest)) ;
}

unsigned char BmpPaletteSampleValue::getIndex() const
{
	return Index ;
}

unsigned char BmpPaletteSampleValue::getRed() const
{
	return Palette->getEntry(Index).Red ;
}

unsigned char BmpPaletteSampleValue::getGreen() const
{
	return Palette->getEntry(Index).Green ;
}

unsigned char BmpPaletteSampleValue::getBlue() const
{
	return Palette->getEntry(Index).Blue ;
}

//
// class BmpRGBSampleValue
//
BmpRGBSampleValue::BmpRGBSampleValue (CvrStgFile *f, unsigned char r, unsigned char g, unsigned char b)
	: BmpSampleValue(f), Color(r, g, b)
{
	SBit = (Bit) ((r & 1) ^ (g & 1) ^ (b & 1)) ;
	Key = ((unsigned long) r << 16) | ((unsigned long) g << 8) | ((unsigned long) b) ;
}

BmpRGBSampleValue::BmpRGBSampleValue (CvrStgFile *f, RGBTriple t)
	: BmpSampleValue(f), Color(t)
{
	SBit = (Bit) ((t.Red & 1) ^ (t.Green & 1) ^ (t.Blue & 1)) ;
	Key = ((unsigned long) t.Red << 16) | ((unsigned long) t.Green << 8) | ((unsigned long) t.Blue) ;
}

SampleValue *BmpRGBSampleValue::getNearestOppositeSampleValue () const
{
	vector<RGBTriple> candidates ;

	addNOSCandidate (candidates, plus(Color.Red, 1), Color.Green, Color.Blue) ;
	addNOSCandidate (candidates, minus(Color.Red, 1), Color.Green, Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, plus(Color.Green, 1), Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, minus(Color.Green, 1), Color.Blue) ;
	addNOSCandidate (candidates, Color.Red, Color.Green, plus(Color.Blue, 1)) ;
	addNOSCandidate (candidates, Color.Red, Color.Green, minus(Color.Blue, 1)) ;

	unsigned int rnd = (unsigned int) RndSrc.getValue (candidates.size()) ;
	return ((SampleValue*) new BmpRGBSampleValue (getFile(), candidates[rnd])) ;
}

void BmpRGBSampleValue::addNOSCandidate (vector<RGBTriple>& cands, unsigned char r, unsigned char g, unsigned char b) const
{
	RGBTriple cand (r, g, b) ;
	if (cand != Color) {
		cands.push_back (cand) ;
	}
}

unsigned char BmpRGBSampleValue::getRed() const
{
	return Color.Red ;
}

unsigned char BmpRGBSampleValue::getGreen() const
{
	return Color.Green ;
}

unsigned char BmpRGBSampleValue::getBlue() const
{
	return Color.Blue ;
}

unsigned char BmpRGBSampleValue::minus (unsigned char a, unsigned char b) const
{
	unsigned char retval = 0 ;
	if (a > b) {
		retval = a - b ;
	}
	return retval ;
}

unsigned char BmpRGBSampleValue::plus (unsigned char a, unsigned char b) const
{
	unsigned int sum = a + b ;
	return ((sum <= 255) ? sum : 255) ;
}
