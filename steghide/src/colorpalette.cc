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

#include <cmath>

#include "colorpalette.h"
#include "common.h"

ColorPalette::ColorPalette (void)
{
}

void ColorPalette::addEntry (const RGBTriple& rgb)
{
	RGBTriples.push_back (rgb) ;
}

void ColorPalette::addEntry (unsigned char r, unsigned char g, unsigned char b)
{
	RGBTriples.push_back (RGBTriple (r, g, b)) ;
}

unsigned int ColorPalette::getSize () const
{
	return RGBTriples.size() ;
}

const RGBTriple& ColorPalette::getEntry (unsigned int i) const
{
	assert (i < getSize()) ;
	return RGBTriples[i] ;
}

void ColorPalette::setEntry (unsigned int i, const RGBTriple& rgb)
{
	RGBTriples[i] = rgb ;
}

unsigned char ColorPalette::getRed (unsigned int i) const
{
	assert (i < getSize()) ;
	return RGBTriples[i].Red ;
}

unsigned char ColorPalette::getGreen (unsigned int i) const
{
	assert (i < getSize()) ;
	return RGBTriples[i].Green ;
}

unsigned char ColorPalette::getBlue (unsigned int i) const
{
	assert (i < getSize()) ;
	return RGBTriples[i].Blue ;
}

void ColorPalette::setRed (unsigned int i, unsigned char r)
{
	assert (i < getSize()) ;
	RGBTriples[i].Red = r ;
}

void ColorPalette::setGreen (unsigned int i, unsigned char g)
{
	assert (i < getSize()) ;
	RGBTriples[i].Green = g ;
}

void ColorPalette::setBlue (unsigned int i, unsigned char b)
{
	assert (i < getSize()) ;
	RGBTriples[i].Blue = b ;
}
