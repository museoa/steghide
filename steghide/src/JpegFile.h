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

#ifndef SH_JPEGFILE_H
#define SH_JPEGFILE_H

#include <vector>

#include "BinaryIO.h"
#include "CvrStgFile.h"
#include "jpegframe.h"

/**
 * \class JpegFile
 * \brief a cover/stego file in jpeg, i.e. jfif format
 **/
class JpegFile : public CvrStgFile {
	public:
	JpegFile (void) ;
	JpegFile (BinaryIO *io) ;
	~JpegFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (SamplePos pos) const ;
	unsigned int getSamplesPerEBit (void) const ;

	private:
	/// the frame of the jpeg file
	JpegFrame *frame ;
} ;

#endif // ndef SH_JPEGFILE_H
