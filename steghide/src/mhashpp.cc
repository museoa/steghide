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
#include <string>

#include <mhash.h>

#include "bitstring.h"
#include "common.h"
#include "error.h"
#include "mhashpp.h"

MHashpp::MHashpp ()
{
	hashing = false ;
	HashBitsValid = false ;
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
	HashBitsValid = false ;
}

BitString MHashpp::end ()
{
	assert (hashing) ;

	unsigned char *hash = (unsigned char*) mhash_end (HashD) ;
	HashBits.clear() ;
	unsigned int n = getHashSize() ;
	for (unsigned int i = 0 ; i < n ; i++) {
		HashBits.append (hash[i]) ;
	}

	hashing = false ;
	HashBitsValid = true ;

	return HashBits ;
}

unsigned int MHashpp::getHashSize (void)
{
	assert (hashing) ;
	return ((unsigned int) mhash_get_block_size (mhash_get_mhash_algo (HashD))) ;
}

MHashpp& MHashpp::operator<< (string v)
{
	assert (hashing) ;
	mhash (HashD, v.data(), v.size()) ;
	return *this ;
}

MHashpp& MHashpp::operator<< (BitString v)
{
	assert (hashing) ;
	assert (v.getLength() % 8 == 0) ;

	unsigned long n = v.getLength() / 8 ;
	for (unsigned int i = 0 ; i < n ; i++) {
		(*this) << (unsigned char) v.getValue (8 * i, 8) ;
	}

	return *this ;
}

MHashpp& MHashpp::operator<< (unsigned char v)
{
	assert (hashing) ;
	mhash (HashD, &v, 1) ;
	return *this ;
}

MHashpp& MHashpp::operator<< (MHashppCommand c)
{
	switch (c) {
		case endhash:
		HashBits = end() ;
		break ;

		default:
		assert (0) ;
		break ;
	}
	return *this ;
}

string MHashpp::getAlgorithmName ()
{
	assert (hashing) ;
	return getAlgorithmName (mhash_get_mhash_algo (HashD)) ;
}

string MHashpp::getAlgorithmName (hashid id)
{
	char *name = mhash_get_hash_name (id) ;
	string retval ;
	if (name == NULL) {
		retval = string ("<algorithm not found>") ;
	}
	else {
		retval = string (name) ;
	}
	free (name) ;
	return retval ;
}

BitString MHashpp::getHashBits ()
{
	assert (HashBitsValid) ;
	return HashBits ;
}

#if 0
unsigned long getseed (char *passphrase)
{
	MHASH hashd ;
	unsigned char *hash = NULL ;
	unsigned char tmp[4] = { '\0', '\0', '\0', '\0' } ;
	unsigned long retval = 0UL ;
	int i = 0 ;

	if ((hashd = mhash_init (MHASH_MD5)) == MHASH_FAILED) {
		throw SteghideError (_("could not initialize libmhash MD5 algorithm.")) ;
	}
	mhash (hashd, passphrase, strlen (passphrase)) ;
	hash = (unsigned char *) mhash_end (hashd) ;

	for (i = 0 ; i < 4 ; i++) {
		tmp[i] = hash[0 + i] ^ hash[4 + i] ^ hash[8 + i] ^ hash[12 + i] ;
	}

	cp32uc2ul_be (&retval, tmp) ;

	return retval ;
}

vector<unsigned char> getcrc32 (vector<unsigned char> data)
{
	MHASH hashd ;

	if ((hashd = mhash_init (MHASH_CRC32)) == MHASH_FAILED) {
		throw SteghideError (_("could not initialize libmhash CRC32 algorithm.")) ;
	}

	unsigned long n = data.size() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		unsigned char c = data[i] ;
		mhash (hashd, &c, 1) ;
	}

	unsigned char *crc32 = mhash_end (hashd) ;

	vector<unsigned char> retval (4) ;
	retval.assign (crc32, &crc32[3]) ;
	return retval ;
}

vector<unsigned char> checkcrc32 (vector<unsigned char> data, vector<unsigned char> crc32)
{
	return (getcrc32 (data) == crc32) ;
}

void *getblowfishkey (char *passphrase)
{
	MHASH hashd ;

	if ((hashd = mhash_init (MHASH_MD5)) == MHASH_FAILED) {
		throw SteghideError (_("could not initialize libmhash MD5 algorithm.")) ;
	}

	mhash (hashd, passphrase, strlen (passphrase)) ;

	return mhash_end (hashd) ;
}
#endif
