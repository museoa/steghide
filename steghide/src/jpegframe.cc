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

#include <limits.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "jpegframe.h"
#include "jpegframehdr.h"
#include "jpegjfifapp0.h"
#include "jpegscan.h"
#include "jpegunusedseg.h"

JpegFrame::JpegFrame ()
	: JpegContainer()
{
	framehdr = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;
}

JpegFrame::JpegFrame (BinaryIO *io)
	: JpegContainer()
{
	framehdr = NULL ;
	ACTables = vector<JpegHuffmanTable*> (4) ;
	DCTables = vector<JpegHuffmanTable*> (4) ;

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

JpegHuffmanTable *JpegFrame::getDCTable (unsigned char ds)
{
	assert (ds < 4) ;
	assert (DCTables[ds]) ;
	return DCTables[ds] ;
}

JpegHuffmanTable *JpegFrame::getACTable (unsigned char ds)
{
	assert (ds < 4) ;
	assert (ACTables[ds]) ;
	return ACTables[ds] ;
}

void JpegFrame::addHuffmanTable (JpegHuffmanTable *ht)
{
	switch (ht->getClass()) {
		case JpegHuffmanTable::DCTABLE:
		DCTables[ht->getDestId()] = ht ;
		break ;

		case JpegHuffmanTable::ACTABLE:
		ACTables[ht->getDestId()] = ht ;
		break ;
	}
}

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
			appendObj (new JpegJFIFAPP0 (io)) ;
		}
		else if (marker == JpegElement::MarkerDHT) {
			JpegHuffmanTable *hufft = NULL ;
			unsigned int lengthremaining = UINT_MAX ;
			do {
				hufft = new JpegHuffmanTable (io, lengthremaining) ;
				if (hufft->getDestId() > 3) {
					throw CorruptJpegError (io, _("huffman table has destination specifier %u (0-3 is allowed)."), hufft->getDestId()) ;
				}
				appendObj (hufft) ;

				addHuffmanTable (hufft) ;

				lengthremaining = hufft->getLengthRemaining() ;
			} while (lengthremaining > 0) ;
		}
		else if (marker == JpegElement::MarkerSOF0) {
			framehdr = new JpegFrameHeader (marker, io) ;
			appendObj (framehdr) ;

			// TODO - support more than one scan
			scan = new JpegScan (this, io) ;
			appendObj (scan) ;
			if (scan->getTerminatingMarker() == JpegElement::MarkerEOI) {
				eoifound = true ;
			}
		}
		else if (((marker >= JpegElement::MarkerAPP1) && (marker <= JpegElement::MarkerAPP15)) ||
				(marker == JpegElement::MarkerDQT) ||
				(marker == JpegElement::MarkerCOM)) {
			appendObj (new JpegUnusedSegment (marker, io)) ;
		}
		else if (marker == JpegElement::MarkerSOF2) {
			// TODO - support progressive DCT
			if (io->is_std()) {
				throw SteghideError (_("the jpeg file on standard input uses progressive DCT mode which is not supported in this version.")) ;
			}
			else {
				throw SteghideError (_("the jpeg file \"%s\" uses progressive DCT mode which is not supported in this version."), io->getName().c_str()) ;
			}
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

void JpegFrame::write (BinaryIO *io)
{
	recalcACTables (scan->getFreqs()) ;
	JpegContainer::write (io) ;
}

void JpegFrame::recalcACTables (vector<vector <unsigned long> > freqs)
{
	for (vector<JpegHuffmanTable*>::iterator ht = ACTables.begin() ; ht != ACTables.end() ; ht++) {	
		if (*ht != NULL) {
			vector<unsigned int> codesize = calcCodeSize (freqs[(*ht)->getDestId()]) ;
			vector<unsigned int> bits = calcBits (codesize) ;
			vector<unsigned int> huffval = calcHuffVal (codesize) ;

			(*ht)->reset (bits, huffval) ;
		}
	}
}

vector<unsigned int> JpegFrame::calcCodeSize (vector<unsigned long> freq)
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

vector<unsigned int> JpegFrame::calcBits (vector<unsigned int> codesize)
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

vector<unsigned int> JpegFrame::calcHuffVal (vector<unsigned int> codesize)
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