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

#ifndef SH_PERMUTATION_H
#define SH_PERMUTATION_H

#include "bitstring.h"

/**
 * \class Permutation
 * \brief constructs a permutation of {0,...,Width - 1}
 **/
class Permutation {
	public:
	Permutation (void) ;
	Permutation (unsigned long w, string pp) ;

	/**
	 * increment the argument of the permutation
	 **/
	Permutation& operator++ (void) ;

	/**
	 * get the current value of the permutation
	 **/
	unsigned long operator* (void) ;

	/**
	 * reset this permutation, so that (*this)* returns first value
	 **/
	void reset (void) ;

	void setWidth (unsigned long w) ;
	void setKey (string pp) ;

	private:
	unsigned long CurArg ;
	/// current value of the permutation curvalue = P(curarg) 
	unsigned long CurValue ;
	/// the permutation ranges from 0...Width - 1
	unsigned long Width ;
	/**
	 * the smallest even number with 2^nBits >= Width
	 * this is the length of the argument and the value
	 **/
	unsigned int nBits ;
	/// the key (as derived from passphrase)
	BitString Key1 ;
	BitString Key2 ;
	BitString Key3 ;
	BitString Key4 ;

	BitString keyhash (BitString key, BitString arg) ;
} ;

#endif //ndef SH_PERMUTATION_H
