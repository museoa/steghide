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

#ifndef SH_JPEGSEGMENTS_H
#define SH_JPEGSEGMENTS_H

#include "bufmanag.h"
#include "jpegelement.h"

/**
 * \class JpegSegment
 * \brief a marker segment in a jpeg file
 *
 * Every object in a jpeg file that starts with a marker but additionally
 * consists of at least a length specification (directly after the marker).
 **/
class JpegSegment : public JpegElement {
	public:
	JpegSegment (void) ;
	JpegSegment (JpegMarker m) ;
	virtual ~JpegSegment (void) ;

	/**
	 * read the marker and the 2-byte length specification
	 * \param io the BinaryIO object to read the data from
	 **/
	virtual void read (BinaryIO *io) ;

	/**
	 * write the marker and the 2-byte length specification 
	 * \param io the BinaryIO object to write the data to
	 **/
	void write (BinaryIO *io) ;

	protected:
	unsigned int getLength (void) ;
	void setLength (unsigned int l) ;

	/**
	 * splits a byte into its two halves
	 * \param byte the byte to split
	 * \param high the adress where the more significant half should be put
	 * \param low the adress where the less significant half should be put
	 *
	 * This function is useful when dealing with 4-bit fields in jpeg files.
	 **/
	void splitByte (unsigned char byte, unsigned char *high, unsigned char *low) ;

	/**
	 * merges two halves of a byte into one byte
	 * \param high the more significant half
	 * \param low the less significant half
	 * \return the merged byte
	 *
	 * This function is useful when dealing with 4-bit fields in jpeg files.
	 **/
	unsigned char mergeByte (unsigned char high, unsigned char low) ;

	private:
	bool length_isset ;
	unsigned int length ;
} ;


#endif // ndef SH_JPEGSEGMENTS_H
