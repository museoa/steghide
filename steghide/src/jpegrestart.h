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

#ifndef SH_JPEGRESTART_H
#define SH_JPEGRESTART_H

#include "binaryio.h"
#include "jpegbase.h"

class JpegDefineRestartInterval : public JpegSegment {
	public:
	JpegDefineRestartInterval (void) ;
	JpegDefineRestartInterval (BinaryIO *io) ;
	virtual ~JpegDefineRestartInterval (void) ;

	/**
	 * read a marker segment which defines the restart interval
	 * \param io the jpeg stream
	 **/
	void read (BinaryIO *io) ;

	/**
	 * write a marker segment which defines the restart interval
	 * \param io the jpeg stream
	 **/
	void write (BinaryIO *io) ;

	private:
	unsigned int restartinterval ;
} ;

#endif //ndef SH_JPEGRESTART_H