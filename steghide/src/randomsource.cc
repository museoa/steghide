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

#include <cstdlib>
#include <ctime>

#include "randomsource.h"

// the global RandomSource object
RandomSource RndSrc ;

RandomSource::RandomSource ()
{
	srand ((unsigned int) time (NULL)) ;
	RandomBytePos = 8 ;
}

unsigned char RandomSource::getByte ()
{
#ifdef NORANDOMNESS
	return 0 ;
#else
	return (unsigned char) (256.0 * (rand() / (RAND_MAX + 1.0))) ;
#endif
}

vector<unsigned char> RandomSource::getBytes (unsigned int n)
{
	vector<unsigned char> retval ;
	for (unsigned int i = 0 ; i < n ; i++) {
		retval.push_back (getByte()) ;
	}
	return retval ;
}

BitString RandomSource::getBits (unsigned int n)
{
	BitString retval ;
	unsigned char rndbyte = 0 ;
	unsigned int bitsused = 8 ;
	while (n > 0) {
		if (bitsused == 8) {
			rndbyte = getByte() ;
			bitsused = 0 ;
		}
		retval.append ((Bit) (rndbyte & 1)) ;
		rndbyte = rndbyte >> 1 ;
		bitsused++ ;
		n-- ;
	}
	return retval ;
}

bool RandomSource::getBool()
{
	if (RandomBytePos == 8) {
		RandomByte = getByte() ;
		RandomBytePos = 0 ;
	}
	bool retval = (RandomByte & (1 << RandomBytePos)) ;
	RandomBytePos++ ;
	return retval ;
}

unsigned long RandomSource::getValue (unsigned long n)
{
	const unsigned long ceilvalue = 0x1000000UL ;
	assert (n < ceilvalue) ;
	unsigned long value = ((unsigned long) (getByte() << 16)) |
						  ((unsigned long) (getByte() << 8)) |
						  ((unsigned long) getByte()) ;

	return (unsigned long) ((((double) value) / ((double) ceilvalue)) * ((double) n)) ;
}
