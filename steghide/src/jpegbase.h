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

#ifndef SH_JPEGBASE_H
#define SH_JPEGBASE_H

#include <vector>

#include "binaryio.h"
#include "cvrstgobject.h"

/**
 * \class JpegObject
 * \brief any object in a jpeg file
 *
 * This class serves as an abstract base class to provide the
 * read/write interface for every object in a jpeg file.
 **/
class JpegObject {
	public:
	/**
	 * read this object from a jpeg stream
	 * \param io the jpeg stream
	 **/
	virtual void read (BinaryIO *io) = 0 ;

	/**
	 * write this object to a jpeg stream
	 * \param io the jpeg stream
	 **/
	virtual void write (BinaryIO *io) = 0 ;
} ;

typedef unsigned char JpegMarker ;

/**
 * \class JpegElement
 * \brief an element of a jpeg file
 * 
 * Every object in a jpeg file that starts with (or consists solely of) a marker.
 **/
class JpegElement : public JpegObject {
	public:
	/// start of image marker
	static const JpegMarker MarkerSOI = 0xD8 ;	
	/// APP0 marker
	static const JpegMarker MarkerAPP0 = 0xE0 ;
	/// comment marker
	static const JpegMarker MarkerCOM = 0xFE ;
	/// end of image marker
	static const JpegMarker MarkerEOI = 0xD9 ;
	/// define quantization table marker
	static const JpegMarker MarkerDQT = 0xDB ;
	/// start of frame 0 (baseline DCT) marker
	static const JpegMarker MarkerSOF0 = 0xC0 ;
	/// start of hierachical sequence of frames marker
	static const JpegMarker MarkerDHP = 0xDE ;
	/// define huffman table marker
	static const JpegMarker MarkerDHT = 0xC4 ;
	/// start of scan marker
	static const JpegMarker MarkerSOS = 0xDA ;

	JpegElement (void) ;
	JpegElement (JpegMarker m) ;
	virtual ~JpegElement (void) ;

	void read (BinaryIO *io) ;

	/**
	 * write the marker
	 * \param io BinaryIO object to write marker to
	 **/
	void write (BinaryIO *io) ;	

	protected:
	JpegMarker getMarker (void) ;
	void setMarker (JpegMarker m) ;

	private:
	/// is true iff marker has been set
	bool marker_isset ;
	/// the value of the marker of this element
	JpegMarker marker ;
} ;

/**
 * \class JpegSegment
 * \brief a marker segment in a jpeg file
 *
 * Every object in a jpeg file that starts with a marker but
 * contains also some additional fields.
 **/
class JpegSegment : public JpegElement {
	public:
	JpegSegment (void) ;
	JpegSegment (JpegMarker m) ;
	virtual ~JpegSegment (void) ;

	/**
	 * read the 2-byte length specification
	 * \param io the BinaryIO object to read the data from
	 **/
	void read (BinaryIO *io) ;

	/**
	 * write the marker and the 2-byte length specification 
	 * \param io the BinaryIO object to write the data to
	 **/
	void write (BinaryIO *io) ;

	protected:
	/**
	 * get the length of this jpeg marker segment
	 * \return the length
	 **/
	unsigned int getLength (void) ;

	/**
	 * set the length of this jpeg marker segment
	 * \param l the length
	 **/
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

/**
 * \class JpegContainer
 * \brief a jpeg container object
 *
 * Every object in a jpeg file that can contain one or more other jpeg objects.
 * It is possible that one or more of the contained objects can be used to embed data.
 **/
class JpegContainer : public JpegObject, public CvrStgObject {
	public:
	JpegContainer (void) ;
	virtual ~JpegContainer (void) ;

	vector<JpegObject*> getJpegObjects (void) ;
	vector<CvrStgObject*> getCvrStgObjects (void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	unsigned long getCapacity (void) ;
	void embedBit (unsigned long pos, int bit) ;
	int extractBit (unsigned long pos) ;

	protected:
	void appendObj (JpegObject *o) ;

	private:
	CvrStgObject *calcCvrStgObject (unsigned long *pos) ;

	vector<JpegObject*> jpegobjs ;
	vector<CvrStgObject*> cvrstgobjs ;
} ;

#endif // ndef SH_JPEGBASE_H
