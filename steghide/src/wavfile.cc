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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "common.h"
#include "cvrstgfile.h"
#include "error.h"
#include "msg.h"
#include "wavfile.h"
#include "wavsamplevalue.h"

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
	delete riffchhdr ;
	delete FormatChunk ;
	delete datachhdr ;
	for (std::vector<WavChunkUnused*>::iterator i = UnusedBeforeData.begin() ; i != UnusedBeforeData.end() ; i++) {
		delete (*i) ;
	}
	for (std::vector<WavChunkUnused*>::iterator i = UnusedAfterData.begin() ; i != UnusedAfterData.end() ; i++) {
		delete (*i) ;
	}
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

unsigned long WavFile::getNumSamples() const
{
	unsigned long retval = 0 ;
	if (FormatChunk->getBitsPerSample() <= 8) {
		retval = data_small.size() ;
	}
	else {
		retval = data_large.size() ;
	}
	return retval ;
}

void WavFile::replaceSample (const SamplePos pos, const SampleValue* s)
{
	const WavPCMSampleValue* sample = dynamic_cast<const WavPCMSampleValue*> (s) ;
	myassert (sample != NULL) ;
	if (FormatChunk->getBitsPerSample() <= 8) {
		data_small[pos] = (unsigned char) sample->getValue() ;
	}
	else {
		data_large[pos] = sample->getValue() ;
	}
}

unsigned int WavFile::getSamplesPerEBit() const
{
	return 2 ;
}

SampleValue *WavFile::getSampleValue (SamplePos pos) const
{
	int value = 0 ;
	if (FormatChunk->getBitsPerSample() <= 8) {
		value = (int) data_small[pos] ;
	}
	else {
		value = data_large[pos] ;
	}
	return ((SampleValue *) new WavPCMSampleValue (this, value)) ;
}

unsigned short WavFile::getBitsPerSample() const
{
	return FormatChunk->getBitsPerSample() ;
}

unsigned short WavFile::getBytesPerSample()
{
	unsigned short retval = 0 ;
	if (FormatChunk->getBitsPerSample() % 8 == 0) {
		retval = FormatChunk->getBitsPerSample() / 8 ;
	}
	else {
		retval = (FormatChunk->getBitsPerSample() / 8) + 1 ;
	}
	return retval ;
}

unsigned short WavFile::getFirstBitPosinSample()
{
	unsigned short retval = 0 ;
	if (FormatChunk->getBitsPerSample() % 8 == 0) {
		retval = 0 ;
	}
	else {
		retval = 8 - (FormatChunk->getBitsPerSample() % 8) ;
	}
	return retval ;
}

/* reads the wav file data from disk */
void WavFile::readdata (void)
{
	try {
		data_small.clear() ;
		data_large.clear() ;
		unsigned short bitspersample = FormatChunk->getBitsPerSample() ;
		unsigned short bytespersample = getBytesPerSample() ;
		unsigned short firstbitpos = getFirstBitPosinSample() ;
			
		unsigned long readpos = 0 ;
		while (readpos < datachhdr->getChunkLength()) {
			if (bitspersample <= 8) {
				data_small.push_back ((getBinIO()->read8()) >> firstbitpos) ;
			}
			else {
				// decode two's complement
				unsigned int value = (unsigned int) getBinIO()->read_le (bytespersample) ;
				value = value >> firstbitpos ;
					
				int sign = 0 ;
				if (((value & (1 << (bitspersample - 1))) >> (bitspersample - 1)) == 0) {
					sign = 1 ;
				}
				else {
					sign = -1 ;
					value = ~value ;
					value++ ;
				}

				unsigned int mask = 0 ;
				for (unsigned short i = 0 ; i < bitspersample ; i++) {
					mask <<= 1 ;
					mask |= 1 ;
				}
				value &= mask ;

				data_large.push_back (sign * ((int) value)) ;
			}
			readpos += bytespersample ;
		}

		UnusedAfterData.clear() ;
		while (!getBinIO()->eof()) {
			WavChunkHeader *chhdr = new WavChunkHeader (getBinIO()) ;
			UnusedAfterData.push_back (new WavChunkUnused (chhdr, getBinIO())) ;
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
		unsigned short bitspersample = FormatChunk->getBitsPerSample() ;
		unsigned short bytespersample = getBytesPerSample() ;
		unsigned short firstbitpos = getFirstBitPosinSample() ;

		unsigned long writepos = 0 ;
		while (writepos < datachhdr->getChunkLength()) {
			if (bitspersample <= 8) {
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

		for (std::vector<WavChunkUnused*>::const_iterator i = UnusedAfterData.begin() ; i != UnusedAfterData.end() ; i++) {
			(*i)->write (getBinIO()) ;
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
		riffchhdr = new WavChunkHeader ("RIFF", getBinIO()->read32_le()) ;

		for (unsigned int i = 0 ; i < 4 ; i++) {
			id_wave[i] = getBinIO()->read8() ;
		}
		if (strncmp (id_wave, "WAVE", 4) != 0) {
			throw UnSupFileFormat (getBinIO()) ;
		}

		FormatChunk = new WavFormatChunk (new WavChunkHeader (getBinIO()), getBinIO()) ;

		UnusedBeforeData.clear() ;
		WavChunkHeader *chhdr = new WavChunkHeader (getBinIO()) ; 
		while (strncmp (chhdr->getChunkId(), "data", 4) != 0) {
			UnusedBeforeData.push_back (new WavChunkUnused (chhdr, getBinIO())) ;
			chhdr = new WavChunkHeader (getBinIO()) ;
		}

		datachhdr = chhdr ;
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
		riffchhdr->write (getBinIO()) ;
		for (int i = 0 ; i < 4 ; i++) {
			getBinIO()->write8 (id_wave[i]) ;
		}

		FormatChunk->write (getBinIO()) ;

		for (std::vector<WavChunkUnused*>::const_iterator i = UnusedBeforeData.begin() ; i != UnusedBeforeData.end() ; i++) {
			(*i)->write (getBinIO()) ;
		}

		datachhdr->write (getBinIO()) ;
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
