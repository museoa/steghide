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

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "binaryio.h"
#include "jpegelement.h"
#include "jpegsegment.h"
#include "jpegquanttable.h"

JpegQuantizationTable::JpegQuantizationTable ()
	: JpegSegment (JpegElement::MarkerDQT)
{
}

JpegQuantizationTable::JpegQuantizationTable (BinaryIO *io)
	: JpegSegment (JpegElement::MarkerDQT)
{
	read (io) ;
}

JpegQuantizationTable::~JpegQuantizationTable ()
{
}

void JpegQuantizationTable::read (BinaryIO *io)
{
	JpegSegment::read (io) ;

	splitByte (io->read8(), &precision, &destination) ;
	if (precision != 0 && precision != 1) {
		if (io->is_std()) {
			throw SteghideError (_("The precision of a quantization table in the jpeg file on standard input is set to the invalid value %d."), precision) ;
		}
		else {
			throw SteghideError (_("The precision of a quantization table in the jpeg file \"%s\" is set to the invalid value %d."), io->getName().c_str(), precision) ;
		}
	}

	for (unsigned int i = 0 ; i < SizeQuantTable ; i++) {
		switch (precision) {
			case 0:
			quanttable[i] = (unsigned int) io->read8() ;
			break ;

			case 1:
			quanttable[i] = io->read16_be() ;
			break ;
		}
	}
}

void JpegQuantizationTable::write (BinaryIO *io)
{
	JpegSegment::write (io) ;
	
	io->write8 (mergeByte (precision, destination)) ;
	for (unsigned int i = 0 ; i < SizeQuantTable ; i++) {
		switch (precision) {
			case 0:
			io->write8 ((unsigned char) quanttable[i]) ;
			break ;

			case 1:
			io->write16_be (quanttable[i]) ;
			break ;
		}
	}
}
