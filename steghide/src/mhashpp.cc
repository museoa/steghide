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

#include <cstdlib>
#include <string>

#include <mhash.h>

#include "bitstring.h"
#include "common.h"
#include "error.h"
#include "mhashpp.h"

MHashpp::MHashpp ()
{
	hashing = false ;
	HashBytesValid = false ;
}

MHashpp::MHashpp (hashid id)
{
	init (id) ;
}

void MHashpp::init (hashid id)
{
	if ((HashD = mhash_init (id)) == MHASH_FAILED) {
		throw SteghideError (_("could not initialize libmhash %s algorithm."), getAlgorithmName(id).c_str()) ;
	}
	hashing = true ;
	HashBytesValid = false ;
}

const std::vector<BYTE>& MHashpp::end ()
{
	myassert (hashing) ;

	unsigned int n = getHashSize() ;
	HashBytes = std::vector<unsigned char> (n) ;
	unsigned char *hash = (unsigned char*) mhash_end (HashD) ;
	hashing = false ;
	for (unsigned int i = 0 ; i < n ; i++) {
		HashBytes[i] = hash[i] ;
	}
	free (hash) ;

	HashBytesValid = true ;

	return HashBytes ;
}

unsigned int MHashpp::getHashSize (void)
{
	myassert (hashing) ;
	return ((unsigned int) mhash_get_block_size (mhash_get_mhash_algo (HashD))) ;
}

MHashpp& MHashpp::operator<< (std::string v)
{
	myassert (hashing) ;
	mhash (HashD, v.data(), v.size()) ;
	return *this ;
}

MHashpp& MHashpp::operator<< (BitString v)
{
	myassert (hashing) ;
	myassert (v.getLength() % 8 == 0) ;

	unsigned long n = v.getLength() / 8 ;
	for (unsigned int i = 0 ; i < n ; i++) {
		(*this) << (unsigned char) v.getValue (8 * i, 8) ;
	}

	return *this ;
}

MHashpp& MHashpp::operator<< (unsigned char v)
{
	myassert (hashing) ;
	mhash (HashD, &v, 1) ;
	return *this ;
}

MHashpp& MHashpp::operator<< (MHashppCommand c)
{
	switch (c) {
		case endhash:
		HashBytes = end() ;
		break ;

		default:
		myassert (0) ;
		break ;
	}
	return *this ;
}

std::string MHashpp::getAlgorithmName ()
{
	myassert (hashing) ;
	return getAlgorithmName (mhash_get_mhash_algo (HashD)) ;
}

std::string MHashpp::getAlgorithmName (hashid id)
{
	char *name = mhash_get_hash_name (id) ;
	std::string retval ;
	if (name == NULL) {
		retval = std::string ("<algorithm not found>") ;
	}
	else {
		retval = std::string (name) ;
	}
	free (name) ;
	return retval ;
}

BitString MHashpp::getHashBits ()
{
	myassert (HashBytesValid) ;
	return BitString (HashBytes) ;
}

const std::vector<BYTE>& MHashpp::getHashBytes()
{
	myassert (HashBytesValid) ;
	return HashBytes ;
}
