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

#include "jpegbase.h"
#include "jpegscanhdr.h"

JpegScanHeader::JpegScanHeader (void)
	: JpegSegment()
{
	isread = false ;
	components = NULL ;
}

JpegScanHeader::JpegScanHeader (BinaryIO *io)
	: JpegSegment (JpegElement::MarkerSOS)
{
	isread = false ;
	components = NULL ;
	read (io) ;
}

JpegScanHeader::~JpegScanHeader ()
{
	if (isread) {
		delete components ;
	}
}

void JpegScanHeader::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	numcomponents = io->read8() ;
	components = new Component[numcomponents] ;
	for (unsigned char i = 0 ; i < numcomponents ; i++) {
		components[i].id = io->read8() ;
		splitByte (io->read8(), &components[i].DCTable, &components[i].ACTable) ;
	}
	sospectral = io->read8() ;
	eospectral = io->read8() ;
	splitByte (io->read8(), &ah, &al) ;

	isread = true ;
}

void JpegScanHeader::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->write8 (numcomponents) ;
	for (unsigned char i = 0 ; i < numcomponents ; i++) {
		io->write8 (components[i].id) ;
		io->write8 (mergeByte (components[i].DCTable, components[i].ACTable)) ;
	}
	io->write8 (sospectral) ;
	io->write8 (eospectral) ;
	io->write8 (mergeByte (ah, al)) ;
}

unsigned char JpegScanHeader::getNumComponents ()
{
	assert (isread) ;
	return numcomponents ;
}

unsigned char JpegScanHeader::getDCDestSpec (unsigned char c)
{
	assert (isread) ;
	assert (c < getNumComponents()) ;
	return components[c].DCTable ;
}

unsigned char JpegScanHeader::getACDestSpec (unsigned char c)
{
	assert (isread) ;
	assert (c < getNumComponents()) ;
	return components[c].ACTable ;
}
