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
#include <limits.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "binaryio.h"
#include "error.h"
#include "jpegbase.h"
#include "jpeghufftable.h"
#include "jpegrestart.h"
#include "jpegscan.h"
#include "jpegscanhdr.h"

JpegScan::JpegScan ()
	: JpegContainer()
{
	scanhdr = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;
}

JpegScan::JpegScan (BinaryIO *io)
	: JpegContainer()
{
	scanhdr = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;

	read (io) ;
}

JpegScan::JpegScan (JpegObject *p, BinaryIO *io)
	: JpegContainer (p)
{
	scanhdr = NULL ;
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
			cerr << "found DHT" << endl ;

			JpegHuffmanTable *hufft = new JpegHuffmanTable (io) ;

			if (hufft->getDestId() > 3) {
				throw CorruptJpegError (io, _("huffman table has destination specifier %u (0-3 is allowed)."), hufft->getDestId()) ;
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
		}
		else if (marker == JpegElement::MarkerDRI) {
			cerr << "found DRI" << endl ;
			appendObj (new JpegDefineRestartInterval (io)) ;
		}
		else if (marker == JpegElement::MarkerSOS) {
			cerr << "found SOS" << endl ;

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
			cerr << "found restart marker: " << hex << (unsigned int) marker << endl ;
			appendObj (new JpegElement (marker)) ;

			JpegEntropyCoded *ecs = new JpegEntropyCoded (this, io) ;
			appendObj (ecs) ;
			ECSegs.push_back (ecs) ;

			if ((marker = ecs->getTerminatingMarker()) != 0x00) {
				havemarker = true ;
			}
		}
		else if (marker == JpegElement::MarkerEOI) {
			cerr << "found EOI" << endl ;
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

void JpegScan::write (BinaryIO *io)
{
	recalcACTables() ;
	JpegContainer::write(io) ;
}

void JpegScan::recalcACTables (void)
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

	vector<JpegObject*> jpegobjs = getJpegObjects() ;
	
	for (vector<JpegObject*>::iterator i = jpegobjs.begin() ; i != jpegobjs.end() ; i++) {	
		if (JpegHuffmanTable *ht = dynamic_cast<JpegHuffmanTable*> (*i)) {
			if (ht->getClass() == JpegHuffmanTable::ACTABLE) {
				vector<unsigned int> codesize = calcCodeSize (freqs[ht->getDestId()]) ;
				vector<unsigned int> bits = calcBits (codesize) ;
				vector<unsigned int> huffval = calcHuffVal (codesize) ;

				ht->reset (bits, huffval) ;
			}
		}
	}
}

vector<unsigned int> JpegScan::calcCodeSize (vector<unsigned long> freq)
{
	vector<unsigned int> codesize(257) ;
	vector<long int> others(257) ;
	for (vector<long int>::iterator j = others.begin() ; j != others.end() ; j++) {
		*j = -1 ;
	}

	bool v2found = true ;
	while (v2found) {
		unsigned long minfreq = ULONG_MAX ;
		unsigned long v1 = 0 ;
		for (unsigned long j = 0 ; j < freq.size() ; j++) {
			if ((freq[j] > 0) && (freq[j] <= minfreq)) {
				minfreq = freq[j] ;
				v1 = j ;
			}
		}

		minfreq = ULONG_MAX ;
		unsigned long v2 = 0 ;
		v2found = false ;
		for (unsigned long j = 0 ; j < freq.size() ; j++) {
			if ((freq[j] > 0) && (freq[j] <= minfreq) && (j != v1)) {
				minfreq = freq[j] ;
				v2 = j ;
				v2found = true ;
			}
		}

		if (v2found) {
			freq[v1] += freq[v2] ;
			freq[v2] = 0 ;

			codesize[v1]++ ;
			while (others[v1] != -1) {
				v1 = others[v1] ;
				codesize[v1]++ ;
			}

			others[v1] = v2 ;

			codesize[v2]++ ;
			while (others[v2] != -1) {
				v2 = others[v2] ;
				codesize[v2]++ ;
			}
		}
	}

	return codesize ;
}

vector<unsigned int> JpegScan::calcBits (vector<unsigned int> codesize)
{
	vector<unsigned int> bits (33) ;

	for (unsigned int i = 0 ; i < 257 ; i++) {
		if (codesize[i] > 0) {
			bits[codesize[i]]++ ;
		}
	}

	// adjust bits
	unsigned int i = 32 ;
	while (i > 16) {
		// start
		if (bits[i] > 0) {
			unsigned int j = i - 1 ;

			do {
				j-- ;
			} while (!(bits[j] > 0)) ;

			bits[i] -= 2 ;
			bits[i - 1]++ ;
			bits[j + 1] += 2 ;
			bits[j]-- ;
		}
		else {
			i-- ;
			if (i == 16) {
				while (bits[i] == 0) {
					i-- ;
				}
				bits[i]-- ;
			}
		}
	}

	for (unsigned int i = 0 ; i < 16 ; i++) {
		bits[i] = bits[i + 1] ;
	}
	bits.erase (bits.begin() + 16, bits.end()) ;

	return bits ;
}

vector<unsigned int> JpegScan::calcHuffVal (vector<unsigned int> codesize)
{
	vector<unsigned int> huffval ;
	for (unsigned int i = 1 ; i <= 32 ; i++) {
		for (unsigned j = 0 ; j <= 255 ; j++) {
			if (codesize[j] == i) {
				huffval.push_back (j) ;
			}
		}
	}

	return huffval ;
}
