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

#ifndef SH_ERROR_H
#define SH_ERROR_H

#include <cstdio>
#include <string>

#include "binaryio.h"
#include "msg.h"

class SteghideError : MessageBase {
	public:
	SteghideError (void) : MessageBase() {} ;
	SteghideError (string msg) : MessageBase (msg) {} ;
	SteghideError (const char *msgfmt, ...) ;

	void printMessage (void) ;	
} ;

class BinaryInputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, FILE_EOF, STDIN_ERR, STDIN_EOF } ;

	BinaryInputError (string fn, FILE* s) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

class BinaryOutputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, STDOUT_ERR } ;

	BinaryOutputError (string fn) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

class UnSupFileFormat : public SteghideError {
	public:
	UnSupFileFormat (BinaryIO *io) ;
} ;

#endif
