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

#ifndef SH_JPEGFILE_H
#define SH_JPEGFILE_H

#include "binaryio.h"
#include "cvrstgfile.h"

class JpegFile : public CvrStgFile {
	public:
	JpegFile (void) ;
	JpegFile (BinaryIO *io) ;
	~JpegFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;
	unsigned long getCapacity (void) ;
	void embedBit (unsigned long pos, int bit) ;
	int extractBit (unsigned long pos) ;

	protected:
} ;

#endif // ndef SH_JPEGFILE_H
