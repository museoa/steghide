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

#include <vector>

#include "common.h"

/**
 * \class Permutation
 * \brief constructs a permutation of {0,...,Width - 1}
 *
 * The maximum value of width is 2^32.
 **/
class Permutation {
	public:
	/**
	 * construct a permutation
	 * \param w the width of the permutation (i.e. the number of different arguments and values)
	 * \param pp the passphrase that will be used to generate this permutation
	 * \param n the number of entries that will be used.
	 *
	 * n should not be set to w if less than w entries are used because the calculation
	 * of the permutation is rather time-consuming.
	 * If the passphrase is not given, the identitiy permutation is generated.
	 **/
	Permutation (UWORD32 w, std::string pp = "", UWORD32 n = MAX_UWORD32) ;

#if 0
	/**
	 * increment the argument of the permutation
	 *
	 * If this Permutation is already at its end and operator++
	 * is called, an assertion in this function fails.
	 **/
	Permutation& operator++ (void) ;

	/**
	 * get the current value of the permutation
	 **/
	UWORD32 operator* (void) ;

	/**
	 * reset this permutation, so that *(*this) returns first value
	 **/
	void reset (void) ;

#endif


	/**
	 * get the value of the permutation at position pos
	 **/
	UWORD32 operator[] (UWORD32 pos) const
		{ return Values[pos] ; } ;

	/**
	 * get the width of this permutation
	 **/
	UWORD32 getWidth (void) const
		{ return Width ; } ;

	private:
	/**
	 * calculate the first n positions of the Values array
	 **/
	void calculate (UWORD32 n) ;

	/// Values[i] contains the value of the permutation at position i
	std::vector<UWORD32> Values ;

	/**
	 * set Key[1-4]
	 **/
	void setKey (std::string pp) ;

	/// the keys (as derived from passphrase)
	UWORD32 Key1 ;
	UWORD32 Key2 ;
	UWORD32 Key3 ;
	UWORD32 Key4 ;

	/**
	 * set the width and the dependent variables (NBits, Mask, MaxArg)
	 **/
	void setWidth (UWORD32 w) ;

	UWORD32 Width ;

	/**
	 * the smallest even number with 2^NBits >= Width
	 * this is the length of the argument and the value
	 **/
	unsigned int NBits ;

	/// contains (NBits / 2) 1s in the lsbs
	UWORD32 Mask ;

	/// the maximum for curarg - this is (2^NBits) - 1
	UWORD32 MaxArg ;


	UWORD32 keyhash (UWORD32 key, UWORD32 arg) ;

	UWORD32 higher (UWORD32 a)
		{ return (a >> (NBits / 2)) ; } ;

	UWORD32 lower (UWORD32 a)
		{ return (a & Mask) ; }

	UWORD32 shortxor (UWORD32 a, UWORD32 b)
		{ return ((a ^ b) & Mask) ; }

	UWORD32 concat (UWORD32 l, UWORD32 h)
		{ return ((h << (NBits / 2)) | l) ; }
} ;

#endif //ndef SH_PERMUTATION_H
