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
#include "bmpsample.h"

//
// class BmpSample
//
float BmpSample::calcDistance (CvrStgSample *s)
{
	BmpSample *sample = dynamic_cast<BmpSample*> (s) ;
	assert (sample != NULL) ;
	float dr = getRed() - sample->getRed() ;
	float dg = getGreen() - sample->getGreen() ;
	float db = getBlue() - sample->getBlue() ;
	return (sqrt (dr*dr + dg*dg + db*db)) ;
}

//
// class BmpPaletteSample
//
unsigned char BmpPaletteSample::getIndex()
{
	return Index ;
}

unsigned char BmpPaletteSample::getRed()
{
	return Red ;
}

unsigned char BmpPaletteSample::getGreen()
{
	return Green ;
}

unsigned char BmpPaletteSample::getBlue()
{
	return Blue ;
}

//
// class BmpRGBSample
//
unsigned char BmpRGBSample::getRed()
{
	return Red ;
}

unsigned char BmpRGBSample::getGreen()
{
	return Green ;
}

unsigned char BmpRGBSample::getBlue()
{
	return Blue ;
}
