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

#include "BmpFile.h"
#include "BmpPaletteSampleValue.h"

BmpPaletteSampleValue::BmpPaletteSampleValue (const CvrStgFile* f, unsigned char i)
	: BmpSampleValue(f), Index(i)
{
	const BmpFile* bmpfile = dynamic_cast<const BmpFile*> (f) ;
	myassert (bmpfile != NULL) ;
	Palette = bmpfile->getPalette() ;
	SBit = (BIT) (Index & 1) ;
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
