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

#ifndef SH_JPEGUNSUPSEG_H
#define SH_JPEGUNSUPSEG_H

#include <vector>

#include "binaryio.h"
#include "jpegbase.h"

/**
 * \class JpegUnusedSegment
 * \brief a jpeg segment that is not used by steghide
 **/
class JpegUnusedSegment : public JpegSegment {
	public:
	JpegUnusedSegment(void) ;
	JpegUnusedSegment(JpegMarker m) ;
	JpegUnusedSegment(JpegMarker m, BinaryIO *io) ;
	virtual ~JpegUnusedSegment(void) ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	private:
	std::vector<unsigned char> data ;
} ;

#endif //ndef SH_JPEGUNSUPSEG_H
