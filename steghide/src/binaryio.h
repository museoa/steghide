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

#ifndef SH_BINARYIO_H
#define SH_BINARYIO_H

#include <cstdio>
#include <string>

#include "common.h"

class BinaryIO {
	public:
	enum MODE { READ, WRITE } ;

	BinaryIO (void) ;
	/**
	 * construct a BinaryIO object
	 * \param fn the filename (can be "" to indicate stdin/stdout)
	 * \m the Mode (BinaryIO::READ or BinaryIO::WRITE)
	 **/
	BinaryIO (std::string fn, MODE m) ;
	~BinaryIO (void) ;

	std::string getName (void) ;
	bool is_open (void) ;
	bool is_std (void) ;
	bool eof (void) ;
	unsigned long getPos (void) const ;

	void open (std::string fn, MODE m) ;	
	void close (void) ;

	BYTE read8 (void) ;
	UWORD16 read16_le (void) ;
	UWORD16 read16_be (void) ;
	UWORD32 read32_le (void) ;
	UWORD32 read32_be (void) ;
	/**
	 * read n bytes (little endian byte ordering)
	 * \param n the number of bytes to read (must be <= 4)
	 **/
	UWORD32 read_le (unsigned short n) ;
	std::string readstring (unsigned int len) ;

	void write8 (BYTE val) ;
	void write16_le (UWORD16 val) ;
	void write16_be (UWORD16 val) ;
	void write32_le (UWORD32 val) ;
	void write32_be (UWORD32 val) ;
	/**
	 * write n bytes of val (little endian byte ordering)
	 * \param n the number of bytes to write (must be <= 4)
	 * \param val the value
	 **/
	void write_le (UWORD32 val, unsigned short n) ;
	void writestring (std::string s) ;

	protected:
	void setStream (FILE *s) ;
	FILE *getStream (void) ;
	void setName (std::string fn) ;
	void set_open (bool fo) ;
	MODE getMode (void) ;
	void setMode (BinaryIO::MODE m) ;

	void checkForce (std::string fn) ;
	bool Fileexists (std::string fn) ;

	private:
	std::string filename ;
	FILE *stream ;
	bool fileopen ;
	MODE mode ;
} ;

#endif /* ndef SH_BINARYIO_H */
