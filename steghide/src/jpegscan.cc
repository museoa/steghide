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

#include "binaryio.h"
#include "error.h"
#include "jpeghufftable.h"
#include "jpegscan.h"
#include "jpegscanhdr.h"

JpegScan::JpegScan ()
	: JpegContainer()
{
}

JpegScan::JpegScan (BinaryIO *io)
	: JpegContainer()
{
	read (io) ;
}

JpegScan::~JpegScan ()
{
}

//DEBUG
void JpegScan::read (BinaryIO *io)
{
	bool scanread = false ;

	while (!scanread) {
		unsigned char marker[2] ;
		if ((marker[0] = io->read8()) != 0xff) {
			if (io->is_std()) {
				throw SteghideError (_("corrupt jpeg file on standard input. could not find start of marker (0xff).")) ;
			}
			else {
				throw SteghideError (_("corrupt jpeg file \"%s\". could not find start of marker (0xff)."), io->getName().c_str()) ;
			}
		}
		marker[1] = io->read8() ;

		JpegObject* next = NULL ;
		switch (marker[1]) {
			case JpegElement::MarkerDHT:
			cerr << "found DHT" << endl ;
			next = new JpegHuffmanTable (io) ;
			break ;

			case JpegElement::MarkerSOS:
			cerr << "found SOS" << endl ;
			next = new JpegScanHeader (io) ;
			// scan daten einlesen FIXME
			// von irgendwo daten nehmen wie lange - dann ende - EOI wird in JpegFrame gefunden
			scanread = true ;
			break ;

			default:
			if (io->is_std()) {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file on standard input while reading scan."), marker[1]) ;
			}
			else {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file \"%s\" while reading scan."), marker[1], io->getName().c_str()) ;
			}
			break ;
		}

		appendObj (next) ;
	}
}
