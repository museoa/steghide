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

#ifndef SH_PERMUTATION_H
#define SH_PERMUTATION_H

#include "BitString.h"

/**
 * \class Permutation
 * \brief constructs a permutation of {0,...,Width - 1}
 *
 * The maximum value of width is 2^32.
 **/
class Permutation {
	public:
	Permutation (void) ;
	Permutation (unsigned long w, std::string pp) ;

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
	void setKey (std::string pp) ;

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
	unsigned int NBits ;

	/// the maximum for curarg - this is (2^NBits) - 1
	unsigned long MaxArg ;

	/// contains (NBits / 2) 1s in the lsbs
	unsigned long Mask ;

	/// the keys (as derived from passphrase)
	unsigned long Key1 ;
	unsigned long Key2 ;
	unsigned long Key3 ;
	unsigned long Key4 ;

	unsigned long higher (unsigned long a) ;
	unsigned long lower (unsigned long b) ;
	unsigned long shortxor (unsigned long a, unsigned long b) ;
	unsigned long concat (unsigned long l, unsigned long h) ;
	unsigned long keyhash (unsigned long key, unsigned long arg) ;
} ;

#endif //ndef SH_PERMUTATION_H
