/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>
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

#ifndef SH_JPEGFILE_H
#define SH_JPEGFILE_H

#include "common.h"

#ifdef USE_LIBJPEG

#include <vector>

extern "C" {
#include <stdio.h>
#include <jpeglib.h>
}

class BinaryIO ;
#include "CvrStgFile.h"
class SampleValue ;

/**
 * \class JpegFile
 * \brief a cover-/stego-file in the jpeg file format
 *
 * This class uses the JPEG library by the IJG for access to jpeg files.
 **/
class JpegFile : public CvrStgFile {
	public:
	JpegFile (BinaryIO* io) ;

	void read (BinaryIO* io) ;
	void write (void) ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (const SamplePos pos) const ;
	unsigned short getSamplesPerEBit (void) const
		{ return SamplesPerEBit ; } ;

#ifdef DEBUG
	std::map<SampleKey,unsigned long>* getFrequencies (void) ;
	/**
	 * SampleKey is unsigned, jpeg dct coefficients are signed data. This function is a specialization
	 * of printFrequencies that converts the SampleKeys back to dct coefficient values before printing
	 * them and also prints the values starting from the lowest (negative) and ending with the highest
	 * (positive).
	 **/
	void printFrequencies (const std::map<SampleKey,unsigned long>& freqs) ;
#endif

	private:
	/// the number of dct coefficients per block
	static const unsigned int CoeffPerBlock = 64 ;

	static const unsigned short SamplesPerEBit = 2 ;
	static const UWORD32 Radius = 1 ;

	struct jpeg_compress_struct CInfo ;
	struct jpeg_decompress_struct DeCInfo ;
	jvirt_barray_ptr* DctCoeffs ;

	std::vector<SWORD16> LinDctCoeffs ;
	std::vector<UWORD32> StegoIndices ;
} ;

#endif // def USE_LIBJPEG

#endif // ndef SH_JPEGFILE_H
