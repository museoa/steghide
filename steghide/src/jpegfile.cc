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

#include <libintl.h>
#define _(S) gettext (S)

#include "binaryio.h"
#include "error.h"
#include "jpegcomment.h"
#include "jpegelement.h"
#include "jpegfile.h"
#include "jpegjfifapp0.h"
#include "jpegquanttable.h"

JpegFile::JpegFile ()
	: CvrStgFile()
{
	// empty
}

JpegFile::JpegFile (BinaryIO *io)
	: CvrStgFile()
{
	read (io) ;
}

JpegFile::~JpegFile ()
{
	for (vector<JpegElement*>::iterator i = elements.begin() ; i != elements.end() ; i++) {
		delete *i ;
	}
}

void JpegFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	// has been read in guesff
	elements.push_back (new JpegElement (JpegElement::MarkerSOI)) ;

	bool eoifound = false ;
	while (!eoifound) {
		unsigned char marker[2] ;
		if ((marker[0] = BinIO->read8()) != 0xff) {
			if (io->is_std()) {
				throw SteghideError (_("corrupt jpeg file on standard input. could not find start of marker (0xff).")) ;
			}
			else {
				throw SteghideError (_("corrupt jpeg file \"%s\". could not find start of marker (0xff)."), io->getName().c_str()) ;
			}
		}
		marker[1] = BinIO->read8() ;

		JpegElement* next = NULL ;
		switch (marker[1]) {
			case JpegElement::MarkerAPP0:
			next = new JpegJFIFAPP0 (BinIO) ;
			break ;

			case JpegElement::MarkerCOM:
			next = new JpegComment (BinIO) ;
			break ;

			case JpegElement::MarkerDQT:
			next = new JpegQuantizationTable (BinIO) ;
			break ;	

			case JpegElement::MarkerEOI:
			next = new JpegElement (JpegElement::MarkerEOI) ;
			eoifound = true ;
			break ;

			default:
			if (io->is_std()) {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file on standard input."), marker[1]) ;
			}
			else {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file \"%s\"."), marker[1], io->getName().c_str()) ;
			}
			break ;
		}

		elements.push_back (next) ;
	}

	// FIXME - check: first must be soi
}

void JpegFile::write ()
{
	CvrStgFile::write() ;

	for (vector<JpegElement*>::iterator i = elements.begin() ; i != elements.end() ; i++) {
		(*i)->write (BinIO) ;
	}
}

unsigned long JpegFile::getCapacity (void)
{
	return 0 ;
}

void JpegFile::embedBit (unsigned long pos, int bit)
{
}

int JpegFile::extractBit (unsigned long pos)
{
	return 0 ;
}
