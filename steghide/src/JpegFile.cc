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

#include "common.h"

#ifdef USE_LIBJPEG

#include <cstdio>
#include <iostream>

#include "BinaryIO.h"
#include "JpegFile.h"
#include "JpegSampleValue.h"
#include "error.h"

JpegFile::JpegFile (BinaryIO* io)
	: CvrStgFile()
{
	setSamplesPerEBit (SamplesPerEBit) ;
	setRadius (Radius) ;
	read (io) ;
}

void JpegFile::read (BinaryIO* io)
{
	CvrStgFile::read (io) ;

	// FIXME - use BinaryIO (or similar class) as input/output module for libjpeg (to avoid the NotImplementedError and using FILE* here)
	FILE *infile = NULL ;
	if (io->is_std()) {
		throw NotImplementedError (_("can not use standard input as source for jpeg files with libjpeg.")) ;
	}
	else {
		infile = io->getStream() ;
		rewind (infile) ;
	}

	struct jpeg_error_mgr errmgr ;
	DeCInfo.err = jpeg_std_error (&errmgr) ;
	jpeg_create_decompress (&DeCInfo) ;
	jpeg_stdio_src (&DeCInfo, infile) ;
	jpeg_read_header (&DeCInfo, TRUE) ;

	DctCoeffs = jpeg_read_coefficients (&DeCInfo) ;

	/*
	   coeffs sind in natural order, nicht in zig-zag order
   */
	
	// resize LinDctCoeffs to size that is enough to contain all dct coeffs
	unsigned int* height_in_blocks = new unsigned int[DeCInfo.num_components] ;
	unsigned int* width_in_blocks = new unsigned int[DeCInfo.num_components] ;

	unsigned long totalnumcoeffs = 0 ;
	for (unsigned short icomp = 0 ; icomp < DeCInfo.num_components ; icomp++) {
		if (DeCInfo.image_height % 8 == 0) {
			height_in_blocks[icomp] = (DeCInfo.image_height / 8) * DeCInfo.comp_info[icomp].v_samp_factor ;
		}
		else {
			// FIXME - include +1 padding into multiplication ??
			height_in_blocks[icomp] = 1 + (DeCInfo.image_height / 8) * DeCInfo.comp_info[icomp].v_samp_factor ;
		}
		if (DeCInfo.image_width % 8 == 0) {
			width_in_blocks[icomp] = (DeCInfo.image_width / 8) * DeCInfo.comp_info[icomp].h_samp_factor ;
		}
		else {
			// FIXME - include +1 padding into multiplication ??
			width_in_blocks[icomp] = 1 + (DeCInfo.image_width / 8) * DeCInfo.comp_info[icomp].h_samp_factor ;
		}
		// FIXME - height,width should be equal to (private) DeCInfo.comp_info[icomp].height_in_blocks resp. width_in_blocks
		totalnumcoeffs += CoeffPerBlock * (height_in_blocks[icomp] * width_in_blocks[icomp]) ;
	}
	LinDctCoeffs.resize (totalnumcoeffs) ;

	UWORD32 linindex = 0 ;
	JBLOCKARRAY array ;
	JBLOCKROW row ;
	JBLOCK block ;
	for (unsigned short icomp = 0 ; icomp < DeCInfo.num_components ; icomp++) {
		unsigned int currow = 0 ;
		while (currow < height_in_blocks[icomp]) {
#if 0
			unsigned int naccess = DeCInfo.comp_info[icomp].v_samp_factor ;
			if (DeCInfo.comp_info[icomp].height_in_blocks - currow < naccess) {
				naccess = DeCInfo.comp_info[icomp].height_in_blocks - currow ;
			}
#endif
			unsigned int naccess = 1 ;
			JBLOCKARRAY array = (*(DeCInfo.mem->access_virt_barray))
				((j_common_ptr) &DeCInfo, DctCoeffs[icomp], currow, naccess, FALSE) ;
			for (unsigned int irow = 0 ; irow < naccess ; irow++) {
				for (unsigned int iblock = 0 ; iblock < width_in_blocks[icomp] ; iblock++) {
					for (unsigned int icoeff = 0 ; icoeff < CoeffPerBlock ; icoeff++) {
						LinDctCoeffs[linindex] = array[irow][iblock][icoeff] ;

						// don't use zero dct coefficients to embed data
						if (LinDctCoeffs[linindex] != 0) {
							StegoIndices.push_back (linindex) ;
						}
						linindex++ ;
					}
				}
			}
			currow += naccess ;
		}
	}

	delete[] height_in_blocks ;
	delete[] width_in_blocks ;
}

