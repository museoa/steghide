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

#include <climits>
#include <iostream>

#include "binaryio.h"
#include "common.h"
#include "error.h"
#include "jpegbase.h"
#include "jpeghufftable.h"

JpegHuffmanTable::JpegHuffmanTable (unsigned int lr)
	: JpegSegment (JpegElement::MarkerDHT)
{
	tableclass = 0xFF ;
	tabledestid = 0xFF ;
	lengthremaining = lr ;
}

JpegHuffmanTable::JpegHuffmanTable (BinaryIO *io, unsigned int lr)
	: JpegSegment (JpegElement::MarkerDHT)
{
	tableclass = 0xFF ;
	tabledestid = 0xFF ;
	lengthremaining = lr ;
	read (io) ;
}

JpegHuffmanTable::~JpegHuffmanTable ()
{
}

JpegHuffmanTable::Class JpegHuffmanTable::getClass ()
{
	Class retval ;

	switch (tableclass) {
		case 0:
		{
			retval = DCTABLE ;
			break ;
		}

		case 1:
		{
			retval = ACTABLE ;
			break ;
		}

		default:
		{
			assert (0) ;
			break ;
		}
	}

	return retval ;
}

unsigned int JpegHuffmanTable::getDestId ()
{
	return tabledestid ;
}

void JpegHuffmanTable::reset (std::vector<unsigned int> b, std::vector<unsigned int> hv)
{
	assert (b.size() == 16) ;

	setLength (2 + 1 + b.size() + hv.size()) ;

	bits = b ;
	huffval = hv ;
	calcTables() ;
}

unsigned int JpegHuffmanTable::getHuffSize (unsigned int i)
{
	assert (i < huffsize.size()) ;
	return huffsize[i] ;
}

unsigned int JpegHuffmanTable::getHuffCode (unsigned int i)
{
	assert (i < huffcode.size()) ;
	return huffcode[i] ;
}

unsigned int JpegHuffmanTable::getBits (unsigned int l)
{
	assert (1 <= l && l <= Len_bits) ;
	return bits[l - 1] ;
}

unsigned int JpegHuffmanTable::getHuffVal (unsigned int i)
{
	assert (i < huffval.size()) ;
	return huffval[i] ;
}

int JpegHuffmanTable::getMinCode (unsigned int l)
{
	assert (1 <= l && l <= mincode.size()) ;
	return mincode[l - 1] ;
}

int JpegHuffmanTable::getMaxCode (unsigned int l)
{
	assert (1 <= l && l <= maxcode.size()) ;
	return maxcode[l - 1] ;
}

int JpegHuffmanTable::getValPtr (unsigned int l)
{
	assert (1 <= l && l <= valptr.size()) ;
	return valptr[l - 1] ;
}

unsigned int JpegHuffmanTable::getEHuffCode (unsigned int v)
{
	assert (v < ehuffcode.size()) ;
	return ehuffcode[v] ;
}

unsigned int JpegHuffmanTable::getEHuffSize (unsigned int v)
{
	assert (v < ehuffsize.size()) ;
	return ehuffsize[v] ;
}

unsigned long JpegHuffmanTable::getXHuffCode (int v)
{
	unsigned int ssss = csize (v) ;

	if (v < 0) {
		v-- ;
	}

	unsigned long retval = getEHuffCode (ssss) ;
	for (unsigned int i = ssss ; i > 0 ; i--) {
		int bit = (v & (1 << (i - 1))) >> (i - 1) ;
		retval = retval << 1 ;
		retval = retval | bit ;
	}

	return retval ;
}

unsigned int JpegHuffmanTable::getXHuffSize (int v)
{
	unsigned int ssss = csize (v) ;
	return (getEHuffSize (ssss) + ssss) ;
}

unsigned int JpegHuffmanTable::csize (int v)
{
	if (v < 0) {
		v = -v ;
	}
	unsigned int rv = 0 ;
	while (v != 0) {
		v = v / 2 ;
		rv++ ;
	}
	return rv ;
}

unsigned int JpegHuffmanTable::getLengthRemaining ()
{
	return lengthremaining ;
}

void JpegHuffmanTable::read (BinaryIO *io)
{
	if (lengthremaining == UINT_MAX) {
		// this table is the first in the DHT segment
		JpegSegment::read (io) ;
	}

	// count the number of bytes read for this huffman table
	unsigned int bytecounter = 0 ;

	splitByte (io->read8(), &tableclass, &tabledestid) ;
	bytecounter++ ;
	for (unsigned int i = 0 ; i < Len_bits ; i++) {
		bits.push_back (io->read8()) ;
		bytecounter++ ;
	}

	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		for (unsigned int j = 0 ; j < getBits (l) ; j++) {
			huffval.push_back (io->read8()) ;
			bytecounter++ ;
		}
	}

	if (lengthremaining == UINT_MAX) {
		// this table is the first in the DHT segment
		lengthremaining = getLength() - 2 /* length field */ - bytecounter ;
		setLength (bytecounter + 2) ;
	}
	else {
		setLength (bytecounter + 2) ;
		if (bytecounter > lengthremaining) {
			throw CorruptJpegError (io, "DHT segment is too short") ;
		}
		lengthremaining -= bytecounter ;
	}

	calcTables() ;
}

