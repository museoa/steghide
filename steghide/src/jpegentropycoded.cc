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

#include "binaryio.h"
#include "common.h"
#include "error.h"
#include "jpegbase.h"
#include "jpegentropycoded.h"
#include "jpegframe.h"
#include "jpegframehdr.h"
#include "jpegsample.h"
#include "jpegscan.h"
#include "jpegscanhdr.h"

JpegEntropyCoded::JpegEntropyCoded ()
	: JpegObject(), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	writebyte = 0 ;
	writebytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;
}

JpegEntropyCoded::JpegEntropyCoded (BinaryIO *io)
	: JpegObject(), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	writebyte = 0 ;
	writebytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;

	read (io) ;
}

JpegEntropyCoded::JpegEntropyCoded (JpegObject *p, BinaryIO *io)
	: JpegObject (p), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	writebyte = 0 ;
	writebytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;

	read (io) ;
}

JpegEntropyCoded::~JpegEntropyCoded ()
{
}

vector<vector <unsigned long> > JpegEntropyCoded::getFreqs ()
{
	vector<vector<unsigned long> > freq ;

	JpegScan *p_scan = (JpegScan *) getParent() ;
	JpegScanHeader *p_scanhdr = (JpegScanHeader *) p_scan->getScanHeader() ;
	JpegFrame *p_frame = (JpegFrame *) p_scan->getParent() ;
	JpegFrameHeader *p_framehdr = (JpegFrameHeader *) p_frame->getFrameHeader() ;

	vector<unsigned int> dataunits ;
	vector<unsigned int> htdestspec ;
	unsigned int maxdestspec = 0 ;
	for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++) {
		dataunits.push_back (p_framehdr->getHorizSampling (comp) * p_framehdr->getVertSampling (comp)) ;
		htdestspec.push_back (p_scanhdr->getACDestSpec (comp)) ;
		if (htdestspec[comp] > maxdestspec) {
			maxdestspec = htdestspec[comp] ;
		}
	}

	for (unsigned int destspec = 0 ; destspec <= maxdestspec ; destspec++) {
		freq.push_back (vector<unsigned long> (257)) ;
		freq[destspec][256] = 1 ;
	}

	unsigned long unitstart = 0 ;
	assert (dctcoeffs.size() % 64 == 0) ;
	while (unitstart < dctcoeffs.size()) {
		for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++) {
			for (unsigned int du = 0 ; du < dataunits[comp] ; du++) {
				unsigned int r = 0 ;
				for (unsigned int k = 1 ; k < 64 ; k++) {
					if (dctcoeffs[unitstart + k] == 0) {
						if (k == 63) {
							freq[htdestspec[comp]][0x00]++ ;
							break ;
						}
						else {
							r++ ;
						}
					}
					else {
						while (r > 15) {
							r -= 16 ;
							freq[htdestspec[comp]][0xF0]++ ;
						}
						unsigned char rs = (r << 4) | csize (dctcoeffs[unitstart + k]) ;
						freq[htdestspec[comp]][rs]++ ;
						r = 0 ;
					}
				}

				unitstart += 64 ;
			}
		}
	}

	return freq ;
}

JpegMarker JpegEntropyCoded::getTerminatingMarker ()
{
	return termmarker ;
}

