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

#ifndef SH_JPEGHUFFTABLE_H
#define SH_JPEGHUFFTABLE_H

#include <vector>

#include "binaryio.h"
#include "jpegbase.h"

/**
 * \class JpegHuffmanTable
 * \brief a jpeg segment containing a huffman table specification
 **/
class JpegHuffmanTable : public JpegSegment {
	public:
	enum Class { DCTABLE, ACTABLE } ;

	JpegHuffmanTable (void) ;
	JpegHuffmanTable (BinaryIO *io) ;
	virtual ~JpegHuffmanTable (void) ;

	/**
	 * read a huffman table specification marker segment
	 * \param io the jpeg stream
	 **/
	void read (BinaryIO *io) ;

	/**
	 * write this object's huffman table into a marker segment
	 * \param io the jpeg stream
	 **/
	void write (BinaryIO *io) ;

	/**
	 * returns the class of this table
	 **/
	Class getClass (void) ;

	/**
	 * returns the table destination identifier
	 **/
	unsigned int getDestId (void) ;

	// FIXME - ??Datentypen
	int getMinCode (unsigned char l) ;
	int getMaxCode (unsigned char l) ;
	unsigned int getValPtr (unsigned char l) ;
	unsigned char getHuffVal (unsigned int i) ;

	private:
	static const unsigned char Len_bits = 16 ;

	void calcTables (void) ;

	unsigned char tableclass ;
	unsigned char tabledestid ;
	vector<unsigned char> bits ;
	vector<unsigned char> huffval ;

	// FIXME - ??Datentypen
	vector<unsigned char> huffsize ;
	vector<unsigned int> huffcode ;
	/// mincode[l] contains the smallest code value for the length l
	vector<int> mincode ;
	/// maxcode[l] contains the largest code value for the length l
	vector<int> maxcode ;
	/// valptr[l] contains the index to the start of the list of values in huffval which are decoded by code word of length l
	vector<int> valptr ;
} ;

#endif // ndef SH_JPEGHUFFTABLE_H
