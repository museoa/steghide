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

#ifndef SH_MHASHPP_H
#define SH_MHASHPP_H

#include <mhash.h>

#include "bitstring.h"

enum MHashppCommand { endhash } ;

class MHashpp {
	public:
	MHashpp (void) ;
	MHashpp (hashid a) ;

	void init (hashid a) ;

	BitString end (void) ;

	/**
	 * feed the string v to the hashing algorithm
	 * \param v the string to be feeded to the hashing algorithm (without '\0' at the end)
	 **/
	MHashpp& operator<< (string v) ;

	/**
	 * feed the BitString v to the hashing algorithm
	 * \param v the BitString to be feeded to the hashing algorithm (v.getLength() % 8 == 0 must hold)
	 **/
	MHashpp& operator<< (BitString v) ;

	/**
	 * feed the byte v to the hashing algorithm
	 * \param v the byte to be feeded to the hashing algorithm
	 **/
	MHashpp& operator<< (unsigned char v) ;

	/**
	 * interpret the command c
	 * \param c a command (member of Hash::Command)
	 **/
	MHashpp& operator<< (MHashppCommand c) ;

	/**
	 * get the hash bits
	 * \return the hash value of the data that has been passed via <<
	 **/
	BitString getHashBits (void) ;

	/**
	 * get the hash size
	 * \return the size of the value returned by getHashBits in bytes
	 **/
	unsigned int getHashSize (void) ;

	private:
	/// true iff HashD contains a legal hash descriptor and data can be passed via <<
	bool hashing ;
	MHASH HashD ;

	/// true iff HashBits is a BitString containing a hash value
	bool HashBitsValid ;
	BitString HashBits ;

	string getAlgorithmName (void) ;
	static string getAlgorithmName (hashid id) ;

#if 0
	vector<unsigned char> getcrc32 (vector<unsigned char> data) ;
	bool checkcrc32 (vector<unsigned char> data, vector<unsigned char> crc32) ;
#endif
} ;

#endif // ndef SH_MHASHPP_H
