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

/**
 * \class BmpSample
 * \brief an (abstract) sample in a bmp file
 **/
class BmpSample : public CvrStgSample {
	public:
	BmpSample (void) ;
	BmpSample (CvrStgFile *f) ;

	bool isNeighbour (CvrStgSample *s) const ;
	float calcDistance (CvrStgSample *s) const ;

	/**
	 * get the red color component
	 **/
	virtual unsigned char getRed (void) const = 0 ;
	
	/**
	 * get the green color component
	 **/
	virtual unsigned char getGreen (void) const = 0 ;
	
	/**
	 * get the blue color component
	 **/
	virtual unsigned char getBlue (void) const = 0 ;

	protected:
	static const float DefaultRadius = 20.0 ;
	static float Radius ;
	static float RadiusSquared ;

	private:
	void setRadius (void) ;
} ;

/**
 * \class BmpPaletteSample
 * \brief a sample in a bmp palette (i.e. 1-,4- or 8-bit) file
 **/
class BmpPaletteSample : public BmpSample {
	public:
	BmpPaletteSample (void)
		: BmpSample() {} ;
	BmpPaletteSample (CvrStgFile *f, unsigned char i) ;

	list<CvrStgSample*> *getOppositeNeighbours (void) const ;
	CvrStgSample* getNearestOppositeSample (void) const ;

	unsigned char getIndex (void) const ;
	unsigned char getRed (void) const ;
	unsigned char getGreen (void) const ;
	unsigned char getBlue (void) const ;

	private:
	ColorPalette *Palette ;
	unsigned char Index ;
} ;

/**
 * \class BmpRGBSample
 * \brief a sample in a bmp rgb (i.e. 24-bit) file
 **/
class BmpRGBSample : public BmpSample {
	public:
	BmpRGBSample (void)
		: BmpSample() {} ;
	BmpRGBSample (CvrStgFile *f, unsigned char r, unsigned char g, unsigned char b) ;
	BmpRGBSample (CvrStgFile *f, RGBTriple t) ;

	list<CvrStgSample*> *getOppositeNeighbours (void) const ;
	CvrStgSample* getNearestOppositeSample (void) const ;

	unsigned char getRed (void) const ;
	unsigned char getGreen (void) const ;
	unsigned char getBlue (void) const ;

	private:
	RGBTriple Color ;

	struct RGBVector {
		RGBVector (int r, int g, int b)
			: Red(r),Green(g),Blue(b) {} ;

		int Red ;
		int Green ;
		int Blue ;
	} ;

	/**
	 * contains all RGBVectors v where Color + v is the RGBTriple of an opposite neighbour
	 * (provided Color + v still is in the RGB Cube)
	 **/
	static vector<RGBVector> OppNeighDeltas ;

	/** 
	 * fill the OppNeighDeltas vector
	 **/
	void calcOppNeighDeltas (void) ;

	/**
	 * add the unsigned chars a and b
	 * \return min(255, a + b)
	 **/
	unsigned char plus (unsigned char a, unsigned char b) const ;

	/**
	 * substract the unsigned char b from the unsigned char a
	 * \return max(0, a - b)
	 **/
	unsigned char minus (unsigned char a, unsigned char b) const ;

	/**
	 * add a candidate for the nearest opposite sample to cands
	 **/
	void BmpRGBSample::addNOSCandidate (vector<RGBTriple>& cands, unsigned char r, unsigned char g, unsigned char b) const ;
} ;

#endif // ndef SH_BMPSAMPLE_H
