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
#include "bmpfile.h"
#include "bmpsample.h"

//
// class BmpPaletteSample
//
BmpPaletteSample::BmpPaletteSample (CvrStgFile *f, unsigned char i)
	: CvrStgSample(f), Index(i)
{
	BmpFile *bmpfile = dynamic_cast<BmpFile*> (f) ;
	assert (f != NULL) ;
	unsigned short bitcount = bmpfile->getBitCount() ;
	unsigned char maxindex = 1 ;
	for (unsigned short i = 0 ; i < bitcount ; i++) {
		maxindex *= 2 ;
	}
	maxindex-- ;

	MaxIndex = maxindex ;
	MinIndex = 0 ;

	assert (MinIndex <= Index) ;
	assert (Index <= MaxIndex) ;
}

Bit BmpPaletteSample::getBit()
{
	return ((Bit) (Index & 1)) ;
}

float BmpPaletteSample::calcDistance (CvrStgSample *s)
{
	BmpPaletteSample *sample = dynamic_cast<BmpPaletteSample*> (s) ;
	assert (sample != NULL) ;
	return (abs (((float) Index) - ((float) sample->getIndex()))) ;
}

CvrStgSample *BmpPaletteSample::getNearestOppositeNeighbour()
{
	int n_index = 0 ;
	if (Index == MinIndex) {
		n_index = MinIndex + 1 ;
	}
	else if (Index == MaxIndex) {
		n_index = MaxIndex - 1 ;
	}
	else {
		if (RndSrc.getBit()) {
			n_index = Index - 1 ;
		}
		else {
			n_index = Index + 1 ;
		}
	}
	return ((CvrStgSample *) new BmpPaletteSample (getFile(), n_index)) ;
}

unsigned char BmpPaletteSample::getIndex()
{
	return Index ;
}

//
// class BmpRGBSample
//
Bit BmpRGBSample::getBit()
{
	return ((Bit) ((Red & 1) ^ (Green & 1) ^ (Blue & 1))) ;
}

float BmpRGBSample::calcDistance (CvrStgSample *s)
{
	BmpRGBSample *sample = dynamic_cast<BmpRGBSample*> (s) ;
	assert (sample != NULL) ;
	float dr = getRed() - sample->getRed() ;
	float dg = getGreen() - sample->getGreen() ;
	float db = getBlue() - sample->getBlue() ;
	return (sqrt (dr*dr + dg*dg + db*db)) ;
}

// FIXME - in 255/128/128 (and similar samples) the neighbours with distance 1
// are not chosen with equal probability - P(254/128/128) = 1/3 whereas
// P(255/127/128) = 1/6
CvrStgSample *BmpRGBSample::getNearestOppositeNeighbour()
{
	unsigned char n_red = Red, n_green = Green, n_blue = Blue ;
	unsigned long rnd = RndSrc.getValue(3) ;
	switch (rnd) {
		case 0: {
			n_red = getNearestOppositeComponent (Red) ;
		break ; }

		case 1: {
			n_green = getNearestOppositeComponent (Green) ;
		break ; }

		case 2: {
			n_blue = getNearestOppositeComponent (Blue) ;
		break ; }
	}
	return ((CvrStgSample*) new BmpRGBSample (getFile(), n_red, n_green, n_blue)) ;
}

unsigned char BmpRGBSample::getNearestOppositeComponent (unsigned char c)
{
	if (c == 0x00) {
		c = 0x01 ;
	}
	else if (c == 0xFF) {
		c = 0xFE ;
	}
	else {
		if (RndSrc.getBit()) {
			c-- ;
		}
		else {
			c++ ;
		}
	}
	return c ;
}

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

