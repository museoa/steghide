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

#include <vector>

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
	scanhdr = NULL ;
	ecs = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;
}

JpegScan::JpegScan (BinaryIO *io)
	: JpegContainer()
{
	scanhdr = NULL ;
	ecs = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;

	read (io) ;
}

JpegScan::JpegScan (JpegObject *p, BinaryIO *io)
	: JpegContainer (p)
{
	scanhdr = NULL ;
	ecs = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;

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

JpegHuffmanTable *JpegScan::getDCTable (unsigned char ds)
{
	assert (ds < 4) ;
	assert (DCTables[ds]) ;
	return DCTables[ds] ;
}

JpegHuffmanTable *JpegScan::getACTable (unsigned char ds)
{
	assert (ds < 4) ;
	assert (ACTables[ds]) ;
	return ACTables[ds] ;
}

//DEBUG
void JpegScan::read (BinaryIO *io)
{
	bool scanread = false ;
	unsigned char marker[2] ;
	bool havemarker = false ;

	while (!scanread) {
		if (!havemarker) {
			if ((marker[0] = io->read8()) != 0xff) {
				if (io->is_std()) {
					throw SteghideError (_("corrupt jpeg file on standard input. could not find start of marker (0xff).")) ;
				}
				else {
					throw SteghideError (_("corrupt jpeg file \"%s\". could not find start of marker (0xff)."), io->getName().c_str()) ;
				}
			}
			marker[1] = io->read8() ;
		}

		JpegHuffmanTable *hufft = NULL ;
		havemarker = false ;
		switch (marker[1]) {
			case JpegElement::MarkerDHT:
			cerr << "found DHT" << endl ;

			hufft = new JpegHuffmanTable (io) ;

			// FIXME - für sequential nur >2 ! - ev. ändern
			// FIXME ev. Klasse CorruptJpegError
			if (hufft->getDestId() > 3) {
				if (io->is_std()) {
					throw SteghideError (_("corrupt jpeg file on standard input. Huffman table has destination identifier %u (4 is the maximum)."), hufft->getDestId()) ;
				}
				else {
					throw SteghideError (_("corrupt jpeg file \"%s\". Huffman table has destination identifier %u (4 is the maximum)."), io->getName().c_str(), hufft->getDestId()) ;
				}
			}
			switch (hufft->getClass()) {
				case JpegHuffmanTable::DCTABLE:
				DCTables[hufft->getDestId()] = hufft ;
				break ;

				case JpegHuffmanTable::ACTABLE:
				ACTables[hufft->getDestId()] = hufft ;
				break ;
			}
			
			appendObj (hufft) ;
			break ;

			case JpegElement::MarkerSOS:
			cerr << "found SOS" << endl ;

			scanhdr = new JpegScanHeader (io) ;
			appendObj (scanhdr) ;

			ecs = new JpegEntropyCoded (this, io) ;
			appendObj (ecs) ;

			marker[0] = 0xFF ;
			marker[1] = ecs->getTerminatingMarker() ; //ev. auf NULL überprüfen FIXME
			havemarker = true ;
			break ;

			case JpegElement::MarkerEOI:
			cerr << "found EOI" << endl ;
			termmarker = JpegElement::MarkerEOI ;
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
	}
}
