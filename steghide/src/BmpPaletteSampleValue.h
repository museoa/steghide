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

#ifndef SH_BMPPALETTESAMPLEVALUE_H
#define SH_BMPPALETTESAMPLEVALUE_H

#include "BmpSampleValue.h"

class ColorPalette ;

/**
 * \class BmpPaletteSampleValue
 * \brief a sample in a bmp palette (i.e. 1-,4- or 8-bit) file
 **/
class BmpPaletteSampleValue : public BmpSampleValue {
	public:
	BmpPaletteSampleValue (void)
		: BmpSampleValue() {} ;
	BmpPaletteSampleValue (const CvrStgFile* f, unsigned char i) ;

	SampleValue* getNearestOppositeSampleValue (void) const ;

	unsigned char getIndex (void) const ;
	unsigned char getRed (void) const ;
	unsigned char getGreen (void) const ;
	unsigned char getBlue (void) const ;

	private:
	ColorPalette* Palette ;
	unsigned char Index ;
} ;

#endif // ndef SH_BMPPALETTESAMPLEVALUE_H
