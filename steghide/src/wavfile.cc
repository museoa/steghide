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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "main.h"
#include "cvrstgfile.h"
#include "wavfile.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

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
	buffree (data) ;
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

unsigned long WavFile::getCapacity ()
{
	unsigned int bytespersample = 0 ;

	if (fmtch.BitsPerSample % 8 == 0) {
		bytespersample = fmtch.BitsPerSample / 8 ;
	}
	else {
		bytespersample = (fmtch.BitsPerSample / 8) + 1 ;
	}

	assert (data->length % bytespersample == 0) ;

	return data->length / bytespersample ;
}

void WavFile::embedBit (unsigned long pos, int value)
{
	assert (value == 0 || value == 1) ;
	assert (pos <= getCapacity()) ;

	unsigned long bytepos = 0 ;
	unsigned int bitpos = 0 ;
	calcpos (pos, &bytepos, &bitpos) ;
	bufsetbit (data, bytepos, bitpos, value) ;
}

int WavFile::extractBit (unsigned long pos)
{
	assert (pos <= getCapacity()) ;

	unsigned long bytepos = 0 ;
	unsigned int bitpos = 0 ;
	calcpos (pos, &bytepos, &bitpos) ;
	return bufgetbit (data, bytepos, bitpos) ;
}

/* calculate position in buffer for n-th embedded bit */
void WavFile::calcpos (unsigned long n, unsigned long *bytepos, unsigned int *bitpos)
{
	unsigned int bytespersample = 0 ;

	if (fmtch.BitsPerSample % 8 == 0) {
		bytespersample = fmtch.BitsPerSample / 8 ;
	}
	else {
		bytespersample = (fmtch.BitsPerSample / 8) + 1 ;
	}

	*bytepos = n * bytespersample ;
	*bitpos = fmtch.BitsPerSample % 8 ;
}

/* reads the wav file data from disk */
void WavFile::readdata (void)
{
	try {
		data = bufcreate (0) ; /* FIXME - set length smarter */

		unsigned long pos = 0 ;
		while (!BinIO->eof()) {
			bufsetbyte (data, pos++, BinIO->read8()) ;
		}

		unsupchunks2.data = NULL ;
		unsupchunks2.len = 0 ;
		if (!BinIO->eof()) {
			unsigned char *ptrunsupdata2 = NULL ;

			while (!BinIO->eof()) {
				unsupchunks2.data = s_realloc (unsupchunks2.data, unsupchunks2.len + 1) ;
				ptrunsupdata2 = (unsigned char *) unsupchunks2.data ;

				ptrunsupdata2[unsupchunks2.len] = BinIO->read8() ;
				unsupchunks2.len++ ;
			}			
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the audio data from the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading audio data."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::STDIN_ERR:
			throw SteghideError (_("an error occured while reading the audio data from standard input.")) ;
			break ;

			case BinaryInputError::STDIN_EOF:
			throw SteghideError (_("premature end of data from standard input while reading audio data.")) ;
			break ;
		}
	}
}

