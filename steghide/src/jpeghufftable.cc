/*
 * steghide 0.4.6 - a steganography program
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

#include <assert.h>

#include <iostream>

#include "binaryio.h"
#include "jpegbase.h"
#include "jpeghufftable.h"

JpegHuffmanTable::JpegHuffmanTable ()
	: JpegSegment (JpegElement::MarkerDHT)
{
}

JpegHuffmanTable::JpegHuffmanTable (BinaryIO *io)
	: JpegSegment (JpegElement::MarkerDHT)
{
	read (io) ;
}

JpegHuffmanTable::~JpegHuffmanTable ()
{
}

//FIXME DELME - alle print*
void JpegHuffmanTable::printMinCode ()
{
	cerr << "mincode:" << endl ;
	for (unsigned int i = 0 ; i < mincode.size() ; i++) {
		cerr << i << " " << mincode[i] << endl ;
	}
}

void JpegHuffmanTable::printBits ()
{
	cerr << "bits:" << endl ;
	for (unsigned int i = 0 ; i < bits.size() ; i++) {
		cerr << i << " " << bits[i] << endl ;
	}
}

void JpegHuffmanTable::printMaxCode ()
{
	cerr << "maxcode:" << endl ;
	for (unsigned int i = 0 ; i < maxcode.size() ; i++) {
		cerr << i << " " << maxcode[i] << endl ;
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

void JpegHuffmanTable::printValPtr ()
{
	cerr << "valptr:" << endl ;
	for (unsigned int i = 0 ; i < valptr.size() ; i++) {
		cerr << i << " " << valptr[i] << endl ;
	}
}

JpegHuffmanTable::Class JpegHuffmanTable::getClass ()
{
	Class retval ;

	switch (tableclass) {
		case 0:
		retval = DCTABLE ;
		break ;

		case 1:
		retval = ACTABLE ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

unsigned int JpegHuffmanTable::getDestId ()
{
	return tabledestid ;
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

//DEBUG
unsigned int JpegHuffmanTable::getBits (unsigned int l)
{
	if (!(1 <= l && l <= Len_bits)) {
		assert (1 <= l && l <= Len_bits) ;
	}
	return bits[l - 1] ;
}

//DEBUG
unsigned int JpegHuffmanTable::getHuffVal (unsigned int i)
{
	if (!(i < huffval.size()))
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

void JpegHuffmanTable::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	splitByte (io->read8(), &tableclass, &tabledestid) ;
	for (unsigned int i = 0 ; i < Len_bits ; i++) {
		bits.push_back (io->read8()) ;
	}

	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		for (unsigned int j = 0 ; j < getBits (l) ; j++) {
			huffval.push_back (io->read8()) ;
		}
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
	vector<unsigned int>::iterator p = huffval.begin() ;
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
	for (unsigned int l = 1 ; l <= Len_bits ; l++) {
		for (unsigned int j = 0 ; j < getBits(l) ; j++) {
			huffsize.push_back (l) ;
		}
	}

	// generate huffcode
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
}
