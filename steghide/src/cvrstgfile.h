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

#ifndef SH_CVRSTGFILE_H
#define SH_CVRSTGFILE_H

#include <string>

#include "binaryio.h"
#include "cvrstgobject.h"
#include "samplevalue.h"

class CvrStgFile : public CvrStgObject {
	public:
	static CvrStgFile *readFile (std::string fn) ;

	CvrStgFile (void) ;
	CvrStgFile (BinaryIO *io) ;
	virtual ~CvrStgFile (void) ;

	virtual void read (BinaryIO *io) ;
	virtual void write (void) ;
	void transform (std::string fn) ;

	virtual unsigned int getSamplesPerEBit (void) = 0 ;

	/**
	 * get the bit that is embedded in the Sample pos
	 * \param pos the position of the sample
	 * \return the bit that is embedded in the sample corresponding to the given sample position
	 *
	 * This is equivalent to getSample(pos)->getBit().
	 **/
	// FIXME - implement this in ...File to save some time - include tests: implementation in ...File is equivalent to getSample(pos)->getBit()
	Bit getSampleBit (SamplePos pos) { return (getSample(pos)->getBit()) ; }

	protected:
	void setBinIO (BinaryIO *io) ;
	BinaryIO *getBinIO (void) ;

	private:
	static int guessff (BinaryIO *io) ;

	BinaryIO *BinIO ;
} ;

/* constants that indicate the cover file format */
#define FF_UNKNOWN	0
#define FF_BMP		1
#define FF_WAV		2
#define FF_AU		3
#define FF_JPEG		4

#endif /* ndef SH_CVRSTGFILE_H */
