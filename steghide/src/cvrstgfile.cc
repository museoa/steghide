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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "common.h"
#include "cvrstgfile.h"
#include "aufile.h"
#include "bmpfile.h"
#include "error.h"
#include "jpegfile.h"
#include "msg.h"
#include "wavfile.h"

CvrStgFile::CvrStgFile (void)
{
	// empty
}

CvrStgFile::CvrStgFile (BinaryIO *io)
{
	read (io) ;
}

CvrStgFile::~CvrStgFile (void)
{
	delete getBinIO() ;
}

void CvrStgFile::read (BinaryIO *io)
{
	setBinIO (io) ;

	if (Args.Command.getValue() == EMBED) {
		if (getBinIO()->is_std()) {
			VerboseMessage v (_("reading cover file from standard input.")) ;
			v.printMessage() ;
		}
		else {
			VerboseMessage v (_("reading cover file \"%s\"."), getBinIO()->getName().c_str()) ;
			v.printMessage() ;
		}
	}
	else if (Args.Command.getValue() == EXTRACT) {
		if (getBinIO()->is_std()) {
			VerboseMessage v (_("reading stego file from standard input.")) ;
			v.printMessage() ;
		}
		else {
			VerboseMessage v (_("reading stego file \"%s\"."), getBinIO()->getName().c_str()) ;
			v.printMessage() ;
		}
	}
}

void CvrStgFile::write (void)
{
	if (getBinIO()->is_std()) {
		VerboseMessage v (_("writing stego file to standard output.")) ;
		v.printMessage() ;
	}
	else {
		VerboseMessage v (_("writing stego file \"%s\"."), getBinIO()->getName().c_str()) ;
		v.printMessage() ;
	}
}

/* 'creates' a stego file from a cover file */
void CvrStgFile::transform (std::string stgfn)
{
	delete getBinIO() ;
	setBinIO (new BinaryIO (stgfn, BinaryIO::WRITE)) ;
}

// guesses the file format by looking for magic values in the first few bytes
int CvrStgFile::guessff (BinaryIO *io)
{
	char buf[4] = { '\0', '\0', '\0', '\0' } ;
	int retval = FF_UNKNOWN ;
	
	for (unsigned int i = 0 ; i < 2 ; i++) {
		buf[i] = (char) io->read8() ;
	}

	if (strncmp ("BM", buf, 2) == 0) {
		retval = FF_BMP ;
	}
	else if ((unsigned char) buf[0] == 0xFF && (unsigned char) buf[1] == 0xD8) {
		retval = FF_JPEG ;
	}
	else {
		for (unsigned int i = 2 ; i < 4 ; i++) {
			buf[i] = (char) io->read8() ;
		}

		if (strncmp (".snd", buf, 4) == 0) {
			retval = FF_AU ;
		}
		else if (strncmp ("RIFF", buf, 4) == 0) {
			retval = FF_WAV ;
		}
	}

	return retval ;
}

CvrStgFile *CvrStgFile::readFile (std::string fn)
{
	BinaryIO *BinIO = new BinaryIO (fn, BinaryIO::READ) ;

	CvrStgFile *file = NULL ;
	switch (guessff (BinIO)) {
		case FF_UNKNOWN: {
			throw UnSupFileFormat (BinIO) ;	
		break ; }

		case FF_BMP: {
			file = new BmpFile (BinIO) ;
		break ; }

		case FF_WAV: {
			file = new WavFile (BinIO) ;
		break ; }

		case FF_AU: {
			file = new AuFile (BinIO) ;
		break ; }

		case FF_JPEG: {
			file = new JpegFile (BinIO) ;
		break ; }

		default: {
			assert (0) ;
		break ; }
	}

	return file ;
}
