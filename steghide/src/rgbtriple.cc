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

#include "rgbtriple.h"

float RGBTriple::calcDistance (const RGBTriple& t) const
{
	float dr = (float) Red - (float) t.Red ;
	float dg = (float) Green - (float) t.Green ;
	float db = (float) Blue - (float) t.Blue ;
	return (sqrt (dr*dr + dg*dg + db*db)) ;
}

bool RGBTriple::operator== (const RGBTriple& t) const
{
	return ((Red == t.Red) && (Green == t.Green) && (Blue == t.Blue)) ;
}

bool RGBTriple::operator!= (const RGBTriple& t) const
{
	return ((Red != t.Red) || (Green != t.Green) || (Blue != t.Blue)) ;
}
