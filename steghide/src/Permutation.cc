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
#include "BitString.h"
#include "MHash.h"
#include "Permutation.h"

Permutation::Permutation (UWORD32 w, std::string pp, UWORD32 n)
{
	if (n > w) { // useful when n is left to default value
		n = w ;
	}
	setWidth (w) ;
	setKey (pp) ;
	if (pp == "") {
		Values.reserve(n) ;
		for (UWORD32 i = 0 ; i < n ; i++) {
			Values.push_back(i) ;
		}
	}
	else {
		calculate (n) ;
	}
}

void Permutation::calculate (UWORD32 n)
{
	Values.clear() ;
	Values.reserve(n) ;
	UWORD32 arg_int = 0 ;
	for (UWORD32 arg_ext = 0 ; arg_ext < n ; arg_ext++) {
		UWORD32 val = 0 ;
		do {
			myassert (arg_int <= MaxArg) ;

			UWORD32 x = lower (arg_int) ;
			UWORD32 y = higher (arg_int) ;

			y = shortxor (y, keyhash (Key1, x)) ;
			x = shortxor (x, keyhash (Key2, y)) ;
			y = shortxor (y, keyhash (Key3, x)) ;
			x = shortxor (x, keyhash (Key4, y)) ;

			val = concat (x, y) ;
			arg_int++ ;
		} while (val >= Width) ;
		Values.push_back (val) ;
	}
}

void Permutation::setWidth (UWORD32 w)
{
	myassert (w > 0) ;
	Width = w ;

	NBits = 1 ;
	UWORD32 tmp = Width - 1;
	while (tmp > 1) {
		tmp /= 2 ;
		NBits++ ;
	}

	if (NBits % 2 == 1) {
		NBits++ ;
	}

	myassert (NBits % 2 == 0) ;
	myassert (NBits <= 32) ;

	Mask = 0 ;
	for (unsigned short i = 0 ; i < (NBits / 2) ; i++) {
		Mask <<= 1 ;
		Mask |= 1 ;
	}

	MaxArg = 0 ;
	for (unsigned short i = 0 ; i < NBits ; i++) {
		MaxArg <<= 1 ;
		MaxArg |= 1 ;
	}
}

void Permutation::setKey (std::string pp)
{
	MHash hash (MHASH_MD5) ;
	hash << pp << MHash::endhash ;
	BitString hashbits = hash.getHashBits() ;

	myassert (hashbits.getLength() == 128) ;
	Key1 = hashbits.getValue (0, 32) ;
	Key2 = hashbits.getValue (32, 32) ;
	Key3 = hashbits.getValue (64, 32) ;
	Key4 = hashbits.getValue (96, 32) ;
}

UWORD32 Permutation::keyhash (UWORD32 key, UWORD32 arg)
{
	// hash the concatenation of key and arg with md5
	MHash hash (MHASH_MD5) ;
	for (unsigned short i = 0 ; i < 32 ; i += 8) {
		hash << (BYTE) ((key >> i) & 0xFF) ;
	}
	myassert (arg <= 0xFFFF) ;
	for (unsigned short i = 0 ; i < (NBits / 2) ; i += 8) {
		hash << (BYTE) ((arg >> i) & 0xFF) ;
	}
	hash << MHash::endhash ;
	std::vector<BYTE> hashbytes = hash.getHashBytes() ;
	myassert (hashbytes.size() == 16) ;

	// compose a 16 bit return value from the 128 bit md5 hash
	UWORD32 retval = 0 ;
	for (unsigned short i = 0 ; i < 8 ; i++) {
		retval ^= hashbytes[i] ;
	}
	retval <<= 8 ;
	for (unsigned short i = 8 ; i < 16 ; i++) {
		retval ^= hashbytes[i] ;
	}

	myassert (retval <= 0xFFFF) ;
	return retval ;
}
