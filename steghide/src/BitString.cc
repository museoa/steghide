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

#include "BitString.h"
#include "common.h"

#define BITPOS(n) (n % 8)
#define BYTEPOS(n) (n / 8)

BitString::BitString ()
{
	Length = 0 ;
}

BitString::BitString (unsigned long l)
{
	unsigned long nbytes = 0 ;
	if (l % 8 == 0) {
		nbytes = l / 8 ;
	}
	else {
		nbytes = (l / 8) + 1 ;
	}

	Data = std::vector<BYTE> (nbytes, 0) ; // is initialized to zeros in std::vector's constructor
	Length = l ;
}

BitString::BitString (const std::vector<BYTE> &d)
{
	Length = 0 ;
	append (d) ;
}

BitString::BitString (const std::string &d)
{
	Length = 0 ;
	append (d) ;
}

BitString& BitString::clear()
{
	Data.clear() ;
	Length = 0 ;
	return *this ;
}

void BitString::_append (BIT v)
{
	if (Length % 8 == 0) {
		Data.push_back (0) ;
	}
	Data[BYTEPOS(Length)] |= (v << BITPOS(Length)) ;
	Length++ ;
}

BitString& BitString::append (BIT v)
{
	_append (v) ;
	return *this ;
}

BitString& BitString::append (const BYTE v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const UWORD16 v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const UWORD32 v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const BitString& v)
{
	for (unsigned long i = 0 ; i < v.getLength() ; i++) {
		_append (v[i]) ;
	}
	return *this ;
}

BitString& BitString::append (const std::vector<BYTE>& v)
{
	for (std::vector<BYTE>::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append (*i) ;
	}
	return *this ;
}

BitString& BitString::append (const std::string& v)
{
	for (std::string::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append ((BYTE) *i) ;
	}
	return *this ;
}

BIT BitString::operator[] (unsigned long i) const
{
	myassert (i < Length) ;
	return ((Data[BYTEPOS(i)] >> BITPOS(i)) & 1) ;
}

BitString BitString::getBits (unsigned long s, unsigned long l) const
{
	BitString retval ;
	for (unsigned long i = 0 ; i < l ; i++) {
		retval.append ((*this)[s + i]) ;
	}
	return retval ;
}

UWORD32 BitString::getValue (unsigned long s, unsigned short l) const
{
	myassert (l <= 32) ;
	UWORD32 retval = 0 ;
	for (unsigned short i = 0 ; i < l ; i++) {
		retval |= (*this)[s + i] << i ;
	}
	return retval ;
}

const std::vector<BYTE>& BitString::getBytes() const
{
	myassert (Length % 8 == 0) ;
	return Data ;
}

BitString& BitString::pad (unsigned long mult, BIT v)
{
	while (Length % mult != 0) {
		_append (v) ;
	}
	return *this ;
}

BitString& BitString::padRandom (unsigned long mult)
{
	while (Length % mult != 0) {
		append (RndSrc.getBool()) ;
	}
	return *this ;
}

bool BitString::operator== (const BitString &v) const
{
	bool retval = true ;

	if (v.getLength() == getLength()) {
		unsigned int n = getLength() ;
		for (unsigned int i = 0 ; i < n ; i++) {
			if (v[i] != (*this)[i]) {
				retval = false ;
			}
		}
	}
	else {
		retval = false ;
	}
	
	return retval ;
}

BitString& BitString::operator^= (const BitString &v)
{
	for (unsigned long i = 0 ; i < Length ; i++) {
		unsigned long bytepos = BYTEPOS (i) ;
		unsigned int bitpos = BITPOS (i) ;
		BIT bit = (Data[bytepos] & (1 << bitpos)) >> bitpos ;
		bit ^= v[i] ;
		Data[bytepos] = (Data[bytepos] & ~(1 << bitpos)) | (bit << bitpos) ;
	}
	return *this ;
}

#ifdef DEBUG
void BitString::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;
	std::cerr << space << "Length: " << getLength() << std::endl ;
	std::cerr << space << "Data.size(): " << Data.size() << std::endl ;
	std::cerr << space << "Data:" << std::endl ;

	std::cerr << space ;
	for (unsigned long i = 0 ; i < getLength() ; i++) {
		if (i % 8 == 0) {
			std::cerr << " " ;
		}
		std::cerr << ((*this)[i]) ;
	}
	std::cerr << std::endl ;
}
#endif

void BitString::printDebug (unsigned short level, unsigned short spc) const
{
#ifdef DEBUG
	if (RUNDEBUGLEVEL(level)) {
		print (spc) ;
	}
#endif
}
