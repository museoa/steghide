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

#include <string>

#include "binaryio.h"
#include "error.h"
#include "jpegelement.h"
#include "jpegsegment.h"
#include "jpegjfifapp0.h"

JpegJFIFAPP0::JpegJFIFAPP0 ()
	: JpegSegment (JpegElement::MarkerAPP0)
{
	thumbnail = NULL ;
}

JpegJFIFAPP0::JpegJFIFAPP0 (BinaryIO *io)
	: JpegSegment (JpegElement::MarkerAPP0)
{
	thumbnail = NULL ;	// FIXME - überall sonst: diese Zeile durch JpegJFIFAPP0() zu ersetzen erzeugt segfault (d.h. initialisiert NICHT!! thumbnail)
	read (io) ;
}

JpegJFIFAPP0::~JpegJFIFAPP0 ()
{
	if (thumbnail != NULL) {
		buffree (thumbnail) ;
	}
}

void JpegJFIFAPP0::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	identifier = io->readstring (4) ;
	unsigned char zerobyte = io->read8() ;
	if (identifier != "JFIF" || zerobyte != 0) {
		throw UnSupFileFormat (io) ;
	}

	version = io->read16_be() ;
	// FIXME check if compatible version
	units = io->read8() ;
	Xdensity = io->read16_be() ;
	Ydensity = io->read16_be() ;
	Xthumbnail = io->read8() ;
	Ythumbnail = io->read8() ;
	// FIXME - ?? in thumbnail auch verstecken ??
	// ev. andere Datenstruktur (nicht BUFFER, ähnlich bmp)

	unsigned int n = Xthumbnail * Ythumbnail ; 
	if (thumbnail != NULL) {
		buffree (thumbnail) ;
	}
	thumbnail = bufcreate (3 * n) ;
	for (unsigned int i = 0 ; i < n ; i++) {
		// R, G and B component
		bufsetbyte (thumbnail, i, io->read8()) ;
		bufsetbyte (thumbnail, i, io->read8()) ;
		bufsetbyte (thumbnail, i, io->read8()) ;
	}
}

void JpegJFIFAPP0::write (BinaryIO *io)
{
	JpegSegment::write (io) ;

	io->writestring (identifier) ;
	io->write8 (0) ;
	io->write16_be (version) ;
	io->write8 (units) ;
	io->write16_be (Xdensity) ;
	io->write16_be (Ydensity) ;
	io->write8 (Xthumbnail) ;
	io->write8 (Ythumbnail) ;
	unsigned int n = Xthumbnail * Ythumbnail ;
	for (unsigned int i = 0 ; i < n ; i++) {
		io->write8 (bufgetbyte (thumbnail, i)) ;
		io->write8 (bufgetbyte (thumbnail, i)) ;
		io->write8 (bufgetbyte (thumbnail, i)) ;
	}
}
