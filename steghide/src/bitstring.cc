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

#include "bitstring.h"
#include "common.h"
#include "randomsource.h"

#define BITPOS(n) (n % 8)
#define BYTEPOS(n) (n / 8)

BitString::BitString ()
{
	length = 0 ;
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

	data = vector<unsigned char> (nbytes) ; // is initialized to zeros in vector's constructor
	length = l ;
}

BitString::BitString (const vector<unsigned char> &d)
{
	data = d ;
	length = 8 * data.size() ;
}

BitString::BitString (const string &d)
{
	length = 0 ;
	for (unsigned int i = 0 ; i < d.size() ; i++) {
		append ((unsigned char) d[i]) ;
	}
}

unsigned long BitString::getLength () const
{
	return length ;
}

BitString& BitString::clear()
{
	data.clear() ;
	length = 0 ;
	return *this ;
}

BitString& BitString::append (Bit v)
{
	assert (v == 0 || v == 1) ;
	_append (v) ;
	return *this ;
}

void BitString::_append (Bit v)
{
	if (length % 8 == 0) {
		data.push_back (0) ;
	}
	data[BYTEPOS (length)] |= (v << BITPOS (length)) ;
	length++ ;
}

BitString& BitString::append (bool v)
{
	_append ((Bit) (v ? 1 : 0)) ;
	return *this ;
}

BitString& BitString::append (unsigned char v, unsigned int n)
{
	for (int i = n - 1 ; i >= 0 ; i--) {
		_append ((Bit) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (unsigned int v, unsigned int n)
{
	for (int i = n - 1 ; i >= 0 ; i--) {
		_append ((Bit) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (unsigned long v, unsigned int n)
{
	for (int i = n - 1 ; i >= 0 ; i--) {
		_append ((Bit) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const BitString &v)
{
	unsigned long n = v.getLength() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		_append (v[i]) ;
	}
	return *this ;
}

BitString& BitString::append (const vector<unsigned char> &v)
{
	for (vector<unsigned char>::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append (*i) ;
	}
	return *this ;
}

BitString& BitString::append (const string &v)
{
	for (string::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append ((unsigned char) *i) ;
	}
	return *this ;
}

Bit BitString::operator[] (unsigned long i) const
{
	assert (i < length) ;
	return ((data[BYTEPOS(i)] >> BITPOS(i)) & 1) ;
}

BitString BitString::getBits (unsigned long s, unsigned long e) const
{
	BitString retval ;
	for (unsigned long i = s ; i <= e ; i++) {
		retval.append ((*this)[i]) ;
	}
	return retval ;
}

unsigned long BitString::getValue (unsigned long s, unsigned int l) const
{
	assert (l <= 32) ;
	unsigned long retval = 0 ;
	for (unsigned long i = s ; i < s + l ; i++) {
		retval = retval << 1 ;
		retval |= (*this)[i] ;
	}
	return retval ;
}

vector<unsigned char> BitString::getBytes() const
{
	assert (length % 8 == 0) ;
	return data ; // FIXME - ? vector as reference
}

BitString& BitString::pad (unsigned long mult, Bit v)
{
	assert (v == 0 || v == 1) ;
	while (length % mult != 0) {
		_append (v) ;
	}
	return *this ;
}

BitString& BitString::padRandom (unsigned long mult)
{
	while (length % mult != 0) {
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
	for (unsigned long i = 0 ; i < length ; i++) {
		unsigned long bytepos = BYTEPOS (i) ;
		unsigned int bitpos = BITPOS (i) ;
		Bit bit = (data[bytepos] & (1 << bitpos)) >> bitpos ;
		bit ^= v[i] ;
		data[bytepos] = (data[bytepos] & ~(1 << bitpos)) | (bit << bitpos) ;
	}
	return *this ;
}
