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

#ifndef SH_BITSTRING_H
#define SH_BITSTRING_H

#include <vector>
#include <string>

// common.h is not included here because randomsource.h is included in common.h
// and bitstring.h is included in randomsource.h
typedef int Bit ;

class BitString {
	public:
	BitString (void) ;
	/**
	 * construct a BitString containing l zeros
	 **/
	BitString (unsigned long l) ;
	/**
	 * construct a BitString containing the data in d
	 **/
	BitString (vector<unsigned char> d) ;
	
	/**
	 * construct a BitString containing the characters in d as 8 bit unsigned chars
	 **/
	BitString (string d) ;
	
	unsigned long getLength (void) ;

	/**
	 * delete the contents of this Bitstring
	 **/
	BitString& clear (void) ;

	/**
	 * append the bit v to this BitString
	 **/
	BitString& append (bool v) ;

	BitString& append (Bit v) ;

	/**
	 * append n lower order bits (with MSB first) of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (unsigned char v, unsigned int n = 8) ;
	
	/**
	 * append n lower order bits (with MSB first) of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (unsigned int v, unsigned int n) ;
	
	/**
	 * append n lower order bits (with MSB first) of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (unsigned long v, unsigned int n = 32) ;
	
	/**
	 * append the string v to this BitString
	 **/
	BitString& append (string v) ;

	/**
	 * append the vector v byte-wise to this BitString
	 **/
	BitString& append (vector<unsigned char> v) ;

	/**
	 * append the BitString v to this BitString
	 * \param v the BitString to be appended
	 **/
	BitString& append (BitString v) ;

	/**
	 * get a BitString that is a part of this BitString
	 * \param s the index of the first bit to be copied from this BitString
	 * \param e the index of the last bit to be copied from this BitString
	 * \return the BitString containing of the bits [s...e] of this BitString
	 **/
	BitString getBits (unsigned long s, unsigned long e) ;

	/**
	 * return an unsigned long value composed from bits in this BitString (msb first)
	 * \param s the index of the first bit to be used for the return value
	 * \param l the total number of bits to be used for the return value (must be <= 32)
	 * \return the unsigned long value (*this)[s],...,(*this)[s+l-1]
	 **/
	unsigned long getValue (unsigned long s, unsigned int l) ;

	/**
	 * get the contents of this BitString as vector of unsigned char
	 * \return the contents of this BitString as vector of unsigned char
	 *
	 * getLength() % 8 must be 0 to call this function
	 **/
	vector<unsigned char> getBytes (void) ;

	/**
	 * pad this BitString with the value in v
	 * \param mult this BitString is padded until size is a multiple of mult
	 **/
	BitString& pad (unsigned long mult, Bit v) ;

	/**
	 * pad this BitString with random data
	 * \param mult this BitString is padded until size is a multiple of mult
	 **/
	BitString& padRandom (unsigned long mult) ;

	/**
	 * get the value of the i-th bit
	 **/
	Bit operator[] (unsigned long i) ;

	/**
	 * xor v with this BitString, saving the result in this Bitstring - the result has the same length as this BitString
	 **/
	BitString& operator^= (BitString v) ;

	/**
	 * compare this BitString with the BitString v
	 * \return true iff the lengths are equal and for every valid index the value is equal
	 **/
	bool operator== (BitString v) ;

	private:
	unsigned long calcBytePos (unsigned long n) ;
	unsigned int calcBitPos (unsigned long n) ;

	unsigned long length ;
	vector<unsigned char> data ;
} ;

#endif // ndef SH_BITSTRING_H
