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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "common.h"
#include "cvrstgfile.h"
#include "error.h"
#include "msg.h"
#include "wavfile.h"
#include "wavsample.h"

WavFile::WavFile ()
	: CvrStgFile()
{
	// empty
}

WavFile::WavFile (BinaryIO *io)
	: CvrStgFile()
{
	read (io) ;
}

WavFile::~WavFile ()
{
	free (unsupchunks1.data) ;
	free (unsupchunks2.data) ;
}

void WavFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	readheaders () ;
	readdata () ;
}

void WavFile::write ()
{
	CvrStgFile::write () ;

	writeheaders () ;
	writedata () ;
}

unsigned long WavFile::getNumSamples()
{
	unsigned long retval = 0 ;
	if (fmtch.BitsPerSample <= 8) {
		retval = data_small.size() ;
	}
	else {
		retval = data_large.size() ;
	}
	return retval ;
}

void WavFile::replaceSample (SamplePos pos, CvrStgSample *s)
{
	WavPCMSample *sample = dynamic_cast<WavPCMSample*> (s) ;
	assert (sample != NULL) ;
	if (fmtch.BitsPerSample <= 8) {
		data_small[pos] = (unsigned char) sample->getValue() ;
	}
	else {
		data_large[pos] = sample->getValue() ;
	}
}

unsigned int WavFile::getSamplesPerEBit()
{
	return 2 ;
}

CvrStgSample *WavFile::getSample (SamplePos pos)
{
	int value = 0 ;
	if (fmtch.BitsPerSample <= 8) {
		value = (int) data_small[pos] ;
	}
	else {
		value = data_large[pos] ;
	}
	return ((CvrStgSample *) new WavPCMSample (this, value)) ;
}

unsigned short WavFile::getBitsPerSample()
{
	return fmtch.BitsPerSample ;
}

unsigned short WavFile::getBytesPerSample()
{
	unsigned short retval = 0 ;
	if (fmtch.BitsPerSample % 8 == 0) {
		retval = fmtch.BitsPerSample / 8 ;
	}
	else {
		retval = (fmtch.BitsPerSample / 8) + 1 ;
	}
	return retval ;
}

unsigned short WavFile::getFirstBitPosinSample()
{
	unsigned short retval = 0 ;
	if (fmtch.BitsPerSample % 8 == 0) {
		retval = 0 ;
	}
	else {
		retval = 8 - (fmtch.BitsPerSample % 8) ;
	}
	return retval ;
}

