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
	SOI.write (BinIO) ;
	frame->write (BinIO) ;
	JpegElement EOI (JpegElement::MarkerEOI) ;
	EOI.write (BinIO) ;
}

unsigned long JpegFile::getCapacity (void)
{
	return frame->getCapacity() ;
}

void JpegFile::embedBit (unsigned long pos, int bit)
{
	frame->embedBit (pos, bit) ;
	return ;
}

int JpegFile::extractBit (unsigned long pos)
{
	return frame->extractBit (pos) ;
}
