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

#ifndef SH_AUSAMPLE_H
#define SH_AUSAMPLE_H

#include "cvrstgsample.h"
#include "common.h"

class AuSample : public CvrStgSample {
	public:
	AuSample (void) : CvrStgSample(NULL) {} ;
	AuSample (CvrStgFile *f, unsigned char v) : CvrStgSample(f), Value(v) {} ;

	Bit getBit (void) const ;
	bool isNeighbour (CvrStgSample *s) const ;
	list<CvrStgSample*> *getOppositeNeighbours (void) const ;
	CvrStgSample* getNearestOppositeNeighbour (void) const ;
	float calcDistance (CvrStgSample *s) const ;
	unsigned long getKey (void) const ;

	unsigned char getValue (void) const ;

	private:
	unsigned char Value ;
} ;

#endif // ndef SH_AUSAMPLE_H
