#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "main.h"
#include "cvrstgfile.h"
#include "bufmanag.h"
#include "msg.h"
#include "support.h"
#include "stegano.h"
#include "hash.h"
#include "bmpfile.h"
#include "wavfile.h"
#include "aufile.h"

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
	delete BinIO ;
}

void CvrStgFile::read (BinaryIO *io)
{
	BinIO = io ;

	if (args->command.getValue() == EMBED) {
		if (BinIO->is_std()) {
			VerboseMessage v (_("reading cover file from standard input.")) ;
			v.printMessage() ;
		}
		else {
			VerboseMessage v (_("reading cover file \"%s\"."), BinIO->getName().c_str()) ;
			v.printMessage() ;
		}
	}
	else if (args->command.getValue() == EXTRACT) {
		if (BinIO->is_std()) {
			VerboseMessage v (_("reading stego file from standard input.")) ;
			v.printMessage() ;
		}
		else {
			VerboseMessage v (_("reading stego file \"%s\"."), BinIO->getName().c_str()) ;
			v.printMessage() ;
		}
	}
	else {
		assert (0) ;
	}
}

void CvrStgFile::write (void)
{
	if (BinIO->is_std()) {
		VerboseMessage v (_("writing stego file to standard output.")) ;
		v.printMessage() ;
	}
	else {
		VerboseMessage v (_("writing stego file \"%s\"."), BinIO->getName().c_str()) ;
		v.printMessage() ;
	}
}

/* 'creates' a stego file from a cover file */
void CvrStgFile::transform (string stgfn)
{
	delete BinIO ;
	BinIO = new BinaryIO (stgfn, BinaryIO::WRITE) ;
}

/* detects file format */
static int detectff (BinaryIO *io, unsigned long *rifflen)
{
	char buf[4] = { '\0', '\0', '\0', '\0' } ;
	int retval = FF_UNKNOWN ;
	
	for (unsigned int i = 0 ; i < 2 ; i++) {
		buf[i] = (char) io->read8() ;
	}

	if (strncmp ("BM", buf, 2) == 0) {
		retval = FF_BMP ;
	}
	else {
		for (unsigned int i = 2 ; i < 4 ; i++) {
			buf[i] = (char) io->read8() ;
		}

		if (strncmp (".snd", buf, 4) == 0) {
			retval = FF_AU ;
		}
		else if (strncmp ("RIFF", buf, 4) == 0) {
			*rifflen = io->read32_le() ;
			for (unsigned int i = 0 ; i < 4 ; i++) {
				buf[i] = (char) io->read8() ;
			}
			if (strncmp ("WAVE", buf, 4) == 0) {
				retval = FF_WAV ;
			}
		}
	}

	return retval ;
}

CvrStgFile *cvrstg_readfile (string filename)
{
	BinaryIO *BinIO = new BinaryIO (filename, BinaryIO::READ) ;

	CvrStgFile *file = NULL ;
	unsigned long rifflen = 0 ;
	switch (detectff (BinIO, &rifflen)) {
		case FF_UNKNOWN:
		if (BinIO->is_std()) {
			throw SteghideError (_("the file format of the data from standard input is not supported.")) ;
		}
		else {
			throw SteghideError (_("the file format of the file \"%s\" is not supported."), BinIO->getName().c_str()) ;
		}
		break ;

		case FF_BMP:
		file = new BmpFile (BinIO) ;
		break ;

		case FF_WAV:
		file = new WavFile (BinIO, rifflen) ;
		break ;

		case FF_AU:
		file = new AuFile (BinIO) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return file ;
}
