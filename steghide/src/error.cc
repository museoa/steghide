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

#include <iostream>
#include <stdio.h>
#include <stdarg.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "main.h"

//
// class SteghideError
//
SteghideError::SteghideError (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void SteghideError::printMessage (void)
{
	cerr << PROGNAME << ": " << getMessage() << endl ;
}

//
// class BinaryInputError
//
BinaryInputError::BinaryInputError (string fn, FILE* s)
{
	if (feof (s)) {
		if (fn == "") {
			SteghideError (_("premature end of data from standard input.")) ;
			setType (STDIN_EOF) ;
		}
		else {
			SteghideError (compose (_("premature end of file \"%s\"."), fn.c_str())) ;
			setType (FILE_EOF) ;
		}
	}
	else {
		if (fn == "") {
			SteghideError (_("an error occured while reading data from standard input.")) ;
			setType (STDIN_ERR) ;
		}
		else {
			SteghideError (compose (_("an error occured while reading data from the file \"%s\"."), fn.c_str())) ;
			setType (FILE_ERR) ;
		}
	}
}

BinaryInputError::TYPE BinaryInputError::getType (void)
{
	return type ;
}

void BinaryInputError::setType (BinaryInputError::TYPE t)
{
	type = t ;
}

//
// class BinaryOutputError
//
BinaryOutputError::BinaryOutputError (string fn)
{
	if (fn == "") {
		SteghideError (_("an error occured while writing data to standard output.")) ;
		setType (STDOUT_ERR) ;
	}
	else {
		SteghideError (compose (_("an error occured while writing data to the file \"%s\"."), fn.c_str())) ;
		setType (FILE_ERR) ;
	}
}

BinaryOutputError::TYPE BinaryOutputError::getType (void)
{
	return type ;
}

void BinaryOutputError::setType (BinaryOutputError::TYPE t)
{
	type = t ;
}

//
// UnSupFileFormat
//
UnSupFileFormat::UnSupFileFormat (BinaryIO *io)
{
	if (io->is_std()) {
		SteghideError (_("the file format of the data from standard input is not supported.")) ;
	}
	else {
		SteghideError (compose (_("the file format of the file \"%s\" is not supported."), io->getName().c_str())) ;
	}
}
