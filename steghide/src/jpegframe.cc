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
#include "jpegcomment.h"
#include "jpegframe.h"
#include "jpegframehdr.h"
#include "jpegjfifapp0.h"
#include "jpegscan.h"
#include "jpegunsupseg.h"

JpegFrame::JpegFrame ()
	: JpegContainer()
{
	framehdr = NULL ;
}

JpegFrame::JpegFrame (BinaryIO *io)
	: JpegContainer()
{
	framehdr = NULL ;
	read (io) ;
}

JpegFrame::~JpegFrame ()
{
}

JpegFrameHeader *JpegFrame::getFrameHeader ()
{
	assert (framehdr) ;
	return framehdr ;
}

//DEBUG
void JpegFrame::read (BinaryIO *io)
{
	JpegContainer::read (io) ;

	bool eoifound = false ;
	while (!eoifound) {
		if (io->read8() != 0xff) {
			throw CorruptJpegError (io, _("could not find start of marker (0xff).")) ;
		}
		JpegMarker marker ;
		while ((marker = io->read8()) == 0xff)
			;

		if (marker == JpegElement::MarkerAPP0) {
			cerr << "found APP0" << endl ;
			appendObj (new JpegJFIFAPP0 (io)) ;
		}
		else if (marker == JpegElement::MarkerCOM) {
			cerr << "found COM" << endl ;
			appendObj (new JpegComment (io)) ;
		}
#if 0
		else if (marker == JpegElement::MarkerDQT) {
			cerr << "found DQT" << endl ;
			appendObj (new JpegQuantizationTable (io)) ;
		}
#endif
		else if (marker == JpegElement::MarkerSOF0) {
			cerr << "found SOF0" << endl ;
			framehdr = new JpegFrameHeader (marker, io) ;
			appendObj (framehdr) ;

			// TODO - support more than one scan
			JpegScan *scan = new JpegScan (this, io) ;
			appendObj (scan) ;
			if (scan->getTerminatingMarker() == JpegElement::MarkerEOI) {
				eoifound = true ;
			}
		}
		else if (((marker >= JpegElement::MarkerAPP1) && (marker <= JpegElement::MarkerAPP15)) ||
				(marker == JpegElement::MarkerDQT)) {
			cerr << "found unsupported: " << hex << (unsigned int) marker << endl ;
			appendObj (new JpegUnsupportedSegment (marker, io)) ;
		}
		else {
			if (io->is_std()) {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file on standard input while reading frame."), marker) ;
			}
			else {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file \"%s\" while reading frame."), marker, io->getName().c_str()) ;
			}
		}
	}

	return ;
}
