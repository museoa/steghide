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

#include <assert.h>

#include "binaryio.h"
#include "jpegelement.h"
#include "jpegsegment.h"

JpegSegment::JpegSegment ()
	: JpegElement()
{
	length_isset = false ;
}

JpegSegment::JpegSegment (JpegMarker m)
	: JpegElement(m)
{
	length_isset = false ;
}

JpegSegment::~JpegSegment ()
{
}

void JpegSegment::read (BinaryIO *io)
{
	setLength (io->read16_be()) ;
}

void JpegSegment::write (BinaryIO *io)
{
	JpegElement::write (io) ;
	io->write16_be (getLength()) ;
}

void JpegSegment::splitByte (unsigned char byte, unsigned char *high, unsigned char *low)
{
	*high = (0xF0 & byte) >> 4 ;
	*low = (0x0F & byte) ;
}

unsigned char JpegSegment::mergeByte (unsigned char high, unsigned char low)
{
	assert (high <= 15) ;
	assert (low <= 15) ;

	return ((high << 4) | low) ;
}

unsigned int JpegSegment::getLength (void)
{
	assert (length_isset) ;
	return length ;
}

void JpegSegment::setLength (unsigned int l)
{
	length = l ;
	length_isset = true ;
}
