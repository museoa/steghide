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

class BmpPaletteSample : public CvrStgSample {
	public:
	BmpPaletteSample (void) : CvrStgSample(NULL) {} ;
	BmpPaletteSample (CvrStgFile *f, unsigned char i) ;

	Bit getBit (void) const ;
	bool isNeighbour (CvrStgSample *s) const ;
	list<CvrStgSample*> *getOppositeNeighbours (void) const ;
	CvrStgSample* getNearestOppositeNeighbour (void) const ;
	float calcDistance (CvrStgSample *s) const ;
	unsigned long getKey (void) const ;

	unsigned char getIndex (void) const ;

	private:
	unsigned char Index ;
	unsigned char MaxIndex ;
	unsigned char MinIndex ;
} ;

class BmpRGBSample : public CvrStgSample {
	public:
	BmpRGBSample (void) : CvrStgSample(NULL) {} ;
	BmpRGBSample (CvrStgFile *f, unsigned char r, unsigned char g, unsigned char b) : CvrStgSample (f), Red(r), Green(g), Blue(b) {} ;

	Bit getBit (void) const ;
	bool isNeighbour (CvrStgSample *s) const ;
	list<CvrStgSample*> *getOppositeNeighbours (void) const ;
	CvrStgSample* getNearestOppositeNeighbour (void) const ;
	float calcDistance (CvrStgSample *s) const ;
	unsigned long getKey (void) const ;

	unsigned char getRed (void) const ;
	unsigned char getGreen (void) const ;
	unsigned char getBlue (void) const ;

	private:
	unsigned char Red ;
	unsigned char Green ;
	unsigned char Blue ;

	unsigned char getNearestOppositeComponent (unsigned char c) const ;
	unsigned char plus (unsigned char a, unsigned char b) const ;
	unsigned char minus (unsigned char a, unsigned char b) const ;
} ;

#endif // ndef SH_BMPSAMPLE_H
