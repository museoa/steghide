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

unsigned char JpegHuffmanTable::getHuffVal (unsigned int i)
{
	assert (i < huffval.size()) ;
	return huffval[i] ;
}

int JpegHuffmanTable::getMinCode (unsigned char l)
{
	assert (l < mincode.size()) ;
	assert (mincode[l] >= 0) ;
	return mincode[l] ;
}

int JpegHuffmanTable::getMaxCode (unsigned char l)
{
	assert (l < maxcode.size()) ;
	return maxcode[l] ;
}

unsigned int JpegHuffmanTable::getValPtr (unsigned char l)
{
	assert (l < valptr.size()) ;
	assert (valptr[l] >= 0) ;
	return ((unsigned int) valptr[l]) ;
}

void JpegHuffmanTable::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	splitByte (io->read8(), &tableclass, &tabledestid) ;
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		bits.push_back (io->read8()) ;
	}
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		for (unsigned char j = 0 ; j < bits[i] ; j++) {
			huffval.push_back (io->read8()) ;
		}
	}

	calcTables() ;
}

void JpegHuffmanTable::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->write8 (mergeByte (tableclass, tabledestid)) ;
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		io->write8 (bits[i]) ;
	}
	vector<unsigned char>::iterator p = huffval.begin() ;
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		for (unsigned char j = 0 ; j < bits[i] ; j++) {
			io->write8 (*p) ;
			p++ ;
		}
	}
}

void JpegHuffmanTable::calcTables ()
{
	// generate huffsize
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		for (unsigned char j = 0 ; j < bits[i] ; j++) {
			huffsize.push_back (i + 1) ;
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
	// FIXME - bei tables - off by one - wo fängt table an ??
	unsigned char i = 0 ;
	unsigned int j = 0 ;
	while (i <= 16) {
		if (bits[i] == 0) {
			maxcode.push_back (-1) ;

			// mincode, valptr should not be used for i anyway
			mincode.push_back (-1) ;
			valptr.push_back (-1) ;
		}
		else {
			valptr.push_back (j) ;
			mincode.push_back (huffcode[j]) ;
			j += bits[i] - 1 ;
			maxcode.push_back (huffcode[j]) ;
			j++ ;
		}
		i++ ;
	}
}
