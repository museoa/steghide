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

#ifndef SH_JPEGFRAME_H
#define SH_JPEGFRAME_H

#include "binaryio.h"
#include "jpegbase.h"
#include "jpegframehdr.h"
#include "jpeghufftable.h"
#include "jpegscan.h"

// declared here to prevent circulating #includes
class JpegFile ;

/**
 * \class JpegFrame
 * \brief a jpeg frame
 **/
class JpegFrame : public JpegContainer {
	public:
	JpegFrame (void) ;
	JpegFrame (JpegFile *f, BinaryIO *io) ;
	~JpegFrame (void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	/**
	 * get the frame header of this frame
	 **/
	JpegFrameHeader *getFrameHeader (void) ;

	/**
	 * get the DC huffman table for a given DC table destination specifier
	 * \param ds DC table destination specifier
	 * \return the DC huffman table corresponding to ds
	 **/
	JpegHuffmanTable *getDCTable (unsigned char ds) ;

	/**
	 * get the AC huffman table for a given AC table destination specifier
	 * \param ds AC table destination specifier
	 * \return the AC huffman table corresponding to ds
	 **/
	JpegHuffmanTable *getACTable (unsigned char ds) ;

	/**
	 * add a huffman table to this frame
	 **/
	void addHuffmanTable (JpegHuffmanTable *ht) ;

	JpegFile *getFile (void) ;

	private:
	void recalcACTables (vector<vector <unsigned long> > freqs) ;
	vector<unsigned int> calcCodeSize (vector<unsigned long> freq) ;
	vector<unsigned int> calcBits (vector<unsigned int> codesize) ;
	vector<unsigned int> calcHuffVal (vector<unsigned int> codesize) ;

	JpegFile *File ;
	JpegFrameHeader *framehdr ;
	JpegScan *scan ;
	vector<JpegHuffmanTable*> DCTables ;
	vector<JpegHuffmanTable*> ACTables ;
} ;

#endif // ndef SH_JEPGFRAME_H