/* reads the wav file data from disk */
void WavFile::readdata (void)
{
	try {
		data_small.clear() ;
		data_large.clear() ;
		unsigned short bytespersample = getBytesPerSample() ;
		unsigned short firstbitpos = getFirstBitPosinSample() ;
			
		unsigned long readpos = 0 ;
		while (readpos < datachhdr.len) {
			if (fmtch.BitsPerSample <= 8) {
				data_small.push_back ((getBinIO()->read8()) >> firstbitpos) ;
			}
			else {
				// decode two's complement
				unsigned int value = (unsigned int) getBinIO()->read_le (bytespersample) ;
				value = value >> firstbitpos ;
					
				int sign = 0 ;
				if (((value & (1 << (fmtch.BitsPerSample - 1))) >> (fmtch.BitsPerSample - 1)) == 0) {
					sign = 1 ;
				}
				else {
					sign = -1 ;
					value = ~value ;
					value++ ;
				}

				unsigned int mask = 0 ;
				for (unsigned short i = 0 ; i < fmtch.BitsPerSample ; i++) {
					mask <<= 1 ;
					mask |= 1 ;
				}
				value &= mask ;

				data_large.push_back (sign * ((int) value)) ;
			}
			readpos += bytespersample ;
		}

		unsupchunks2.data = NULL ;
		unsupchunks2.len = 0 ;
		if (!getBinIO()->eof()) {
			unsigned char *ptrunsupdata2 = NULL ;

			while (!getBinIO()->eof()) {
				unsupchunks2.data = s_realloc (unsupchunks2.data, unsupchunks2.len + 1) ;
				ptrunsupdata2 = (unsigned char *) unsupchunks2.data ;

				ptrunsupdata2[unsupchunks2.len] = getBinIO()->read8() ;
				unsupchunks2.len++ ;
			}			
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR: {
				throw SteghideError (_("an error occured while reading the audio data from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::FILE_EOF: {
				throw SteghideError (_("premature end of file \"%s\" while reading audio data."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::STDIN_ERR: {
				throw SteghideError (_("an error occured while reading the audio data from standard input.")) ;
			break ; }

			case BinaryInputError::STDIN_EOF: {
				throw SteghideError (_("premature end of data from standard input while reading audio data.")) ;
			break ; }
		}
	}
}

void WavFile::writedata (void)
{
	try {
		unsigned short bytespersample = getBytesPerSample() ;
		unsigned short firstbitpos = getFirstBitPosinSample() ;

		unsigned long writepos = 0 ;
		while (writepos < datachhdr.len) {
			if (fmtch.BitsPerSample <= 8) {
				getBinIO()->write8 (data_small[writepos] << firstbitpos) ;
			}
			else {
				unsigned long value = 0 ;
				if (data_large[writepos / bytespersample] >= 0) {
					value = (unsigned long) data_large[writepos / bytespersample] ;
				}
				else {
					value = (unsigned long) -data_large[writepos / bytespersample] ; // value is now |sample|
					value = ~value ;
					value++ ;
				}
				value <<= firstbitpos ;
				getBinIO()->write_le (value, bytespersample) ;
			}
			writepos += bytespersample ;
		}

		if (unsupchunks2.len > 0) {
			unsigned char *ptrunsupdata2 = (unsigned char *) unsupchunks2.data ;
			for (int i = 0; i < unsupchunks2.len; i++) {
				getBinIO()->write8 (ptrunsupdata2[i]) ;
			}
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR: {
				throw SteghideError (_("an error occured while writing the audio data to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryOutputError::STDOUT_ERR: {
				throw SteghideError (_("an error occured while writing the audio data to standard output.")) ;
			break ; }
		}
	}
}

/* reads the headers of a wav file from disk */
void WavFile::readheaders ()
{
	try {
		riffchhdr.id[0] = 'R' ;
		riffchhdr.id[1] = 'I' ;
		riffchhdr.id[2] = 'F' ;
		riffchhdr.id[3] = 'F' ;

		riffchhdr.len = getBinIO()->read32_le() ;

		for (unsigned int i = 0 ; i < 4 ; i++) {
			id_wave[i] = getBinIO()->read8() ;
		}
		if (!(id_wave[0] == 'W' &&
			id_wave[1] == 'A' &&
			id_wave[2] == 'V' &&
			id_wave[3] == 'E')) {
			throw UnSupFileFormat (getBinIO()) ;
		}

		fmtchhdr = *getChhdr() ;
		if (!(fmtchhdr.id[0] == 'f' &&
			fmtchhdr.id[1] == 'm' &&
			fmtchhdr.id[2] == 't' &&
			fmtchhdr.id[3] == ' ')) {
			if (getBinIO()->is_std()) {
				throw SteghideError (_("could not find the format chunk in the wav file from standard input.")) ;
			}
			else {
				throw SteghideError (_("could not find the format chunk in the wav file \"%s\"."), getBinIO()->getName().c_str()) ;
			}
		}
		if ((fmtch.FormatTag = getBinIO()->read16_le()) != FormatPCM) {
			if (getBinIO()->is_std()) {
				throw SteghideError (_("the wav file from standard input has a format that is not supported.")) ;
			}
			else {
				throw SteghideError (_("the wav file \"%s\" has a format that is not supported."), getBinIO()->getName().c_str()) ;
			}
		}
		fmtch.Channels = getBinIO()->read16_le () ;
		fmtch.SamplesPerSec = getBinIO()->read32_le () ;
		fmtch.AvgBytesPerSec = getBinIO()->read32_le () ;
		fmtch.BlockAlign = getBinIO()->read16_le () ;
		fmtch.BitsPerSample = getBinIO()->read16_le() ;

		unsupchunks1.data = NULL ;
		unsupchunks1.len = 0 ;
		ChunkHeader *tmpchhdr = getChhdr () ;
		while (strncmp (tmpchhdr->id, "data", 4) != 0) {
			unsupchunks1.data = s_realloc (unsupchunks1.data, (unsupchunks1.len + SizeChunkHeader + tmpchhdr->len)) ;

			unsigned char *ptrunsupdata1 = (unsigned char *) unsupchunks1.data ;
			unsigned int i = 0 ;
			for (i = 0; i < 4; i++) {
				ptrunsupdata1[unsupchunks1.len++] = (unsigned char) tmpchhdr->id[i] ;
			}
			cp32ul2uc_le (&ptrunsupdata1[unsupchunks1.len], tmpchhdr->len) ;
			unsupchunks1.len += 4 ;

			for (i = 0; i < tmpchhdr->len; i++) {
				ptrunsupdata1[unsupchunks1.len++] = (unsigned char) getBinIO()->read8() ;
			}

			tmpchhdr = getChhdr() ;
		}

		strncpy (datachhdr.id, tmpchhdr->id, 4) ;
		datachhdr.len = tmpchhdr->len ;
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while reading the wav headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::FILE_EOF:
			{
				throw SteghideError (_("premature end of file \"%s\" while reading wav headers."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::STDIN_ERR:
			{
				throw SteghideError (_("an error occured while reading the wav headers from standard input.")) ;
				break ;
			}

			case BinaryInputError::STDIN_EOF:
			{
				throw SteghideError (_("premature end of data from standard input while reading wav headers.")) ;
				break ;
			}
		}
	}

	return ;
}

/* writes the headers of a wav file to disk */
void WavFile::writeheaders ()
{
	try {
		putChhdr (&riffchhdr) ;
		for (int i = 0 ; i < 4 ; i++) {
			getBinIO()->write8 (id_wave[i]) ;
		}

		putChhdr (&fmtchhdr) ;
		getBinIO()->write16_le (fmtch.FormatTag) ;
		getBinIO()->write16_le (fmtch.Channels) ;
		getBinIO()->write32_le (fmtch.SamplesPerSec) ;
		getBinIO()->write32_le (fmtch.AvgBytesPerSec) ;
		getBinIO()->write16_le (fmtch.BlockAlign) ;
		getBinIO()->write16_le (fmtch.BitsPerSample) ;

		if (unsupchunks1.len > 0) {
			unsigned char *ptrunsupdata1 = (unsigned char *) unsupchunks1.data ;
			for (int i = 0; i < unsupchunks1.len; i++)
				getBinIO()->write8 (ptrunsupdata1[i]) ;
		}

		putChhdr (&datachhdr) ;
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while writing the wav headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryOutputError::STDOUT_ERR:
			{
				throw SteghideError (_("an error occured while writing the wav headers to standard output.")) ;
				break ;
			}
		}
	}
}

/* reads a wav chunk header */
WavFile::ChunkHeader *WavFile::getChhdr ()
{
	ChunkHeader *retval = new ChunkHeader ;

	retval->id[0] = getBinIO()->read8() ;
	retval->id[1] = getBinIO()->read8() ;
	retval->id[2] = getBinIO()->read8() ;
	retval->id[3] = getBinIO()->read8() ;
	retval->len = getBinIO()->read32_le() ;

	return retval ;
}

/* writes a wav chunk header from chhdr to the file */
void WavFile::putChhdr (ChunkHeader *chhdr)
{
	getBinIO()->write8 (chhdr->id[0]) ;
	getBinIO()->write8 (chhdr->id[1]) ;
	getBinIO()->write8 (chhdr->id[2]) ;
	getBinIO()->write8 (chhdr->id[3]) ;
	getBinIO()->write32_le (chhdr->len) ;
}

void *WavFile::s_realloc (void *ptr, size_t size)
{
	void *retval = NULL ;
	if ((retval = realloc (ptr, size)) == NULL) {
		throw SteghideError (_("could not reallocate memory.")) ;
	}
	return retval ;
}

void WavFile::cp32ul2uc_le (unsigned char *dest, unsigned long src)
{
	dest[0] = src & 0xFF ;
	dest[1] = (src >> 8) & 0xFF ;
	dest[2] = (src >> 16) & 0xFF ;
	dest[3] = (src >> 24) & 0xFF ;
}
