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

#ifndef SH_JPEGBASE_H
#define SH_JPEGBASE_H

#include <vector>

#include "BinaryIO.h"
#include "common.h"
#include "CvrStgObject.h"

/**
 * \class JpegObject
 * \brief any object in a jpeg file
 *
 * This class serves as an abstract base class for every object in a jpeg file.
 **/
class JpegObject {
	public:
	JpegObject (void) ;	
	JpegObject (JpegObject *p) ;

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

	JpegObject *getParent (void) const ;
	void setParent (JpegObject *p) ;
	bool issetParent (void) const ;

	protected:
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
	JpegObject *parent ;
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
	/// APP1 marker
	static const JpegMarker MarkerAPP1 = 0xE1 ;
	/// APP2 marker
	static const JpegMarker MarkerAPP2 = 0xE2 ;
	/// APP3 marker
	static const JpegMarker MarkerAPP3 = 0xE3 ;
	/// APP4 marker
	static const JpegMarker MarkerAPP4 = 0xE4 ;
	/// APP5 marker
	static const JpegMarker MarkerAPP5 = 0xE5 ;
	/// APP6 marker
	static const JpegMarker MarkerAPP6 = 0xE6 ;
	/// APP7 marker
	static const JpegMarker MarkerAPP7 = 0xE7 ;
	/// APP8 marker
	static const JpegMarker MarkerAPP8 = 0xE8 ;
	/// APP9 marker
	static const JpegMarker MarkerAPP9 = 0xE9 ;
	/// APP10 marker
	static const JpegMarker MarkerAPP10 = 0xEA ;
	/// APP11 marker
	static const JpegMarker MarkerAPP11 = 0xEB ;
	/// APP12 marker
	static const JpegMarker MarkerAPP12 = 0xEC ;
	/// APP13 marker
	static const JpegMarker MarkerAPP13 = 0xED ;
	/// APP14 marker
	static const JpegMarker MarkerAPP14 = 0xEE ;
	/// APP15 marker
	static const JpegMarker MarkerAPP15 = 0xEF ;
	/// comment marker
	static const JpegMarker MarkerCOM = 0xFE ;
	/// end of image marker
	static const JpegMarker MarkerEOI = 0xD9 ;
	/// define quantization table marker
	static const JpegMarker MarkerDQT = 0xDB ;
	/// start of frame 0 (baseline DCT) marker
	static const JpegMarker MarkerSOF0 = 0xC0 ;
	/// start of frame 2 (progressive DCT) marker
	static const JpegMarker MarkerSOF2 = 0xC2 ;
	/// start of hierachical sequence of frames marker
	static const JpegMarker MarkerDHP = 0xDE ;
	/// define huffman table marker
	static const JpegMarker MarkerDHT = 0xC4 ;
	/// start of scan marker
	static const JpegMarker MarkerSOS = 0xDA ;
	/// define restart interval marker
	static const JpegMarker MarkerDRI = 0xDD ;
	/// restart marker 0
	static const JpegMarker MarkerRST0 = 0xD0 ;
	/// restart marker 7
	static const JpegMarker MarkerRST7 = 0xD7 ;

	JpegElement (void) ;
	JpegElement (JpegMarker m) ;
	JpegElement (JpegObject *p, JpegMarker m) ;
	virtual ~JpegElement (void) ;

	void read (BinaryIO *io) ;

	/**
	 * write the marker
	 * \param io BinaryIO object to write marker to
	 **/
	void write (BinaryIO *io) ;	

	protected:
	/**
	 * return the marker of this JpegElement
	 * \return the 1-byte marker code
	 **/
	JpegMarker getMarker (void) ;

	/**
	 * set the marker of this JpegElement
	 * \param m the 1-byte marker code
	 **/
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
	JpegSegment (JpegObject *p, JpegMarker m) ;
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
	JpegContainer (JpegObject *p) ;
	virtual ~JpegContainer (void) ;

	/**
	 * get all objects of this container
	 * \return a std::vector containing all JpegObjects of this container
	 **/
	std::vector<JpegObject*> getJpegObjects (void) ;

	/**
	 * get all objects that can hold embedded data of this container
	 * \return a std::vector containing all CvrStgObjects of this container
	 **/
	std::vector<CvrStgObject*> getCvrStgObjects (void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (SamplePos pos) const ;

	protected:
	/**
	 * appends a JpegObject
	 * \param o the jpeg object to append
	 *
	 * This function appends o to the std::vector jpegobjs and if o is a CvrStgObject
	 * also appends it to the std::vector cvrstgobjs.
	 **/
	void appendObj (JpegObject *o) ;

	/**
	 * clears the std::vector of jpeg objects and the std::vector of cvrstg objects
	 **/
	void clearObjs (void) ;

	private:
	CvrStgObject *calcCvrStgObject (SamplePos *pos) const ;

	/**
	 * contains all JpegObjects in this JpegContainer
	 * (in the same order as they appeared in the original file)
	 **/
	std::vector<JpegObject*> jpegobjs ;

	/**
	 * contains all CvrStgObjects in this JpegContainer
	 * (in the same order as they appeared in the original file)
	 **/
	std::vector<CvrStgObject*> cvrstgobjs ;
} ;

#endif // ndef SH_JPEGBASE_H
