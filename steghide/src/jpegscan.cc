/*
 * steghide 0.5.1 - a steganography program
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

#include <vector>
#include <climits>

#include "binaryio.h"
#include "common.h"
#include "error.h"
#include "jpegbase.h"
#include "jpegframe.h"
#include "jpeghufftable.h"
#include "jpegrestart.h"
#include "jpegscan.h"
#include "jpegscanhdr.h"

JpegScan::JpegScan ()
	: JpegContainer()
{
	scanhdr = NULL ;
}

JpegScan::JpegScan (BinaryIO *io)
	: JpegContainer()
{
	scanhdr = NULL ;
	read (io) ;
}

JpegScan::JpegScan (JpegObject *p, BinaryIO *io)
	: JpegContainer (p)
{
	scanhdr = NULL ;
	read (io) ;
}

JpegScan::~JpegScan ()
{
}

JpegMarker JpegScan::getTerminatingMarker ()
{
	return termmarker ;
}

JpegScanHeader *JpegScan::getScanHeader ()
{
	assert (scanhdr) ;
	return scanhdr ;
}

void JpegScan::read (BinaryIO *io)
{
	bool scanread = false ;
	bool havemarker = false ;
	JpegMarker marker ;

	while (!scanread) {
		if (!havemarker) {
			if (io->read8() != 0xff) {
				throw CorruptJpegError (io, _("could not find start of marker (0xff).")) ;
			}
			while ((marker = io->read8()) == 0xff)
				;
		}

		havemarker = false ;
		if (marker == JpegElement::MarkerDHT) {
			JpegHuffmanTable *hufft = NULL ;
			unsigned int lengthremaining = UINT_MAX ;
			do {
				hufft = new JpegHuffmanTable (io, lengthremaining) ;
				if (hufft->getDestId() > 3) {
					throw CorruptJpegError (io, _("huffman table has destination specifier %u (0-3 is allowed)."), hufft->getDestId()) ;
				}
				appendObj (hufft) ;

				JpegFrame *p_frame = (JpegFrame *) getParent() ;
				p_frame->addHuffmanTable (hufft) ;

				lengthremaining = hufft->getLengthRemaining() ;
			} while (lengthremaining > 0) ;
		}
		else if (marker == JpegElement::MarkerDRI) {
			appendObj (new JpegDefineRestartInterval (io)) ;
		}
		else if (marker == JpegElement::MarkerSOS) {
			scanhdr = new JpegScanHeader (io) ;
			appendObj (scanhdr) ;

			JpegEntropyCoded *ecs = new JpegEntropyCoded (this, io) ;
			appendObj (ecs) ;
			ECSegs.push_back (ecs) ;

			if ((marker = ecs->getTerminatingMarker()) != 0x00) {
				havemarker = true ;
			}
		}
		else if ((marker >= JpegElement::MarkerRST0) && (marker <= JpegElement::MarkerRST7)) {
			appendObj (new JpegElement (marker)) ;

			JpegEntropyCoded *ecs = new JpegEntropyCoded (this, io) ;
			appendObj (ecs) ;
			ECSegs.push_back (ecs) ;

			if ((marker = ecs->getTerminatingMarker()) != 0x00) {
				havemarker = true ;
			}
		}
		else if (marker == JpegElement::MarkerEOI) {
			termmarker = JpegElement::MarkerEOI ;
			scanread = true ;
		}
		else {
			if (io->is_std()) {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file on standard input while reading scan."), marker) ;
			}
			else {
				throw SteghideError (_("encountered unknown marker code 0x%x in jpeg file \"%s\" while reading scan."), marker, io->getName().c_str()) ;
			}
		}
	}
}

vector<vector <unsigned long> > JpegScan::getFreqs ()
{
	vector<vector <unsigned long> > freqs = ECSegs[0]->getFreqs() ;
	for (vector<JpegEntropyCoded*>::iterator i = ECSegs.begin() + 1 ; i != ECSegs.end() ; i++) {
		vector<vector <unsigned long> > addfreqs = (*i)->getFreqs() ;
		for (unsigned int j = 0 ; j < addfreqs.size() ; j++) {
			for (unsigned int k = 0 ; k < 256 ; k++) {
				// don't touch freqs[j][256], must remain 1
				freqs[j][k] += addfreqs[j][k] ;
			}
		}
	}

	return freqs ;
}


