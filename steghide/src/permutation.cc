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
		BitString curarg ;
		curarg.append (CurArg, nBits) ;
		BitString y = curarg.getBits (0, (nBits / 2) - 1) ;
		BitString x = curarg.getBits (nBits / 2, nBits - 1) ;

		y ^= keyhash (Key1, x) ;
		x ^= keyhash (Key2, y) ;
		y ^= keyhash (Key3, x) ;
		x ^= keyhash (Key4, y) ;

		CurValue = y.append(x).getValue(0, nBits) ;
	} while (CurValue >= Width) ;

	return *this ;
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

		BitString curarg ;
		curarg.append (CurArg, nBits) ;
		BitString y = curarg.getBits (0, (nBits / 2) - 1) ;
		BitString x = curarg.getBits (nBits / 2, nBits - 1) ;

		y ^= keyhash (Key1, x) ;
		x ^= keyhash (Key2, y) ;
		y ^= keyhash (Key3, x) ;
		x ^= keyhash (Key4, y) ;

		CurValue = y.append(x).getValue(0, nBits) ;
	} while (CurValue >= Width) ;
}

void Permutation::setWidth (unsigned long w)
{
	assert (w > 0) ;
	Width = w ;

	nBits = 1 ;
	unsigned long tmp = Width - 1;
	while (tmp > 1) {
		tmp /= 2 ;
		nBits++ ;
	}

	if (nBits % 2 == 1) {
		nBits++ ;
	}
}

void Permutation::setKey (string passphrase)
{
	// FIXME - use mhash_key_generation here ?
	MHashpp hash (MHASH_MD5) ;
	hash << passphrase << endhash ;
	BitString hashbits = hash.getHashBits() ;

	//DEBUG
	assert (hashbits.getLength() == 128) ;

	Key1 = hashbits.getBits (0, 31) ;
	Key2 = hashbits.getBits (32, 63) ;
	Key3 = hashbits.getBits (64, 95) ;
	Key4 = hashbits.getBits (96, 127) ;
}

BitString Permutation::keyhash (BitString key, BitString arg)
{
	MHashpp hash (MHASH_MD5) ;
	key.append(arg) ;
	assert (key.getLength() % 8 == 0) ;
	hash << key << endhash ;
	BitString hashbits = hash.getHashBits() ;

	assert (hashbits.getLength() == 128) ;

	// FIXME - wieso wird das hier bis 32 gemacht - zwei Hälften zu je 32 bits ?
	BitString retval ;
	for (unsigned int i = 0 ; i < 32 ; i++) {
		retval.append (hashbits[i] ^ hashbits[32 + i] ^ hashbits[64 + i] ^ hashbits [96 + i]) ;
	}

	return retval ;
}