void JpegEntropyCoded::read (BinaryIO *io)
{
	assert (readbyte == 0) ;
	assert (readbytecontains == 0) ;
	assert (termmarker == 0x00) ;

	JpegScan *p_scan = (JpegScan *) getParent() ;
	JpegScanHeader *p_scanhdr = (JpegScanHeader *) p_scan->getScanHeader() ;
	JpegFrame *p_frame = (JpegFrame *) p_scan->getParent() ;
	JpegFrameHeader *p_framehdr = (JpegFrameHeader *) p_frame->getFrameHeader() ;

	unsigned long unitstart = 0 ;
	vector<int> prediction ;
	for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++) {
		prediction.push_back (0) ;
	}

	while (termmarker == 0x00) {
		// read one MCU
		try {
			termclean = true ;	// true if a marker can occur

			for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++)  {
				unsigned char compdataunits = p_framehdr->getHorizSampling (comp) * p_framehdr->getVertSampling (comp) ;
				JpegHuffmanTable *DCTable = p_frame->getDCTable (p_scanhdr->getDCDestSpec (comp)) ;
				JpegHuffmanTable *ACTable = p_frame->getACTable (p_scanhdr->getACDestSpec (comp)) ;

				for (unsigned char du = 0 ; du < compdataunits ; du++) {
					// decode DC
					unsigned char t = decode (io, DCTable) ;
					int diff = receive (io, t) ;
					diff = extend (diff, t) ;
					termclean = false ;

					for (unsigned char i = 0 ; i < 64 ; i++) {
						dctcoeffs.push_back (0) ;
					}
					dctcoeffs[unitstart] = prediction[comp] + diff ;
					prediction[comp] = dctcoeffs[unitstart] ;

					// decode ACs
					unsigned char k = 1 ;
					unsigned char ssss = 0 ;
					unsigned char rrrr = 0 ;
					bool eob = false ;

					while ((k <= 63) && !eob) {
						splitByte (decode (io, ACTable), &rrrr, &ssss) ;
						if (ssss == 0) {
							if (rrrr == 15) {
								k += 16 ; // goto start of loop
							}
							else {
								eob = true ;
							}
						}
						else {
							k += rrrr ;
							dctcoeffs[unitstart + k] = extend (receive (io, ssss), ssss) ;
							k++ ;	// next coeff
						}
					}

					unitstart += 64 ;
				}
			}
		}
		catch (JpegMarkerFound e) {
			if (termclean || (e.getMarkerCode() == JpegElement::MarkerEOI)) {
				termmarker = e.getMarkerCode() ;
			}
			else {
				throw CorruptJpegError (io, _("entropy coded data interrupted by marker 0x%x."), e.getMarkerCode()) ;
			}
		}
	}
}

void JpegEntropyCoded::write (BinaryIO *io)
{
	assert (writebyte == 0) ;
	assert (writebytecontains == 0) ;

	// init
	JpegScan *p_scan = (JpegScan *) getParent() ;
	JpegScanHeader *p_scanhdr = (JpegScanHeader *) p_scan->getScanHeader() ;
	JpegFrame *p_frame = (JpegFrame *) p_scan->getParent() ;
	JpegFrameHeader *p_framehdr = (JpegFrameHeader *) p_frame->getFrameHeader() ;

	vector<int> prediction ;
	vector<JpegHuffmanTable*> DCTables ;
	vector<JpegHuffmanTable*> ACTables ;
	vector<unsigned int> dataunits ;
	for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++) {
		prediction.push_back (0) ;
		DCTables.push_back (p_frame->getDCTable (p_scanhdr->getDCDestSpec (comp))) ;
		ACTables.push_back (p_frame->getACTable (p_scanhdr->getACDestSpec (comp))) ;
		dataunits.push_back (p_framehdr->getHorizSampling (comp) * p_framehdr->getVertSampling (comp)) ;
	}

	// write
	unsigned long unitstart = 0 ;
	assert (dctcoeffs.size() % 64 == 0) ;
	while (unitstart < dctcoeffs.size()) {
		for (unsigned int comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++) {
			for (unsigned int du = 0 ; du < dataunits[comp] ; du++) {
				// encode DC
				int diff = dctcoeffs[unitstart] - prediction[comp] ;
				writebits (io, DCTables[comp]->getXHuffCode (diff), DCTables[comp]->getXHuffSize (diff)) ;
				prediction[comp] = dctcoeffs[unitstart] ;

				// encode ACs
				unsigned int r = 0 ;
				for (unsigned int k = 1 ; k < 64 ; k++) {
					if (dctcoeffs[unitstart + k] == 0) {
						if (k == 63) {
							writebits (io, ACTables[comp]->getEHuffCode (0x00), ACTables[comp]->getEHuffSize (0x00)) ;
							break ;
						}
						else {
							r++ ;
						}
					}
					else {
						while (r > 15) {
							r -= 16 ;
							writebits (io, ACTables[comp]->getEHuffCode (0xF0), ACTables[comp]->getEHuffSize (0xF0)) ;
						}
						encode (io, ACTables[comp], r, dctcoeffs[unitstart + k]) ;
						r = 0 ;
					}
				}

				unitstart += 64 ;
			}
		}
	}

	if (writebytecontains != 0) {
		// pad with ones
		unsigned int n = 8 - writebytecontains ;
		for (unsigned int i = 0 ; i < n ; i++) {
			writebit (io, 1) ;
		}
	}
}

