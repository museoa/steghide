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

#ifndef SH_JPEGFRAMEHDR_H
#define SH_JPEGFRAMEHDR_H

#include "jpegbase.h"

/**
 * \class JpegFrameHeader
 * \brief the header of a frame
 **/
class JpegFrameHeader : public JpegSegment {
	public:
	JpegFrameHeader (void) ;
	JpegFrameHeader (JpegMarker m, BinaryIO *io) ;
	virtual ~JpegFrameHeader (void) ;

	/**
	 * read a frame header marker segment
	 * \param io the jpeg stream
	 **/
	void read (BinaryIO *io) ;

	/**
	 * write a frame header marker segment
	 * \param io the jpeg stream
	 **/
	void write (BinaryIO *io) ;

	private:
	struct Component {
		unsigned char id ;
		unsigned char horizsampling ;
		unsigned char vertsampling ;
		unsigned char quanttable ;
	} ;
	/// sample precision (in bits)
	unsigned char precision ;
	/// number of lines
	unsigned int Ysize ;
	/// number of samples per line
	unsigned int Xsize ;
	/// number of components in frame
	unsigned char numcomponents ;
	Component *components ;
} ;

#endif // ndef SH_JPEGFRAMEHDR_H
