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

#ifndef SH_BMPSAMPLE_H
#define SH_BMPSAMPLE_H

#include "samplevalue.h"
#include "colorpalette.h"
#include "common.h"

/**
 * \class BmpSampleValue
 * \brief an (abstract) sample value in a bmp file
 **/
class BmpSampleValue : public SampleValue {
	public:
	BmpSampleValue (void) ;
	BmpSampleValue (CvrStgFile *f) ;

	bool isNeighbour (SampleValue *s) const ;
	float calcDistance (SampleValue *s) const ;

	float getRadius (void) const ;

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
} ;

/**
 * \class BmpPaletteSampleValue
 * \brief a sample in a bmp palette (i.e. 1-,4- or 8-bit) file
 **/
class BmpPaletteSampleValue : public BmpSampleValue {
	public:
	BmpPaletteSampleValue (void)
		: BmpSampleValue() {} ;
	BmpPaletteSampleValue (CvrStgFile *f, unsigned char i) ;

	SampleValue* getNearestOppositeSampleValue (void) const ;

	unsigned char getIndex (void) const ;
	unsigned char getRed (void) const ;
	unsigned char getGreen (void) const ;
	unsigned char getBlue (void) const ;

	private:
	ColorPalette *Palette ;
	unsigned char Index ;
} ;

/**
 * \class BmpRGBSampleValue
 * \brief a sample in a bmp rgb (i.e. 24-bit) file
 **/
class BmpRGBSampleValue : public BmpSampleValue {
	public:
	BmpRGBSampleValue (void)
		: BmpSampleValue() {} ;
	BmpRGBSampleValue (CvrStgFile *f, unsigned char r, unsigned char g, unsigned char b) ;
	BmpRGBSampleValue (CvrStgFile *f, RGBTriple t) ;

	SampleValue* getNearestOppositeSampleValue (void) const ;

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
	void BmpRGBSampleValue::addNOSCandidate (vector<RGBTriple>& cands, unsigned char r, unsigned char g, unsigned char b) const ;
} ;

#endif // ndef SH_BMPSAMPLE_H
