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
#include "common.h"

/**
 * \class BmpRGBSampleValue
 * \brief a sample in a bmp rgb (i.e. 24-bit) file
 **/
class BmpRGBSampleValue : public BmpSampleValue {
	public:
	BmpRGBSampleValue (BYTE r, BYTE g, BYTE b) ;
	BmpRGBSampleValue (RGBTriple t) ;

	SampleValue* getNearestOppositeSampleValue (void) const ;

	BYTE getRed (void) const { return Color.Red ; } ;
	BYTE getGreen (void) const { return Color.Green ; } ;
	BYTE getBlue (void) const { return Color.Blue ; } ;

	private:
	RGBTriple Color ;

	/**
	 * add the BYTEs a and b
	 * \return min(255, a + b)
	 **/
	BYTE plus (BYTE a, BYTE b) const ;

	/**
	 * substract the BYTE b from the BYTE a
	 * \return max(0, a - b)
	 **/
	BYTE minus (BYTE a, BYTE b) const ;

	/**
	 * add a candidate for the nearest opposite sample to cands
	 **/
	void addNOSCandidate (std::vector<RGBTriple>& cands, BYTE r, BYTE g, BYTE b) const ;
} ;

#endif // ndef SH_BMPRGBSAMPLEVALUE_H
