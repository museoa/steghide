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

// FIXME - make MinIndex and MaxIndex static - this will save some time

bool BmpPaletteSample::isNeighbour (CvrStgSample *s) const
{
	return (calcDistance (s) <= 1.0) ;
}

Bit BmpPaletteSample::getBit() const
{
	return ((Bit) (Index & 1)) ;
}

list<CvrStgSample*> *BmpPaletteSample::getOppositeNeighbours() const
{
	list<CvrStgSample*> *retval = new list<CvrStgSample*> ;
	if (Index != MinIndex) {
		retval->push_back ((CvrStgSample *) new BmpPaletteSample (getFile(), Index - 1)) ;
	}
	if (Index != MaxIndex) {
		retval->push_back ((CvrStgSample *) new BmpPaletteSample (getFile(), Index + 1)) ;
	}
	return retval ;
}

CvrStgSample *BmpPaletteSample::getNearestOppositeNeighbour() const
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

float BmpPaletteSample::calcDistance (CvrStgSample *s) const
{
	BmpPaletteSample *sample = dynamic_cast<BmpPaletteSample*> (s) ;
	assert (sample != NULL) ;
	return (abs (((float) Index) - ((float) sample->getIndex()))) ;
}

unsigned long BmpPaletteSample::getKey() const
{
	return ((unsigned long) Index) ;
}

unsigned char BmpPaletteSample::getIndex() const
{
	return Index ;
}

//
// class BmpRGBSample
//
Bit BmpRGBSample::getBit() const
{
	return ((Bit) ((Red & 1) ^ (Green & 1) ^ (Blue & 1))) ;
}

bool BmpRGBSample::isNeighbour (CvrStgSample *s) const
{
	return (calcDistance (s) <= 1.0) ;
}

list<CvrStgSample*> *BmpRGBSample::getOppositeNeighbours() const
{
	list<CvrStgSample*> *retval = new list<CvrStgSample*> ;

	// for all samples that are in the smallest cube containing all neighbours, test if they are opposite neighbours
	// single component maximum change
	int scmc = 1 /* Neighbourhood radius */ ;
	int r_start = minus(Red, scmc), r_end = plus(Red, scmc) ;
	int g_start = minus(Green, scmc), g_end = plus(Green, scmc) ;
	int b_start = minus(Blue, scmc), b_end = plus(Blue, scmc) ;
	
	for (int r = r_start ; r <= r_end ; r++) {
		for (int g = g_start ; g <= g_end ; g++) {
			for (int b = b_start ; b <= b_end ; b++) {
				if (((r & 1) ^ (g & 1) ^ (b & 1)) != getBit()) {
					// r/g/b is opposite
					float dr = Red - r ;
					float dg = Green - g ;
					float db = Blue - b ;
					if (sqrt (dr*dr + dg*dg + db*db) <= 1.0 /* Neighbourhood radius */) {
						// r/g/b is neighbour
						retval->push_back ((CvrStgSample *) new BmpRGBSample (getFile(), r, g, b)) ;
					}
				}
			}
		}
	}

	return retval ;
}

// FIXME - in 255/128/128 (and similar samples) the neighbours with distance 1
// are not chosen with equal probability - P(254/128/128) = 1/3 whereas
// P(255/127/128) = 1/6
CvrStgSample *BmpRGBSample::getNearestOppositeNeighbour() const
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

float BmpRGBSample::calcDistance (CvrStgSample *s) const
{
	BmpRGBSample *sample = dynamic_cast<BmpRGBSample*> (s) ;
	assert (sample != NULL) ;
	float dr = Red - sample->getRed() ;
	float dg = Green - sample->getGreen() ;
	float db = Blue - sample->getBlue() ;
	return (sqrt (dr*dr + dg*dg + db*db)) ;
}

unsigned long BmpRGBSample::getKey() const
{
	return (((unsigned long) Red << 16) | ((unsigned long) Green << 8) | ((unsigned long) Blue)) ;
}

unsigned char BmpRGBSample::getNearestOppositeComponent (unsigned char c) const
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

unsigned char BmpRGBSample::getRed() const
{
	return Red ;
}

unsigned char BmpRGBSample::getGreen() const
{
	return Green ;
}

unsigned char BmpRGBSample::getBlue() const
{
	return Blue ;
}

unsigned char BmpRGBSample::minus (unsigned char a, unsigned char b) const
{
	unsigned char retval = 0 ;
	if (a > b) {
		retval = a - b ;
	}
	return retval ;
}

unsigned char BmpRGBSample::plus (unsigned char a, unsigned char b) const
{
	unsigned int sum = a + b ;
	return ((sum <= 255) ? sum : 255) ;
}

