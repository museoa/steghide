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
			pverbose (_("reading cover file from standard input.")) ;
		}
		else {
			pverbose (_("reading cover file \"%s\"."), BinIO->getName().c_str()) ;
		}
	}
	else if (args->command.getValue() == EXTRACT) {
		if (BinIO->is_std()) {
			pverbose (_("reading stego file from standard input.")) ;
		}
		else {
			pverbose (_("reading stego file \"%s\"."), BinIO->getName().c_str()) ;
		}
	}
	else {
		assert (0) ;
	}
}

void CvrStgFile::write (void)
{
	if (BinIO->is_std()) {
		pverbose (_("writing stego file to standard output.")) ;
	}
	else {
		pverbose (_("writing stego file \"%s\"."), BinIO->getName().c_str()) ;
	}
}

/* 'creates' a stego file from a cover file */
void CvrStgFile::transform (string stgfn)
{
	delete BinIO ;
	BinIO = new BinaryIO (stgfn, BinaryIO::WRITE) ;

#if 0
	if (file->filename != NULL) {
		if (fclose (file->stream) == EOF) {
			exit_err (_("could not close file \"%s\"."), file->filename) ;
		}
	}

	file->filename = stgfilename ;
	if (file->filename == NULL) {
		file->stream = stdout ;
	}
	else {
		if (!args->force.value) {
			/* check if file already exists */
			if (fileexists (file->filename)) {
				if ((args->cvrfn.value == NULL) || (args->plnfn.value == NULL)) {
					exit_err (_("file \"%s\" does already exist."), file->filename) ;
				}
				else {
					if (!pquestBinIOn (_("file \"%s\" does already exist. overwrite ?"), file->filename)) {
						exit_err (_("did not write to file \"%s\"."), file->filename) ;
					}
				}
			}
		}

		if ((file->stream = fopen (file->filename, "wb")) == NULL) {
			exit_err (_("could not create stego file \"%s\"."), file->filename) ;
		}
	}
#endif

	return ;
}





// FIXME - detectff sollte mit BinaryIO arbeiten und auch zurückgeben oder verwenden oder so - damit CvrStgFile bzw. abgeleitetes Objekt gleich mit BinaryIO weiterarbeiten kann - ohne rewind oder so...



/* detects file format */
static int detectff (BinaryIO *io)
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
			unsigned long rifflen ;

			// FIXME - wie kommt WavFile an rifflen ??
			rifflen = io->read32_le() ;
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

	switch (detectff (BinIO)) {
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
		file = new WavFile (BinIO) ;
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
