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

#ifndef SH_JPEGENTROPYCODED_H
#define SH_JPEGENTROPYCODED_H

#include "binaryio.h"
#include "cvrstgobject.h"
#include "jpegbase.h"
#include "jpeghufftable.h"

/**
 * \class JpegEntropyCoded
 * \brief an entropy (huffman)-coded segment of a jpeg file
 **/
class JpegEntropyCoded : public JpegObject, public CvrStgObject {
	public:
	JpegEntropyCoded (void) ;
	JpegEntropyCoded (BinaryIO *io) ;
	JpegEntropyCoded (JpegObject *p, BinaryIO *io) ;
	virtual ~JpegEntropyCoded (void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	unsigned long getCapacity (void) const ;
	void embedBit (unsigned long pos, int bit) ;
	int extractBit (unsigned long pos) const ;

	vector<vector<unsigned long> > getFreqs (void) ;

	JpegMarker getTerminatingMarker (void) ;

	private:
	/**
	 * read a bit of encoded jpeg data
	 * \param io the jpeg stream
	 * \return the next bit
	 **/
	int readbit (BinaryIO *io) ;

	/**
	 * write a bit of encoded jpeg data
	 * \param io the jpeg stream
	 * \param bit the bit to be written
	 **/
	void writebit (BinaryIO *io, int bit) ;

	/**
	 * decode and return an 8-bit value
	 * \param io the jpeg stream
	 * \param the huffman table to be used for decoding this value
	 * \return the decoded value
	 **/
	unsigned char decode (BinaryIO *io, JpegHuffmanTable *ht) ;

	/**
	 * read the next nbits bits from io into low order bits of return value, msb first
	 * \param io the jpeg stream
	 * \param nbits number of bits to read
	 * \return the next nbits from io
	 **/
	int receive (BinaryIO *io, unsigned char nbits) ;

	int extend (int val, unsigned char t) ;

	unsigned int csize (int ac) ;

	/**
	 * encode and write an ac coefficient
	 * \param io the jpeg stream
	 * \param ht the huffman table to be used for encoding this coefficient
	 * \param r the runlength of zero coefficients
	 * \param coeff the coefficient to be encoded and written
	 **/
	void encode (BinaryIO *io, JpegHuffmanTable *ht, unsigned int r, int coeff) ;

	/**
	 * write the n lower order bits of v to io
	 * \param io the jpeg stream
	 * \param v the value to be written
	 * \param n the number of bits to write
	 **/
	void writebits (BinaryIO *io, unsigned long v, unsigned int n) ;

	// contains the DCT coefficients; for i % 64 == 0 we have a DC, else an AC coefficient in dctcoeffs[i]
	vector<int> dctcoeffs ;
	/// contains (the rest of) a byte read in readbit
	unsigned char readbyte ;
	/// the number of (original jpeg) bits in readbyte
	unsigned int readbytecontains ;

	unsigned char writebyte ;
	unsigned int writebytecontains ;

	/// the marker code that terminated the data from this entropy coded segment
	JpegMarker termmarker ;
	/// is true iff the read method is in a state where a terminating marker may appear
	bool termclean ;
} ;

/**
 * \class JpegMarkerFound
 * \brief is thrown when a marker terminating the entropy-coded data has been found
 **/
class JpegMarkerFound {
	public:
	JpegMarkerFound (void) { mcode = 0x00 ; } ;
	JpegMarkerFound (unsigned char m) { mcode = m ; } ;

	JpegMarker getMarkerCode (void) { return mcode ; } ;

	private:
	JpegMarker mcode ;
} ;

#endif // ndef SH_JPEGENTROPYCODED_H
