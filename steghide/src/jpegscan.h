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

#ifndef SH_JPEGSCAN_H
#define SH_JPEGSCAN_H

#include <vector>

#include "binaryio.h"
#include "jpegbase.h"
#include "jpegentropycoded.h"
#include "jpeghufftable.h"
#include "jpegscanhdr.h"

/**
 * \class JpegScan
 * \brief a jpeg scan
 **/
class JpegScan : public JpegContainer {
	public:
	JpegScan (void) ;
	JpegScan (BinaryIO *io) ;
	JpegScan (JpegObject *p, BinaryIO *io) ;
	~JpegScan (void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	JpegMarker getTerminatingMarker (void) ;
	JpegScanHeader *getScanHeader (void) ;

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

	protected:
#if 0
	/**
	 * is a restart interval defined ?
	 * \return true iff a DRI marker has been read
	 **/
	bool is_RIdefined (void) ;
#endif

	private:
	void recalcACTables (void) ;
	vector<unsigned int> calcCodeSize (vector<unsigned long> freq) ;
	vector<unsigned int> calcBits (vector<unsigned int> codesize) ;
	vector<unsigned int> calcHuffVal (vector<unsigned int> codesize) ;

	JpegScanHeader *scanhdr ;

	vector<JpegEntropyCoded*> ECSegs ;
	vector<JpegHuffmanTable*> DCTables ;
	vector<JpegHuffmanTable*> ACTables ;

	JpegMarker termmarker ;
} ;

#endif // ndef SH_JPEGSCAN_H
