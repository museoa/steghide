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

void JpegHuffmanTable::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	splitByte (io->read8(), &tableclass, &tableid) ;
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		bits.push_back (io->read8()) ;
	}
	for (unsigned char i = 0 ; i < Len_bits ; i++) {
		for (unsigned char j = 0 ; j < bits[i] ; j++) {
			huffval.push_back (io->read8()) ;
		}
	}

	calcTable() ;
}

void JpegHuffmanTable::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->write8 (mergeByte (tableclass, tableid)) ;
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

void JpegHuffmanTable::calcTable ()
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

	return ;
}
