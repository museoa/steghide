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
#include "mhashpp.h"
#include "permutation.h"

Permutation::Permutation ()
{
	CurArg = 0 ;
	CurValue = 0 ;
}

Permutation::Permutation (unsigned long w, string pp)
{
	CurArg = 0 ;
	CurValue = 0 ;
	setWidth (w) ;
	setKey (pp) ;
	reset() ;
}

Permutation& Permutation::operator++ ()
{
	do {
		CurArg++ ;
		assert (CurArg <= MaxArg) ;

		unsigned long x = lower (CurArg) ;
		unsigned long y = higher (CurArg) ;

		y = shortxor (y, keyhash (Key1, x)) ;
		x = shortxor (x, keyhash (Key2, y)) ;
		y = shortxor (y, keyhash (Key3, x)) ;
		x = shortxor (x, keyhash (Key4, y)) ;

		CurValue = concat (x, y) ;
	} while (CurValue >= Width) ;

	return *this ;
}

unsigned long Permutation::higher (unsigned long a)
{
	return (a >> (NBits / 2)) ;
}

unsigned long Permutation::lower (unsigned long a)
{
	return (a & Mask) ;
}

unsigned long Permutation::shortxor (unsigned long a, unsigned long b)
{
	return ((a ^ b) & Mask) ;
}

unsigned long Permutation::concat (unsigned long l, unsigned long h)
{
	return ((h << (NBits / 2)) | l) ;
}

unsigned long Permutation::operator* ()
{
	return CurValue ;
}

void Permutation::reset()
{
	bool first = true ;
	do {
		if (first) {
			CurArg = 0 ;
			first = false ;
		}
		else {
			CurArg++ ;
		}
		assert (CurArg <= MaxArg) ;

		unsigned long x = lower (CurArg) ;
		unsigned long y = higher (CurArg) ;

		y = shortxor (y, keyhash (Key1, x)) ;
		x = shortxor (x, keyhash (Key2, y)) ;
		y = shortxor (y, keyhash (Key3, x)) ;
		x = shortxor (x, keyhash (Key4, y)) ;

		CurValue = concat (x, y) ;
	} while (CurValue >= Width) ;
}

void Permutation::setWidth (unsigned long w)
{
	assert (w > 0) ;
	Width = w ;

	NBits = 1 ;
	unsigned long tmp = Width - 1;
	while (tmp > 1) {
		tmp /= 2 ;
		NBits++ ;
	}

	if (NBits % 2 == 1) {
		NBits++ ;
	}

	assert (NBits % 2 == 0) ;
	assert (NBits <= 32) ;

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

void Permutation::setKey (string pp)
{
	MHashpp hash (MHASH_MD5) ;
	hash << pp << endhash ;
	BitString hashbits = hash.getHashBits() ;

	assert (hashbits.getLength() == 128) ;
	Key1 = hashbits.getValue (0, 32) ;
	Key2 = hashbits.getValue (32, 32) ;
	Key3 = hashbits.getValue (64, 32) ;
	Key4 = hashbits.getValue (96, 32) ;
}

unsigned long Permutation::keyhash (unsigned long key, unsigned long arg)
{
	// hash the concatenation of key and arg with md5
	MHashpp hash (MHASH_MD5) ;
	for (unsigned short i = 0 ; i < 32 ; i += 8) {
		hash << (unsigned char) ((key >> i) & 0xFF) ;
	}
	assert (arg <= 0xFFFF) ;
	for (unsigned short i = 0 ; i < (NBits / 2) ; i+= 8) {
		hash << (unsigned char) ((arg >> i) & 0xFF) ;
	}
	hash << endhash ;
	vector<unsigned char> hashbytes = hash.getHashBytes() ;
	assert (hashbytes.size() == 16) ;

	// compose a 16 bit return value from the 128 bit md5 hash
	unsigned long retval = 0 ;
	for (unsigned short i = 0 ; i < 8 ; i++) {
		retval ^= hashbytes[i] ;
	}
	retval <<= 8 ;
	for (unsigned short i = 8 ; i < 16 ; i++) {
		retval ^= hashbytes[i] ;
	}

	assert (retval <= 0xFFFF) ;
	return retval ;
}
