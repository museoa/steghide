/*
 * steghide 0.4.6b - a steganography program
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

#include <vector>

#include "binaryio.h"
#include "jpegbase.h"
#include "jpegunusedseg.h"

JpegUnusedSegment::JpegUnusedSegment()
	: JpegSegment()
{
}

JpegUnusedSegment::JpegUnusedSegment(JpegMarker m)
	: JpegSegment(m)
{
}

JpegUnusedSegment::JpegUnusedSegment(JpegMarker m, BinaryIO *io)
	: JpegSegment(m)
{
	read(io) ;
}

JpegUnusedSegment::~JpegUnusedSegment()
{
}

void JpegUnusedSegment::read(BinaryIO *io)
{
	JpegSegment::read(io) ;
	data.clear() ;
	for (unsigned int i = 0 ; i < getLength() - 2 ; i++) {
		data.push_back (io->read8()) ;
	}
}

void JpegUnusedSegment::write (BinaryIO *io)
{
	JpegSegment::write (io) ;
	for (vector<unsigned char>::iterator i = data.begin() ; i != data.end() ; i++) {
		io->write8 (*i) ;
	}
}
