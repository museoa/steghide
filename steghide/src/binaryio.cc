/*
 * steghide 0.4.6b - a steganography program
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
#include <sstream>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "binaryio.h"
#include "error.h"
#include "msg.h"
#include "support.h"

BinaryIO::BinaryIO (void)
{
	setName ("") ;
	setStream (NULL) ;
	set_open (false) ;
}

BinaryIO::BinaryIO (string fn, MODE m)
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

string BinaryIO::getName (void)
{
	return filename ;
}

void BinaryIO::setName (string fn)
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

void BinaryIO::open (string fn, MODE m)
{
	if (fn == "") {
		switch (m) {
			case READ:
			{
				setStream (stdin) ;
				break ;
			}

			case WRITE:
			{
				setStream (stdout) ;
				break ;
			}

			default:
			{
				assert (0) ;
				break ;
			}
		}
	}
	else {
		FILE *s = NULL ;
		char *cmode ;

		switch (m) {
			case READ:
			{
				cmode = "rb" ;
				break ;
			}

			case WRITE:
			{
				checkforce (fn.c_str()) ;
				cmode = "wb" ;
				break ;
			}
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
	// FIXME
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

unsigned char BinaryIO::read8 (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int c = EOF ;
	if ((c = fgetc (getStream())) == EOF) {
		throw BinaryInputError (getName(), getStream()) ;
	}

	return (unsigned char) c ;
}

unsigned int BinaryIO::read16_le (void)
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

unsigned int BinaryIO::read16_be (void)
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

unsigned long BinaryIO::read32_le (void)
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

unsigned long BinaryIO::read32_be (void)
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

string BinaryIO::readstring (unsigned int len)
{
	ostringstream ost ;
	for (unsigned int i = 0 ; i < len ; i++) {
		ost << read8() ;
	}
	return ost.str() ;
}

void BinaryIO::write8 (unsigned char val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	if (fputc ((int) val, getStream()) == EOF) {
		throw BinaryOutputError (getName()) ;
	}
}

void BinaryIO::write16_le (unsigned int val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;
	
	for (int i = 0 ; i <= 1 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write16_be (unsigned int val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 1 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_le (unsigned long val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 0 ; i <= 3 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_be (unsigned long val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 3 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::writestring (string s)
{
	if (fputs (s.c_str(), getStream()) == EOF) {
		throw BinaryOutputError (getName()) ;
	}
}
