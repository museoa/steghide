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

#include "binaryio.h"
#include "common.h"
#include "error.h"

BinaryIO::BinaryIO (void)
{
	setName ("") ;
	setStream (NULL) ;
	set_open (false) ;
}

BinaryIO::BinaryIO (std::string fn, MODE m)
{
	BinaryIO () ;
	open (fn, m) ;
}

BinaryIO::~BinaryIO (void)
{
	if (is_open()) {
		close () ;
	}
}

FILE *BinaryIO::getStream (void)
{
	return stream ;
}

void BinaryIO::setStream (FILE *s)
{
	stream = s ;
}

std::string BinaryIO::getName (void)
{
	return filename ;
}

void BinaryIO::setName (std::string fn)
{
	filename = fn ;
}

bool BinaryIO::is_open (void)
{
	return fileopen ;
}

void BinaryIO::set_open (bool fo)
{
	fileopen = fo ;
}

bool BinaryIO::is_std (void)
{
	return (getName() == "") ;
}

BinaryIO::MODE BinaryIO::getMode (void)
{
	return mode ;
}

void BinaryIO::setMode (BinaryIO::MODE m)
{
	mode = m ;
}

unsigned long BinaryIO::getPos (void) const
{
	return ftell (stream) ;
}

bool BinaryIO::Fileexists (std::string fn)
{
	bool retval = false ;
	FILE *fd = fopen (fn.c_str(), "r") ;
	if (fd != NULL) {
		retval = true ;
		fclose (fd) ;
	}
	return retval ;
}

void BinaryIO::checkForce (std::string fn)
{
	if (!Args.Force.getValue()) {
		if (Fileexists (fn)) {
			if (Args.stdin_isused()) {
				throw SteghideError (_("the file \"%s\" does already exist."), fn.c_str()) ;
			}
			else {
				Question q (_("the file \"%s\" does already exist. overwrite ?"), fn.c_str()) ;
				q.printMessage() ;
				if (!q.getAnswer()) {
					throw SteghideError (_("did not write to file \"%s\"."), fn.c_str()) ;
				}
			}
		}
	}
}

void BinaryIO::open (std::string fn, MODE m)
{
	if (fn == "") {
		switch (m) {
			case READ: {
				setStream (stdin) ;
			break ; }

			case WRITE: {
				setStream (stdout) ;
			break ; }

			default: {
				assert (0) ;
			break ; }
		}
	}
	else {
		FILE *s = NULL ;
		char *cmode ;

		switch (m) {
			case READ: {
				cmode = "rb" ;
			break ; }

			case WRITE: {
				checkForce (fn) ;
				cmode = "wb" ;
			break ; }
		}

		if ((s = fopen (fn.c_str(), cmode)) == NULL) {
			throw SteghideError (_("could not open the file \"%s\"."), fn.c_str()) ;
		}

		setStream (s) ;
	}
	setName (fn) ;
	setMode (m) ;
	set_open (true) ;
}

bool BinaryIO::eof (void)
{
	// FIXME - is there another way to do this ?
	int c = fgetc (getStream()) ;
	bool retval = feof (getStream()) ;
	ungetc (c, getStream()) ;
	return retval ;
}

void BinaryIO::close (void)
{
	if (getName() != "") {
		if (fclose (getStream()) == EOF) {
			throw SteghideError (_("could not close the file \"%s\"."), getName().c_str()) ;
		}
	}

	setName ("") ;
	setStream (NULL) ;
	set_open (false) ;
}

BYTE BinaryIO::read8 (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int c = EOF ;
	if ((c = fgetc (getStream())) == EOF) {
		throw BinaryInputError (getName(), getStream()) ;
	}

	return (unsigned char) c ;
}

UWORD16 BinaryIO::read16_le (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[2] ;
	for (int i = 0 ; i < 2 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[1] << 8) | bytes[0]) ;
}

UWORD16 BinaryIO::read16_be (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[2] ;
	for (int i = 0 ; i < 2 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[0] << 8) | bytes[1]) ;
}

UWORD32 BinaryIO::read32_le (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[4] ;
	for (int i = 0 ; i < 4 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0]) ;
}

UWORD32 BinaryIO::read32_be (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[4] ;
	for (int i = 0 ; i < 4 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]) ;
}

UWORD32 BinaryIO::read_le (unsigned short n)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;
	assert (n <= 4) ;

	UWORD32 retval = 0 ;
	for (unsigned short i = 0 ; i < n ; i++) {
		int byte = EOF ;
		if ((byte = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
		retval |= ((byte & 0xFF) << (8 * i)) ;
	}

	return retval ;
}

std::string BinaryIO::readstring (unsigned int len)
{
	char retval[len + 1] ;
	for (unsigned int i = 0 ; i < len ; i++) {
		retval[i] = (char) read8() ;
	}
	retval[len] = '\0' ;
	return std::string (retval) ;
}

void BinaryIO::write8 (BYTE val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	if (fputc ((int) val, getStream()) == EOF) {
		throw BinaryOutputError (getName()) ;
	}
}

void BinaryIO::write16_le (UWORD16 val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;
	
	for (int i = 0 ; i <= 1 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write16_be (UWORD16 val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 1 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_le (UWORD32 val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 0 ; i <= 3 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_be (UWORD32 val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 3 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write_le (UWORD32 val, unsigned short n)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;
	assert (n <= 4) ;

	for (short i = 0 ; i < n ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::writestring (std::string s)
{
	if (fputs (s.c_str(), getStream()) == EOF) {
		throw BinaryOutputError (getName()) ;
	}
}
