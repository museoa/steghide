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

#include <iostream>

#include "cvrstgfile.h"
#include "jpegbase.h"
#include "jpegfile.h"

JpegFile::JpegFile ()
	: CvrStgFile()
{
	frame = NULL ;
}

JpegFile::JpegFile (BinaryIO *io)
	: CvrStgFile()
{
	frame = NULL ;
	read (io) ;
}

JpegFile::~JpegFile ()
{
	if (frame != NULL) {
		delete frame ;
	}
}

void JpegFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	// SOI has been read in guesff
	frame = new JpegFrame (io) ;
	// EOI is read in JpegFrame
}

void JpegFile::write ()
{
	CvrStgFile::write() ;

	JpegElement SOI (JpegElement::MarkerSOI) ;
	SOI.write (getBinIO()) ;
	frame->write (getBinIO()) ;
	JpegElement EOI (JpegElement::MarkerEOI) ;
	EOI.write (getBinIO()) ;
}

unsigned long JpegFile::getCapacity (void) const
{
	return frame->getCapacity() ;
}

void JpegFile::embedBit (unsigned long pos, int bit)
{
	assert (pos < getCapacity()) ;
	assert (bit == 0 || bit == 1) ;

	frame->embedBit (pos, bit) ;
}

int JpegFile::extractBit (unsigned long pos) const
{
	assert (pos < getCapacity()) ;

	return frame->extractBit (pos) ;
}