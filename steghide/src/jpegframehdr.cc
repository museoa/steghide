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

#include "common.h"
#include "jpegbase.h"
#include "jpegframehdr.h"

JpegFrameHeader::JpegFrameHeader (void)
	: JpegSegment()
{
	isread = false ;
	components = NULL ;
}

JpegFrameHeader::JpegFrameHeader (JpegMarker m, BinaryIO *io)
	: JpegSegment (m)
{
	isread = false ;
	components = NULL ;
	read (io) ;
}

JpegFrameHeader::~JpegFrameHeader ()
{
	if (components != NULL) {
		delete components ;
	}
}

void JpegFrameHeader::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	precision = io->read8() ;
	Ysize = io->read16_be() ;
	Xsize = io->read16_be() ;
	numcomponents = io->read8() ;
	components = new Component[numcomponents] ;
	for (unsigned char i = 0 ; i < numcomponents ; i++) {
		components[i].id = io->read8() ;
		splitByte (io->read8(), &components[i].horizsampling, &components[i].vertsampling) ;
		components[i].quanttable = io->read8() ;
	}

	isread = true ;
}

void JpegFrameHeader::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->write8 (precision) ;
	io->write16_be (Ysize) ;
	io->write16_be (Xsize) ;
	io->write8 (numcomponents) ;
	for (unsigned char i = 0 ; i < numcomponents ; i++) {
		io->write8 (components[i].id) ;
		io->write8 (mergeByte (components[i].horizsampling, components[i].vertsampling)) ;
		io->write8 (components[i].quanttable) ;
	}
}

unsigned char JpegFrameHeader::getNumComponents (void)
{
	assert (isread) ;
	return numcomponents ;
}

unsigned char JpegFrameHeader::getHorizSampling (unsigned char c)
{
	assert (isread) ;
	assert (c < getNumComponents()) ;
	return components[c].horizsampling ;
}

unsigned char JpegFrameHeader::getVertSampling (unsigned char c)
{
	assert (isread) ;
	assert (c < getNumComponents()) ;
	return components[c].vertsampling ;
}
