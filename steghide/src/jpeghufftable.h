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
 * \brief a huffman table in a jpeg file
 *
 * This class represents a huffman table as well as the jpeg segment containing
 * a huffman table definition. Consequently, it contains methods for
 * reading/writing this jpeg segment as well as methods for accessing and
 * working with the huffman table.
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

	/**
	 * create a new huffman table from the bits and the huffval arrays
	 * \param b the bits array (going from 0 to 15)
	 * \param hv the huffval array (starting at 0)
	 **/
	void reset (vector<unsigned int> b, vector<unsigned int> hv) ;

	/**
	 * get the number of codes with length l
	 * \param l the code length (1 <= l <= 16)
	 **/
	unsigned int getBits (unsigned int l) ;

	/**
	 * get the i-th symbol value
	 * \param i (0 <= i)
	 **/
	unsigned int getHuffVal (unsigned int i) ;

	/**
	 * get the length of the i-th code
	 * \param i (0 <= i)
	 **/
	unsigned int getHuffSize (unsigned int i) ;

	/**
	 * get the i-th huffman code (corresponding to the length getHuffSize(i))
	 * \param i (0 <= i)
	 **/
	unsigned int getHuffCode (unsigned int i) ;

	/**
	 * get the smallest code value of length l
	 * \param l the code length (1 <= l <= 16)
	 **/
	int getMinCode (unsigned int l) ;

	/**
	 * get the largest code value of length l
	 * \param l the code length (1 <= l <= 16)
	 **/
	int getMaxCode (unsigned int l) ;

	/**
	 * get the index of the start of the values in huffval which are decoded by words of length l
	 * \param l the code length (1 <= l <= 16)
	 **/
	int getValPtr (unsigned int l) ;

	/**
	 * get the size of the huffman code representing the value v
	 * \param v a value to be huffman encoded (0 <= v)
	 **/
	unsigned int getEHuffSize (unsigned int v) ;

	/**
	 * get the huffman code representing the value v
	 * \param v a value to be huffman encoded (0 <= v)
	 **/
	unsigned int getEHuffCode (unsigned int v) ;

	/**
	 * get the size of the huffman code + additional bit field returned by getXHuffCode
	 * \param v the diff value of the DC coefficient
	 **/
	unsigned int getXHuffSize (int v) ;

	/**
	 * get the huffman code + additional bit field for fully specifying a DC coefficient
	 * \param v the diff value of the DC coefficient
	 **/
	unsigned long getXHuffCode (int v) ;

#ifdef DEBUG
	void print (void) ;
	void printBits (void) ;
	void printHuffVal (void) ;
	void printHuffSize (void) ;
	void printHuffCode (void) ;
	void printMinCode (void) ;
	void printMaxCode (void) ;
	void printValPtr (void) ;
	void printEHuffSize (void) ;
	void printEHuffCode (void) ;
#endif

	private:
	static const unsigned char Len_bits = 16 ;

	/**
	 * calculates the tables huffsize, huffcode, mincode, maxcode, valptr, ehuffsize and ehuffcode
	 **/
	void calcTables (void) ;

	public:
	/**
	 * returns the difference magnitude category for the diff value of a DC coefficient
	 * \param v the diff value of the DC coefficient
	 **/
	unsigned int csize (int v) ;

	private:
	unsigned char tableclass ;
	unsigned char tabledestid ;
	vector<unsigned int> bits ;
	vector<unsigned int> huffval ;
	/// contains a list of code lengths
	vector<unsigned int> huffsize ;
	/// contains the codes corresponding to the code lengths in huffsize 
	vector<unsigned int> huffcode ;
	/// mincode[l] contains the smallest code value for the length l
	vector<int> mincode ;
	/// maxcode[l] contains the largest code value for the length l
	vector<int> maxcode ;
	/// valptr[l] contains the index to the start of the list of values in huffval which are decoded by code word of length l
	vector<int> valptr ;
	/// ehuffsize[v] contains the size of the huffman code representing the value v
	vector<unsigned int> ehuffsize ;
	/// ehuffcode[v] contains the huffman code representing the value v
	vector<unsigned int> ehuffcode ;
} ;

#endif // ndef SH_JPEGHUFFTABLE_H