/* writes a wav file to disk */
void WavFile::writedata (void)
{
	try {
		int c = EOF ;
		unsigned long pos = 0 ;

		while ((c = bufgetbyte (data, pos++)) != ENDOFBUF) {
			BinIO->write8 ((unsigned char) c) ;
		}

		if (unsupchunks2.len > 0) {
			unsigned char *ptrunsupdata2 = (unsigned char *) unsupchunks2.data ;
			for (int i = 0; i < unsupchunks2.len; i++) {
				BinIO->write8 (ptrunsupdata2[i]) ;
			}
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the audio data to the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the audio data to standard output.")) ;
			break ;
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

		riffchhdr.len = BinIO->read32_le() ;

		for (unsigned int i = 0 ; i < 4 ; i++) {
			id_wave[i] = BinIO->read8() ;
		}
		if (!(id_wave[0] == 'W' &&
			id_wave[1] == 'A' &&
			id_wave[2] == 'V' &&
			id_wave[3] == 'E')) {
			throw UnSupFileFormat (BinIO) ;
		}

		fmtchhdr = *getChhdr() ;
		if (!(fmtchhdr.id[0] == 'f' &&
			fmtchhdr.id[1] == 'm' &&
			fmtchhdr.id[2] == 't' &&
			fmtchhdr.id[3] == ' ')) {
			if (BinIO->is_std()) {
				throw SteghideError (_("could not find the format chunk in the wav file from standard input.")) ;
			}
			else {
				throw SteghideError (_("could not find the format chunk in the wav file \"%s\"."), BinIO->getName().c_str()) ;
			}
		}
		if ((fmtch.FormatTag = BinIO->read16_le()) != FormatPCM) {
			if (BinIO->is_std()) {
				throw SteghideError (_("the wav file from standard input has a format that is not supported.")) ;
			}
			else {
				throw SteghideError (_("the wav file \"%s\" has a format that is not supported."), BinIO->getName().c_str()) ;
			}
		}
		fmtch.Channels = BinIO->read16_le () ;
		fmtch.SamplesPerSec = BinIO->read32_le () ;
		fmtch.AvgBytesPerSec = BinIO->read32_le () ;
		fmtch.BlockAlign = BinIO->read16_le () ;
		/* if a number other than a multiple of 8 is used, we cannot hide data,
		   because the least significant bits are always set to zero - FIXME */
		if ((fmtch.BitsPerSample = BinIO->read16_le ()) % 8 != 0) {
			if (BinIO->is_std()) {
				throw SteghideError (_("the bits/sample rate of the wav file from standard input is not a multiple of eight.")) ;
			}
			else {
				throw SteghideError (_("the bits/sample rate of the wav file \"%s\" is not a multiple of eight."), BinIO->getName().c_str()) ;
			}
		}

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
				ptrunsupdata1[unsupchunks1.len++] = (unsigned char) BinIO->read8() ;
			}

			tmpchhdr = getChhdr() ;
		}

		strncpy (datachhdr.id, tmpchhdr->id, 4) ;
		datachhdr.len = tmpchhdr->len ;
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the wav headers from the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading wav headers."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::STDIN_ERR:
			throw SteghideError (_("an error occured while reading the wav headers from standard input.")) ;
			break ;

			case BinaryInputError::STDIN_EOF:
			throw SteghideError (_("premature end of data from standard input while reading wav headers.")) ;
			break ;
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
			BinIO->write8 (id_wave[i]) ;
		}

		putChhdr (&fmtchhdr) ;
		BinIO->write16_le (fmtch.FormatTag) ;
		BinIO->write16_le (fmtch.Channels) ;
		BinIO->write32_le (fmtch.SamplesPerSec) ;
		BinIO->write32_le (fmtch.AvgBytesPerSec) ;
		BinIO->write16_le (fmtch.BlockAlign) ;
		BinIO->write16_le (fmtch.BitsPerSample) ;

		if (unsupchunks1.len > 0) {
			unsigned char *ptrunsupdata1 = (unsigned char *) unsupchunks1.data ;
			for (int i = 0; i < unsupchunks1.len; i++)
				BinIO->write8 (ptrunsupdata1[i]) ;
		}

		putChhdr (&datachhdr) ;
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the wav headers to the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the wav headers to standard output.")) ;
			break ;
		}
	}
}

/* reads a wav chunk header */
WavFile::ChunkHeader *WavFile::getChhdr ()
{
	ChunkHeader *retval = new ChunkHeader ;

	retval->id[0] = BinIO->read8() ;
	retval->id[1] = BinIO->read8() ;
	retval->id[2] = BinIO->read8() ;
	retval->id[3] = BinIO->read8() ;
	retval->len = BinIO->read32_le() ;

	return retval ;
}

/* writes a wav chunk header from chhdr to the file */
void WavFile::putChhdr (ChunkHeader *chhdr)
{
	BinIO->write8 (chhdr->id[0]) ;
	BinIO->write8 (chhdr->id[1]) ;
	BinIO->write8 (chhdr->id[2]) ;
	BinIO->write8 (chhdr->id[3]) ;
	BinIO->write32_le (chhdr->len) ;
}
