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

#ifndef SH_BINARYIO_H
#define SH_BINARYIO_H

#include <stdio.h>
#include <string>

class BinaryIO {
	public:
	enum MODE { READ, WRITE } ;

	BinaryIO (void) ;
	BinaryIO (string fn, MODE m) ;
	~BinaryIO (void) ;

	string getName (void) ;
	bool is_open (void) ;
	bool is_std (void) ;
	bool eof (void) ;

	void open (string fn, MODE m) ;	
	void close (void) ;

	unsigned char read8 (void) ;
	unsigned int read16_le (void) ;
	unsigned int read16_be (void) ;
	unsigned long read32_le (void) ;
	unsigned long read32_be (void) ;
	string readstring (unsigned int len) ;

	void write8 (unsigned char val) ;
	void write16_le (unsigned int val) ;
	void write16_be (unsigned int val) ;
	void write32_le (unsigned long val) ;
	void write32_be (unsigned long val) ;
	void writestring (string s) ;

	protected:
	void setStream (FILE *s) ;
	FILE *getStream (void) ;
	void setName (string fn) ;
	void set_open (bool fo) ;
	MODE getMode (void) ;
	void setMode (BinaryIO::MODE m) ;

	private:
	string filename ;
	FILE *stream ;
	bool fileopen ;
	MODE mode ;
} ;

#endif /* ndef SH_BINARYIO_H */
