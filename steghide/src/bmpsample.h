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

#ifndef SH_BMPSAMPLE_H
#define SH_BMPSAMPLE_H

#include "cvrstgsample.h"
#include "common.h"

class BmpSample : public CvrStgSample {
	public:
	virtual unsigned char getRed (void) = 0 ;
	virtual unsigned char getGreen (void) = 0 ;
	virtual unsigned char getBlue (void) = 0 ;

	float calcDistance (CvrStgSample *s) ;
} ;

class BmpPaletteSample : public BmpSample {
	public:
	BmpPaletteSample (void) {} ;
	BmpPaletteSample (unsigned char i, unsigned char r, unsigned char g, unsigned char b) : Index(i), Red(r), Green(g), Blue(b) {} ;

	CvrStgSample* getNearestOppositeNeighbour (void) ;

	unsigned char getIndex (void) ;
	unsigned char getRed (void) ;
	unsigned char getGreen (void) ;
	unsigned char getBlue (void) ;

	private:
	unsigned char Index ;
	// FIXME - take rgb values from a palette
	unsigned char Red ;
	unsigned char Green ;
	unsigned char Blue ;
} ;

class BmpRGBSample : public BmpSample {
	public:
	BmpRGBSample (void) {} ;
	BmpRGBSample (unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {} ;

	CvrStgSample* getNearestOppositeNeighbour (void) ;

	unsigned char getRed (void) ;
	unsigned char getGreen (void) ;
	unsigned char getBlue (void) ;

	private:
	unsigned char Red ;
	unsigned char Green ;
	unsigned char Blue ;
} ;

#endif // ndef SH_BMPSAMPLE_H