unsigned long JpegEntropyCoded::getNumSamples()
{
	return ((unsigned long) dctcoeffs.size()) ;
}

void JpegEntropyCoded::replaceSample (SamplePos pos, CvrStgSample *s)
{
	assert (pos < dctcoeffs.size()) ;
	JpegSample *sample = dynamic_cast<JpegSample*> (s) ;
	assert (sample != NULL) ;
	dctcoeffs[pos] = sample->getDctCoeff() ;
}

CvrStgSample *JpegEntropyCoded::getSample (SamplePos pos)
{
	assert (pos < dctcoeffs.size()) ;
	JpegScan *p_scan = (JpegScan *) getParent() ;
	JpegFrame *p_frame = (JpegFrame *) p_scan->getParent() ;
	JpegFile *p_file = p_frame->getFile() ;
	return ((CvrStgSample *) new JpegSample ((CvrStgFile *) p_file, dctcoeffs[pos])) ;
}

unsigned char JpegEntropyCoded::decode (BinaryIO *io, JpegHuffmanTable *ht)
{
	unsigned int len = 1 ;
	int code = readbit (io) ;

	while (code > ht->getMaxCode(len)) {
		len++ ;
		code = (code << 1) | readbit (io) ;
		assert (len <= 16) ;
	}

	unsigned int j = ht->getValPtr (len) ;
	int mc = ht->getMinCode (len) ;
	assert (code >= mc) ;
	j += code - mc ;
	return ht->getHuffVal(j) ;
}

void JpegEntropyCoded::encode (BinaryIO *io, JpegHuffmanTable *ht, unsigned int r, int coeff)
{
	unsigned char ssss = csize (coeff) ;
	unsigned char rs = (r << 4) | ssss ;
	writebits (io, ht->getEHuffCode (rs), ht->getEHuffSize (rs)) ;
	if (coeff < 0) {
		coeff-- ;
	}
	writebits (io, coeff, ssss) ;
}

int JpegEntropyCoded::receive (BinaryIO *io, unsigned char nbits)
{
	int retval = 0 ;

	while (nbits > 0) {
		retval = (retval << 1) | readbit (io) ;
		nbits-- ;
	}

	return retval ;
}

int JpegEntropyCoded::extend (int val, unsigned char t)
{
	if (val < (1 << (t - 1))) {
		val += (-1 << t) + 1 ;
	}

	return val ;
}

unsigned int JpegEntropyCoded::csize (int ac)
{
	assert (ac != 0) ;
	if (ac < 0) {
		ac = -ac ;
	}
	unsigned int rv = 0 ;
	while (ac != 1) {
		ac = ac / 2 ;
		rv++ ;
	}
	rv++ ;
	return rv ;
}

int JpegEntropyCoded::readbit (BinaryIO *io)
{
	int retval = 0 ;

	if (readbytecontains == 0) {
		readbyte = io->read8() ;
		readbytecontains = 8 ;
		if (readbyte == 0xFF) {
			unsigned char markerbyte = io->read8() ;
			if (markerbyte != 0x00) {
				throw JpegMarkerFound (markerbyte) ;
			}
		}
	}

	retval = readbyte >> 7 ;
	readbytecontains-- ;
	readbyte = readbyte << 1 ;

	return retval ;
}

void JpegEntropyCoded::writebits (BinaryIO *io, unsigned long v, unsigned int n)
{
	for (unsigned int i = n ; i > 0 ; i--) {
		writebit (io, (v & (1 << (i - 1))) >> (i - 1)) ;
	}
}

void JpegEntropyCoded::writebit (BinaryIO *io, int bit)
{
	assert (bit == 0 || bit == 1) ;
	assert (writebytecontains < 8) ;

	writebyte = writebyte << 1 ;
	writebyte = writebyte | bit ;
	writebytecontains++ ;

	if (writebytecontains == 8) {
		io->write8 (writebyte) ;
		if (writebyte == 0xFF) {
			io->write8 (0x00) ;
		}
		writebytecontains = 0 ;
		writebyte = 0 ;
	}
}