void JpegFile::write ()
{
	CvrStgFile::write() ;

	FILE* outfile = getBinIO()->getStream() ;

	// prepare for writing
	jpeg_create_compress (&CInfo) ;
	jpeg_copy_critical_parameters (&DeCInfo, &CInfo) ;
	struct jpeg_error_mgr jerr2 ;
	CInfo.err = jpeg_std_error(&jerr2) ;
	jpeg_stdio_dest (&CInfo, outfile) ;

	// write file header
	jpeg_write_coefficients (&CInfo, DctCoeffs) ;

	// copy current values to virtual array
	unsigned int* height_in_blocks = new unsigned int[CInfo.num_components] ;
	unsigned int* width_in_blocks = new unsigned int[CInfo.num_components] ;

	UWORD32 linindex = 0 ;
	for (unsigned short icomp = 0 ; icomp < CInfo.num_components ; icomp++) {
		if (CInfo.image_height % 8 == 0) {
			height_in_blocks[icomp] = (CInfo.image_height / 8) * CInfo.comp_info[icomp].v_samp_factor ;
		}
		else {
			// FIXME - include +1 padding into multiplication ??
			height_in_blocks[icomp] = 1 + (CInfo.image_height / 8) * CInfo.comp_info[icomp].v_samp_factor ;
		}
		if (CInfo.image_width % 8 == 0) {
			width_in_blocks[icomp] = (CInfo.image_width / 8) * CInfo.comp_info[icomp].h_samp_factor ;
		}
		else {
			// FIXME - include +1 padding into multiplication ??
			width_in_blocks[icomp] = 1 + (CInfo.image_width / 8) * CInfo.comp_info[icomp].h_samp_factor ;
		}
		// FIXME - height,width should be equal to (private) DeCInfo.comp_info[icomp].height_in_blocks resp. width_in_blocks

		unsigned int currow = 0 ;
		while (currow < height_in_blocks[icomp]) {
#if 0
			unsigned int naccess = CInfo.comp_info[icomp].v_samp_factor ;
			if (CInfo.comp_info[icomp].height_in_blocks - currow < naccess) {
				naccess = CInfo.comp_info[icomp].height_in_blocks - currow ;
			}
#endif
			unsigned int naccess = 1 ;
			JBLOCKARRAY array = (*(CInfo.mem->access_virt_barray))
				((j_common_ptr) &CInfo, DctCoeffs[icomp], currow, naccess, TRUE) ;
			for (unsigned int irow = 0 ; irow < naccess ; irow++) {
				for (unsigned int iblock = 0 ; iblock < width_in_blocks[icomp] ; iblock++) {
					for (unsigned int icoeff = 0 ; icoeff < CoeffPerBlock ; icoeff++) {
						array[irow][iblock][icoeff] = LinDctCoeffs[linindex] ;
						linindex++ ;
					}
				}
			}
			currow += naccess ;
		}
	}

	delete[] height_in_blocks ;
	delete[] width_in_blocks ;

	// write and deallocate everything (writing is possible only once)
	jpeg_finish_compress (&CInfo) ;
	jpeg_destroy_compress(&CInfo);
	jpeg_finish_decompress (&DeCInfo) ;
	jpeg_destroy_decompress(&DeCInfo);
}

unsigned long JpegFile::getNumSamples (void) const
{
	return StegoIndices.size() ;
}

SampleValue* JpegFile::getSampleValue (const SamplePos pos) const
{
	myassert (pos < StegoIndices.size()) ;
	return new JpegSampleValue (LinDctCoeffs[StegoIndices[pos]]) ;
}

void JpegFile::replaceSample (const SamplePos pos, const SampleValue* s)
{
	const JpegSampleValue* sample = dynamic_cast<const JpegSampleValue*> (s) ;
	myassert (sample != NULL) ;
	myassert (pos <= StegoIndices.size()) ;
	LinDctCoeffs[StegoIndices[pos]] = sample->getDctCoeff() ;
}

#ifdef DEBUG
std::map<SampleKey,unsigned long>* JpegFile::getFrequencies ()
{
	unsigned long n = LinDctCoeffs.size() ;
	std::map<SampleKey,unsigned long>* table = new std::map<SampleKey,unsigned long> () ;

	for (unsigned long pos = 0 ; pos < n ; pos++) {
		SampleValue *sv = (SampleValue*) new JpegSampleValue (LinDctCoeffs[pos]) ;
		(*table)[sv->getKey()]++ ;
		delete sv ;
	}

	return table ;
}

void JpegFile::printFrequencies (const std::map<SampleKey,unsigned long>& freqs)
{
	std::list<std::string> output ;

	// insert the positive dct coeffs into output list
	for (std::map<SampleKey,unsigned long>::const_iterator pit = freqs.begin() ; pit->first < 2147483648UL /* 2^31 */ ; pit++) {
		char buf[30] ;
		sprintf (buf, "%d: %d", (long) pit->first, pit->second) ;
		output.push_back (std::string(buf)) ;
	}

	// insert the negative dct coeffs into output list
	for (std::map<SampleKey,unsigned long>::const_reverse_iterator nit = freqs.rbegin() ; nit->first > 2147483648UL /* 2^31 */ ; nit++) {
		char buf[30] ;
		sprintf (buf, "%d: %d", (long) nit->first, nit->second) ;
		output.push_front (std::string(buf)) ;
	}

	for (std::list<std::string>::const_iterator it = output.begin() ; it != output.end() ; it++) {
		std::cout << *it << std::endl ;
	}
}
#endif // def DEBUG

#endif // def USE_LIBJPEG
