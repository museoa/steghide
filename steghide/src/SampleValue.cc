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

#include "common.h"
#include "SampleValue.h"
#include "CvrStgFile.h"

SampleValue::SampleValue (const CvrStgFile* f)
{
	File = f ;
}

bool SampleValue::isNeighbour (const SampleValue *s) const
{
	return (calcDistance (s) <= Radius) ;
}

void SampleValue::decNumEdges ()
{
	myassert (NumEdges > 0) ;
	NumEdges-- ;
}

void SampleValue::setRadius (UWORD32 dr)
{
	if (Radius == 0) {
		if (Args.Radius.is_set()) {
			Radius = Args.Radius.getValue() ;
		}
		else {
			Radius = dr ;
		}
	}
}

void SampleValue::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;
	std::cerr << space << "SampleValue:" << std::endl ;
	std::cerr << space << " Label: " << getLabel() << std::endl ;
	std::cerr << space << " Key(hex): " << std::hex << getKey() << std::dec << std::endl ;
	std::cerr << space << " Bit: " << getBit() << std::endl ;
}

UWORD32 SampleValue::Radius = 0 ;