void JpegHuffmanTable::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->write8 (mergeByte (tableclass, tabledestid)) ;
	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		io->write8 ((unsigned char) getBits(l)) ;
	}
	std::vector<unsigned int>::iterator p = huffval.begin() ;
	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		for (unsigned int j = 0 ; j < getBits (l) ; j++) {
			io->write8 (*p) ;
			p++ ;
		}
	}
}

void JpegHuffmanTable::calcTables ()
{
	// generate huffsize
	huffsize.clear() ;
	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		for (unsigned int j = 0 ; j < getBits(l) ; j++) {
			huffsize.push_back (l) ;
		}
	}

	// generate huffcode
	huffcode.clear() ;
	unsigned char cursize = huffsize[0] ;
	unsigned int code = 0 ;
	for (unsigned char k = 0 ; k < huffsize.size() ; k++) {
		while (huffsize[k] != cursize) {
			code = code << 1 ;
			cursize++ ;
		}
		huffcode.push_back (code) ;
		code++ ;
	}

	// generate mincode, maxcode and valptr
	mincode.clear() ;
	maxcode.clear() ;
	valptr.clear() ;
	unsigned int j = 0 ;
	for (unsigned int l = 1 ; l <= 16 ; l++) {
		if (getBits (l) == 0) {
			maxcode.push_back (-1) ;

			// mincode, valptr should not be used for this l anyway
			mincode.push_back (-1) ;
			valptr.push_back (-1) ;
		}
		else {
			valptr.push_back (j) ;
			mincode.push_back (huffcode[j]) ;
			j += getBits (l) ;
			maxcode.push_back (huffcode[j - 1]) ;
		}
	}

	// generate ehuffcode and ehuffsize
	ehuffcode.clear() ;
	ehuffsize.clear() ;
	for (unsigned int k = 0 ; k < huffval.size() ; k++) {
		unsigned int i = huffval[k] ;
		if (ehuffcode.size() < (i + 1)) {
			ehuffcode.resize (i + 1) ;
			ehuffsize.resize (i + 1) ;
		}
		ehuffcode[i] = huffcode[k] ;
		ehuffsize[i] = huffsize[k] ;
	}
}

#ifdef DEBUG
void JpegHuffmanTable::print ()
{
	cerr << "huffman table:" << endl ;
	cerr << "tableclass: " << (unsigned int) tableclass << endl ;
	cerr << "tabledestid: " << (unsigned int) tabledestid << endl ;
	printBits() ;
	printHuffVal() ;
	printHuffSize() ;
	printHuffCode() ;
	printMinCode() ;
	printMaxCode() ;
	printValPtr() ;
	printEHuffSize() ;
	printEHuffCode() ;
}

void JpegHuffmanTable::printBits ()
{
	cerr << "bits:" << endl ;
	for (unsigned int i = 0 ; i < bits.size() ; i++) {
		cerr << i << " " << bits[i] << endl ;
	}
}

void JpegHuffmanTable::printHuffVal ()
{
	cerr << "huffval:" << endl ;
	for (unsigned int i = 0 ; i < huffval.size() ; i++) {
		cerr << i << " " << huffval[i] << endl ;
	}
}

void JpegHuffmanTable::printHuffSize ()
{
	cerr << "huffsize:" << endl ;
	for (unsigned int i = 0 ; i < huffsize.size() ; i++) {
		cerr << i << " " << huffsize[i] << endl ;
	}
}

void JpegHuffmanTable::printHuffCode ()
{
	cerr << "huffcode:" << endl ;
	for (unsigned int i = 0 ; i < huffcode.size() ; i++) {
		cerr << i << " " << huffcode[i] << endl ;
	}
}

void JpegHuffmanTable::printMinCode ()
{
	cerr << "mincode:" << endl ;
	for (unsigned int i = 0 ; i < mincode.size() ; i++) {
		cerr << i << " " << mincode[i] << endl ;
	}
}

void JpegHuffmanTable::printMaxCode ()
{
	cerr << "maxcode:" << endl ;
	for (unsigned int i = 0 ; i < maxcode.size() ; i++) {
		cerr << i << " " << maxcode[i] << endl ;
	}
}

void JpegHuffmanTable::printValPtr ()
{
	cerr << "valptr:" << endl ;
	for (unsigned int i = 0 ; i < valptr.size() ; i++) {
		cerr << i << " " << valptr[i] << endl ;
	}
}

void JpegHuffmanTable::printEHuffSize ()
{
	cerr << "ehuffsize:" << endl ;
	for (unsigned int i = 0 ; i < ehuffsize.size() ; i++) {
		cerr << i << " " << ehuffsize[i] << endl ;
	}
}

void JpegHuffmanTable::printEHuffCode ()
{
	cerr << "ehuffcode:" << endl ;
	for (unsigned int i = 0 ; i < ehuffcode.size() ; i++) {
		cerr << i << " " << ehuffcode[i] << endl ;
	}
}
#endif
