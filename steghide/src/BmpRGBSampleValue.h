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

#ifndef SH_BMPRGBSAMPLEVALUE_H
#define SH_BMPRGBSAMPLEVALUE_H

#include "BmpSampleValue.h"
#include "RGBTriple.h"

/**
 * \class BmpRGBSampleValue
 * \brief a sample in a bmp rgb (i.e. 24-bit) file
 **/
class BmpRGBSampleValue : public BmpSampleValue {
	public:
	BmpRGBSampleValue (void)
		: BmpSampleValue() {} ;
	BmpRGBSampleValue (const CvrStgFile* f, unsigned char r, unsigned char g, unsigned char b) ;
	BmpRGBSampleValue (const CvrStgFile* f, RGBTriple t) ;

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
	void addNOSCandidate (std::vector<RGBTriple>& cands, unsigned char r, unsigned char g, unsigned char b) const ;
} ;

#endif // ndef SH_BMPRGBSAMPLEVALUE_H