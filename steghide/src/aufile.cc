/*
 * steghide 0.4.5 - a steganography program
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

#include "aufile.h"
#include "bufmanag.h"
#include "error.h"
#include "main.h"
#include "cvrstgfile.h"
#include "support.h"
#include "msg.h"

/* TODO for au file format:
   wieso wird audio enconding nicht verwendet - muss nicht immer 8 bit sein !! */

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
	free (header) ;
	free (infofield) ;
	buffree (data) ;
}

BUFFER* AuFile::getData (void)
{
	return data ;
}

void AuFile::setData (BUFFER *d)
{
	// FIXME free old buffer
	data = d ;
}

AuFile::AuHeader* AuFile::getHeader (void)
{
	return header ;
}

void AuFile::setHeader (AuHeader *h)
{
	// FIXME free old header
	header = h ;
}

void AuFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	readheaders () ;
	readdata () ;

	return ;
}

void AuFile::write (void)
{
	CvrStgFile::write () ;

	writeheaders () ;
	writedata () ;

	return ;
}

unsigned long AuFile::getCapacity (void)
{
	return data->length ;
}

void AuFile::embedBit (unsigned long pos, int value)
{
	assert (value == 0 || value == 1) ;
	bufsetbit (data, pos, 0, value) ;

	return ;
}

int AuFile::extractBit (unsigned long pos)
{
	return bufgetbit (data, pos, 0) ;
}

void AuFile::readheaders (void)
{
	try {
		header = (AuHeader *) s_malloc (sizeof (AuHeader)) ;

		header->id[0] = '.' ;
		header->id[1] = 's' ;
		header->id[2] = 'n' ;
		header->id[3] = 'd' ;

		header->offset = BinIO->read32_be() ;
		header->size = BinIO->read32_be() ;
		header->encoding = BinIO->read32_be() ;
		header->samplerate = BinIO->read32_be() ;
		header->channels = BinIO->read32_be() ;

		if ((len_infofield = (header->offset - HeaderSize)) != 0) {
			unsigned char *ptr = NULL ;
			unsigned int i = 0 ;

			infofield = s_malloc (len_infofield) ;

			ptr = (unsigned char *) infofield ;
			for (i = 0 ; i < len_infofield ; i++) {
				ptr[i] = BinIO->read8() ;
			}
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the au headers from the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading au headers."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::STDIN_ERR:
			throw SteghideError (_("an error occured while reading the au headers from standard input.")) ;
			break ;

			case BinaryInputError::STDIN_EOF:
			throw SteghideError (_("premature end of data from standard input while reading au headers.")) ;
			break ;
		}
	}

	return ;
}

void AuFile::writeheaders (void)
{
	try {
		BinIO->write8 (header->id[0]) ;
		BinIO->write8 (header->id[1]) ;
		BinIO->write8 (header->id[2]) ;
		BinIO->write8 (header->id[3]) ;
		BinIO->write32_be (header->offset) ;
		BinIO->write32_be (header->size) ;
		BinIO->write32_be (header->encoding) ;
		BinIO->write32_be (header->samplerate) ;
		BinIO->write32_be (header->channels) ;

		if (len_infofield != 0) {
			unsigned char *ptr = (unsigned char *) infofield ;
			unsigned int i = 0 ;

			for (i = 0 ; i < len_infofield ; i++) {
				BinIO->write8 (ptr[i]) ;
			}
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the au headers to the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the au headers to standard output.")) ;
			break ;
		}
	}

	return ;
}

void AuFile::readdata (void)
{
	try {
		/* if available, use size of audio data (in bytes) to create buffer */
		/* FIXME - wie oft 0xFFFFFFFF ?? */
		if (header->size == 0xFFFFFFFF) {
			data = bufcreate (0) ;
		}
		else {
			data = bufcreate (header->size) ; /* FIXME - test this */
		}

		unsigned long bufpos = 0 ;
		while (!BinIO->eof()) {
			bufsetbyte (data, bufpos, BinIO->read8()) ;
			bufpos++ ;
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

	return ;
}

void AuFile::writedata (void)
{
	try {
		int c = ENDOFBUF ;
		unsigned long bufpos = 0 ;

		while ((c = bufgetbyte (data, bufpos)) != ENDOFBUF) {
			BinIO->write8 ((unsigned char) c) ;
			bufpos++ ;
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

	return ;
}
