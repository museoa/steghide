/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@teleweb.at>
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
#include "cvrstgsample.h"
#include "cvrstgfile.h"

CvrStgSample::CvrStgSample (void)
	: File(NULL), Label(0)
{
}

CvrStgSample::CvrStgSample (CvrStgFile *f)
	: File(f), Label(0)
{
}

Bit CvrStgSample::getBit (void) const
{
	return SBit ;
}

unsigned long CvrStgSample::getKey (void) const
{
	return Key ;
}

CvrStgFile *CvrStgSample::getFile (void) const
{
	return File ;
}

void CvrStgSample::setFile (CvrStgFile *f)
{
	File = f ;
}

unsigned long CvrStgSample::getLabel (void) const
{
	return Label ;
}

void CvrStgSample::setLabel (unsigned long l)
{
	Label = l ;
}

#if 0
void CvrStgSample::setRadius (void)
{
#ifdef DEBUG
	if (Args.Radius.is_set()) {
		Radius = Args.Radius.getValue() ;
	}
	else {
		Radius = getDefaultRadius() ;
	}
#else
	Radius = getDefaultRadius() ;
#endif
}

float CvrStgSample::Radius = 0.0 ;
#endif
