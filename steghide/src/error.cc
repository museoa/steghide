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

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "common.h"
#include "error.h"

//
// class SteghideError
//
SteghideError::SteghideError (void)
	: MessageBase(_("error, exiting. (no error message defined)"))
{
}

SteghideError::SteghideError(std::string msg)
	: MessageBase(msg)
{
}

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
	std::cerr << PROGNAME << ": " << getMessage() << std::endl ;
}

//
// class BinaryInputError
//
BinaryInputError::BinaryInputError (std::string fn, FILE* s)
	: SteghideError()
{
	if (feof (s)) {
		if (fn == "") {
			setMessage(_("premature end of data from standard input.")) ;
			setType (STDIN_EOF) ;
		}
		else {
			setMessage(compose (_("premature end of file \"%s\"."), fn.c_str())) ;
			setType (FILE_EOF) ;
		}
	}
	else {
		if (fn == "") {
			setMessage(_("an error occured while reading data from standard input.")) ;
			setType (STDIN_ERR) ;
		}
		else {
			setMessage(compose (_("an error occured while reading data from the file \"%s\"."), fn.c_str())) ;
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
BinaryOutputError::BinaryOutputError (std::string fn)
	: SteghideError()
{
	if (fn == "") {
		setMessage(_("an error occured while writing data to standard output.")) ;
		setType(STDOUT_ERR) ;
	}
	else {
		setMessage(compose (_("an error occured while writing data to the file \"%s\"."), fn.c_str())) ;
		setType(FILE_ERR) ;
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
// class UnSupFileFormat
//
UnSupFileFormat::UnSupFileFormat (BinaryIO *io)
	: SteghideError()
{
	if (io->is_std()) {
		setMessage(_("the file format of the data from standard input is not supported.")) ;
	}
	else {
		setMessage(compose (_("the file format of the file \"%s\" is not supported."), io->getName().c_str())) ;
	}
}

//
// class CorruptJpegError
//
CorruptJpegError::CorruptJpegError (BinaryIO *io, const char *msgfmt, ...)
	: SteghideError()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	std::string auxmsg = vcompose (msgfmt, ap) ;
	va_end (ap) ;

	std::string mainmsg ;
	if (io->is_std()) {
		mainmsg = std::string (_("corrupt jpeg file on standard input:")) ;
	}
	else {
		mainmsg = compose (_("corrupt jpeg file \"%s\":"), io->getName().c_str()) ;
	}

	setMessage(mainmsg + " " + auxmsg) ;
}
