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

#ifndef SH_JPEGELEMENTS_H
#define SH_JPEGELEMENTS_H

#include "binaryio.h"

typedef unsigned char JpegMarker ;

/**
 * \class JpegElement
 * \brief an element of a jpeg file
 * 
 * Every object in a jpeg file that starts with (or consists solely of) a marker.
 **/
class JpegElement {
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

	JpegElement (void) ;
	JpegElement (JpegMarker m) ;
	virtual ~JpegElement (void) ;

	/**
	 * write the marker
	 * \param io BinaryIO object to write marker to
	 **/
	virtual void write (BinaryIO *io) ;	

	protected:
	JpegMarker getMarker (void) ;
	void setMarker (JpegMarker m) ;

	private:
	/// is true iff marker has been set
	bool marker_isset ;
	JpegMarker marker ;
} ;

#endif // ndef SH_JPEGELEMENTS_H
