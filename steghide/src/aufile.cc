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
#include <vector>

#include "aufile.h"
#include "ausamplevalue.h"
#include "common.h"
#include "cvrstgfile.h"
#include "cvrstgobject.h"
#include "error.h"

AuFile::AuFile (void)
	: CvrStgFile()
{
	// empty
}

AuFile::AuFile (BinaryIO *io)
	: CvrStgFile()
{
	read (io) ;
}

AuFile::~AuFile (void)
{
}

// FIXME - use reference to vector not vector
std::vector<unsigned char> AuFile::getData (void)
{
	return data ;
}

// FIXME - use reference to vector not vector
void AuFile::setData (std::vector<unsigned char> d)
{
	data = d ;
}

void AuFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	readheaders () ;
	readdata () ;
}

void AuFile::write()
{
	CvrStgFile::write () ;

	writeheaders () ;
	writedata () ;
}

unsigned int AuFile::getSamplesPerEBit()
{
	return 2 ;
}

unsigned long AuFile::getNumSamples()
{
	return data.size() ;
}

void AuFile::replaceSample (SamplePos pos, SampleValue *s)
{
	AuSampleValue *sample = dynamic_cast<AuSampleValue*> (s) ;
	assert (sample != NULL) ;
	assert (pos < getNumSamples()) ;
	data[pos] = sample->getValue() ;
}

SampleValue *AuFile::getSample (SamplePos pos)
{
	assert (pos < getNumSamples()) ;
	return ((SampleValue *) new AuSampleValue (this, data[pos])) ;
}

void AuFile::readheaders (void)
{
	try {
		header.id[0] = '.' ;
		header.id[1] = 's' ;
		header.id[2] = 'n' ;
		header.id[3] = 'd' ;

		header.offset = getBinIO()->read32_be() ;
		header.size = getBinIO()->read32_be() ;
		header.encoding = getBinIO()->read32_be() ;
		header.samplerate = getBinIO()->read32_be() ;
		header.channels = getBinIO()->read32_be() ;

		if ((len_infofield = (header.offset - HeaderSize)) != 0) {
			infofield.clear() ;
			for (unsigned long i = 0 ; i < len_infofield ; i++) {
				infofield.push_back (getBinIO()->read8()) ;
			}
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while reading the au headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::FILE_EOF:
			{
				throw SteghideError (_("premature end of file \"%s\" while reading au headers."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::STDIN_ERR:
			{
				throw SteghideError (_("an error occured while reading the au headers from standard input.")) ;
				break ;
			}

			case BinaryInputError::STDIN_EOF:
			{
				throw SteghideError (_("premature end of data from standard input while reading au headers.")) ;
				break ;
			}
		}
	}

	return ;
}

void AuFile::writeheaders (void)
{
	try {
		getBinIO()->write8 (header.id[0]) ;
		getBinIO()->write8 (header.id[1]) ;
		getBinIO()->write8 (header.id[2]) ;
		getBinIO()->write8 (header.id[3]) ;
		getBinIO()->write32_be (header.offset) ;
		getBinIO()->write32_be (header.size) ;
		getBinIO()->write32_be (header.encoding) ;
		getBinIO()->write32_be (header.samplerate) ;
		getBinIO()->write32_be (header.channels) ;

		if (len_infofield != 0) {
			for (unsigned long i = 0 ; i < len_infofield ; i++) {
				getBinIO()->write8 (infofield[i]) ;
			}
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while writing the au headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryOutputError::STDOUT_ERR:
			{
				throw SteghideError (_("an error occured while writing the au headers to standard output.")) ;
				break ;
			}
		}
	}

	return ;
}

void AuFile::readdata (void)
{
	try {
		/* if available, use size of audio data (in bytes) to create buffer */
		data.clear() ;
		if (header.size != 0xFFFFFFFF) {
			data.reserve (header.size) ;
		}

		while (!getBinIO()->eof()) {
			data.push_back (getBinIO()->read8()) ;
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while reading the audio data from the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::FILE_EOF:
			{
				throw SteghideError (_("premature end of file \"%s\" while reading audio data."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::STDIN_ERR:
			{
				throw SteghideError (_("an error occured while reading the audio data from standard input.")) ;
				break ;
			}

			case BinaryInputError::STDIN_EOF:
			{
				throw SteghideError (_("premature end of data from standard input while reading audio data.")) ;
				break ;
			}
		}
	}

	return ;
}

void AuFile::writedata (void)
{
	try {
		for (std::vector<unsigned char>::iterator i = data.begin() ; i != data.end() ; i++) {
			getBinIO()->write8 (*i) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while writing the audio data to the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryOutputError::STDOUT_ERR:
			{
				throw SteghideError (_("an error occured while writing the audio data to standard output.")) ;
				break ;
			}
		}
	}

	return ;
}
