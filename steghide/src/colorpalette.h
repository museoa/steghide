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

#ifndef SH_COLORPALETTE_H
#define SH_COLORPALETTE_H

#include <vector>

#include "rgbtriple.h"

/**
 * \class ColorPalette
 * \brief a color palette
 **/
class ColorPalette {
	public:
	ColorPalette (void) ;

	void addEntry (const RGBTriple& rgb) ;
	void addEntry (unsigned char r, unsigned char g, unsigned char b) ;

	unsigned int getSize (void) const ;

	const RGBTriple& getEntry (unsigned int i) const ;
	void setEntry (unsigned int i, const RGBTriple& rgb) ;

	unsigned char getRed (unsigned int i) const ;
	unsigned char getGreen (unsigned int i) const ;
	unsigned char getBlue (unsigned int i) const ;

	void setRed (unsigned int i, unsigned char r) ;
	void setGreen (unsigned int i, unsigned char g) ;
	void setBlue (unsigned int i, unsigned char b) ;
	
	private:
	vector<RGBTriple> RGBTriples ;
} ;

#endif // ndef SH_COLORPALETTE_H
