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

#ifndef SH_BITSTRING_H
#define SH_BITSTRING_H

#include <vector>
#include <string>

#include "common.h"

/**
 * \class BitString
 * \brief a string of bits
 *
 * This class provides a way to conveniently store and
 * manipulate a string of bits. Various objects can be
 * appended to a BitString.
 * 
 * For data storage a vector of BYTEs is used with little
 * endian bit encoding, i.e. the first bit is the least
 * significant bit of the first byte and so on.
 **/
class BitString {
	public:
	/**
	 * construct an empty BitString
	 **/
	BitString (void) ;
	
	/**
	 * construct a BitString containing l zeros
	 **/
	BitString (const unsigned long l) ;
	
	/**
	 * construct a BitString containing the data in d
	 **/
	BitString (const std::vector<BYTE>& d) ;
	
	/**
	 * construct a BitString containing the characters in d as 8 bit unsigned chars
	 **/
	BitString (const std::string& d) ;
	
	/**
	 * get the length of this bitstring (in bits)
	 **/
	unsigned long getLength (void) const
		{ return Length ; } ;

	/**
	 * delete the contents of this Bitstring
	 **/
	BitString& clear (void) ;

	/**
	 * append the bit v to this BitString
	 **/
	BitString& append (const BIT v) ;

	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const BYTE v, const unsigned short n = 8) ;
	
	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const UWORD16 v, const unsigned short n = 16) ;
	
	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const UWORD32 v, const unsigned short n = 32) ;
	
	/**
	 * append the string v to this BitString
	 **/
	BitString& append (const std::string& v) ;

	/**
	 * append the vector v byte-wise to this BitString
	 **/
	BitString& append (const std::vector<BYTE>& v) ;

	/**
	 * append the BitString v to this BitString
	 * \param v the BitString to be appended
	 **/
	BitString& append (const BitString& v) ;

	/**
	 * get a BitString that is a part of this BitString
	 * \param s the index of the first bit to be copied from this BitString
	 * \param l the total number of bits to be used for the return value
	 * \return the BitString containing of the bits [s...s+(l-1)] of this BitString
	 **/
	BitString getBits (const unsigned long s, const unsigned long l) const ;

	/**
	 * return a value composed from bits in this BitString
	 * \param s the index of the first bit to be used for the return value
	 * \param l the total number of bits to be used for the return value (must be <= 32)
	 * \return the unsigned long value (*this)[s],...,(*this)[s+l-1]
	 **/
	UWORD32 getValue (const unsigned long s, const unsigned short l) const ;

	/**
	 * get the contents of this BitString as vector of bytes
	 * \return the contents of this BitString as vector of bytes
	 *
	 * getLength() % 8 must be 0 to call this function
	 **/
	const std::vector<BYTE>& getBytes (void) const ;

	/**
	 * pad this BitString with the value in v
	 * \param mult this BitString is padded until size is a multiple of mult (given in bits)
	 * \param v the value this BitString should be padded with
	 **/
	BitString& pad (const unsigned long mult, const BIT v) ;

	/**
	 * pad this BitString with random data
	 * \param mult this BitString is padded until size is a multiple of mult (given in bits)
	 **/
	BitString& padRandom (const unsigned long mult) ;

	/**
	 * get the value of the i-th bit
	 **/
	BIT operator[] (const unsigned long i) const ;

	/**
	 * xor v with this BitString, saving the result in this Bitstring.
	 * The result has the same length as this BitString.
	 **/
	BitString& operator^= (const BitString &v) ;

	/**
	 * compare this BitString with the BitString v
	 * \return true iff the lengths are equal and for every valid index the value is equal
	 **/
	bool operator== (const BitString& v) const ;

#ifdef DEBUG
	void print (unsigned short spc = 0) const ;
#endif
	void printDebug (unsigned short level, unsigned short spc = 0) const ;

	protected:
	void _append (BIT v) ;

	private:
	unsigned long Length ;
	std::vector<BYTE> Data ;
} ;

#endif // ndef SH_BITSTRING_H
