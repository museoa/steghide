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

#include <cfloat>

#include "ColorPalette.h"

unsigned int ColorPalette::getNearest (unsigned int idx, ColorPalette::SUBSET s)
{
	const RGBTriple& thistriple = (*this)[idx] ;

	unsigned int i = 0 ;
	unsigned int inc = 0 ;
	switch (s) {
		case ALL:
		i = 0 ;
		inc = 1 ;
		break ;

		case EVENINDICES:
		i = 0 ;
		inc = 2 ;
		break ;

		case ODDINDICES:
		i = 1 ;
		inc = 2 ;
		break ;

		default:
		myassert (0) ;
		break ;
	}

	unsigned int idx_mindist = 0 ;
	float mindist = FLT_MAX ;
	for ( ; i < size() ; i += inc) {
		float curdist = thistriple.calcDistance ((*this)[i]) ;
		if (curdist < mindist && i != idx) {
			mindist = curdist ;
			idx_mindist = i ;
		}
	}

	return idx_mindist ;
}
