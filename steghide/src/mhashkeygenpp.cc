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

#include <cstring>
#include <cstdlib>
#include <vector>

#include "common.h"
#include "error.h"
#include "mhashkeygenpp.h"

MHashKeyGenpp::MHashKeyGenpp ()
{
	AlgorithmData.count = 0 ;
	AlgorithmData.salt = NULL ;
	AlgorithmData.salt_size = 0 ;
}

MHashKeyGenpp::MHashKeyGenpp (keygenid kgalgo, hashid halgo, unsigned int keysize)
{
	setKeyGenAlgorithm (kgalgo) ;
	setKeySize (keysize) ;
	setHashAlgorithm (halgo) ;
	AlgorithmData.count = 0 ;
	AlgorithmData.salt = NULL ;
	AlgorithmData.salt_size = 0 ;
}

MHashKeyGenpp::~MHashKeyGenpp ()
{
	if (AlgorithmData.salt != NULL) {
		free (AlgorithmData.salt) ;
	}
}

std::vector<unsigned char> MHashKeyGenpp::createKey (std::string password)
{
	// FIXME - myassert (ready) ;

	char *passwd = (char *) s_malloc (password.size() + 1) ;
	strcpy (passwd, password.c_str()) ;
	int passwdlen = strlen (passwd) ;
	unsigned char *key = (unsigned char *) s_malloc (KeySize) ;

	if (mhash_keygen_ext (Algorithm, AlgorithmData, key, KeySize, (unsigned char *) passwd, passwdlen) < 0) {
		throw SteghideError (_("could not generate key using libmhash.")) ;
	}

	std::vector<unsigned char> retval (KeySize) ;
	for (unsigned int i = 0 ; i < KeySize ; i++) {
		retval[i] = key[i] ;
	}

	free (passwd) ;
	free (key) ;

	return retval ;
}

void MHashKeyGenpp::setKeySize (unsigned int keysize)
{
	KeySize = keysize ;
}

void MHashKeyGenpp::setKeyGenAlgorithm (keygenid algo)
{
	Algorithm = algo ;
}

void MHashKeyGenpp::setHashAlgorithm (hashid hashalgo)
{
	AlgorithmData.hash_algorithm[0] = hashalgo ;
}

void MHashKeyGenpp::setHashAlgorithms (std::vector<hashid> hashalgos)
{
	myassert (hashalgos.size() <= 2) ;
	for (unsigned int i = 0 ; i < hashalgos.size() ; i++) {
		AlgorithmData.hash_algorithm[i] = hashalgos[i] ;
	}
}

void MHashKeyGenpp::setSalt (std::vector<unsigned char> salt)
{
	AlgorithmData.salt_size = salt.size() ;
	if (AlgorithmData.salt != NULL) {
		free (AlgorithmData.salt) ;
	}
	AlgorithmData.salt = s_malloc (AlgorithmData.salt_size) ;
	unsigned char *tmp = (unsigned char *) AlgorithmData.salt ;
	for (int i = 0 ; i < AlgorithmData.salt_size ; i++) {
		tmp[i] = salt[i] ;
	}
}

void *MHashKeyGenpp::s_malloc (size_t size)
{
	void *retval = NULL ;
	if ((retval = malloc (size)) == NULL) {
		throw SteghideError (_("could not allocate memory.")) ;
	}
	return retval ;
}
