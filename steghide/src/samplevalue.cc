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
#include "samplevalue.h"
#include "cvrstgfile.h"

SampleValue::SampleValue (void)
	: File(NULL), Label(0)
{
}

SampleValue::SampleValue (CvrStgFile *f)
	: File(f), Label(0)
{
}

Bit SampleValue::getBit (void) const
{
	return SBit ;
}

unsigned long SampleValue::getKey (void) const
{
	return Key ;
}

CvrStgFile *SampleValue::getFile (void) const
{
	return File ;
}

void SampleValue::setFile (CvrStgFile *f)
{
	File = f ;
}

unsigned long SampleValue::getLabel (void) const
{
	return Label ;
}

void SampleValue::setLabel (unsigned long l)
{
	Label = l ;
}
