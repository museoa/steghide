/*
 * steghide 0.4.4 - a steganography program
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "main.h"
#include "io.h"
#include "bufmanag.h"
#include "msg.h"
#include "support.h"
#include "stegano.h"
#include "hash.h"

/* defines use or non-use of the noncvr buffer per file format
the array indices are the values of the FF_XXX constants */
int noncvrbufuse[] = {	0,	/* FF_UNKNOWN ; not used */
						0,	/* FF_BMP */
						1,	/* FF_WAV */
						0,	/* FF_AU */	} ;

/* auto-detects file format while reading headers into file->headers */
void readheaders (CVRFILE *file)
{
	char buf[4] = { '\0', '\0', '\0', '\0' } ;
	int i = 0 ;

	for (i = 0 ; i < 2 ; i++) {
		buf[i] = (char) getc (file->stream) ;
	}

	if (strncmp ("BM", buf, 2) == 0) {
		file->fileformat = FF_BMP ;
		bmp_readheaders (file) ;
	}
	else {
		for (i = 2 ; i < 4 ; i++) {
			buf[i] = (char) getc (file->stream) ;
		}

		if (strncmp (".snd", buf, 4) == 0) {
			file->fileformat = FF_AU ;
			au_readheaders (file) ;
		}
		else if (strncmp ("RIFF", buf, 4) == 0) {
			unsigned long rifflen ;

			rifflen = read32_le (file->stream) ;
			for (i = 0 ; i < 4 ; i++) {
				buf[i] = getc (file->stream) ;
			}
			if (strncmp ("WAVE", buf, 4) == 0) {
				file->fileformat = FF_WAV ;
				wav_readheaders (file, rifflen) ;
			}
			else {
				if (file->filename == NULL) {
					exit_err (_("the file format of the data from standard input is not supported.")) ;
				}
				else {
					exit_err (_("the file format of the file \"%s\" is not supported."), file->filename) ;
				}
			}
		}
		else {
			if (file->filename == NULL) {
				exit_err (_("the file format of the data from standard input is not supported.")) ;
			}
			else {
				exit_err (_("the file format of the file \"%s\" is not supported."), file->filename) ;
			}
		}
	}

	return ;
}

/* creates a CVRFILE structure that is used to save the stego data to disk */
CVRFILE *createstgfile (CVRFILE *cvrfile, char *stgfilename)
{
	CVRFILE *stgfile = NULL ;

	if (stgfilename == NULL) {
		pverbose (_("writing stego file to standard output.")) ;
	}
	else {
		pverbose (_("writing stego file \"%s\"."), stgfilename) ;
	}

	stgfile = s_malloc (sizeof *stgfile) ;
	
	if (stgfilename == NULL) {
		stgfile->filename = NULL ;
	}
	else {
		stgfile->filename = stgfilename ;
	}
	
	stgfile->fileformat = cvrfile->fileformat ;
	stgfile->headers = cvrfile->headers ;
	stgfile->unsupdata1 = cvrfile->unsupdata1 ;
	stgfile->unsupdata1len = cvrfile->unsupdata1len ;
	stgfile->unsupdata2 = cvrfile->unsupdata2 ;
	stgfile->unsupdata2len = cvrfile->unsupdata2len ;
	stgfile->cvrdata = cvrfile->cvrdata ;
	if (noncvrbufuse[stgfile->fileformat]) {
		stgfile->noncvrdata = cvrfile->noncvrdata ;
	}
	else {
		stgfile->noncvrdata = NULL ;
	}

	return stgfile ;
}

void assemble_plndata (PLNFILE *plnfile)
{
	BUFFER *buf = NULL, *tmpbuf = NULL ;
	unsigned long pos = 0 ;
	char *tmp = NULL ;
	int nbytes_plnfilename = 0 ;
	int i = 0 ;

	buf = bufcreate (1) ;
	
	if ((args.plnfn.value == NULL) || (!args.embedplnfn.value)) {
		/* standard input is used or plain file name embedding has
		   been turned off explicitly */
		bufsetbyte (buf, pos++, 0) ;
	}
	else {
		assert (args.plnfn.is_set) ;

		tmp = stripdir (args.plnfn.value) ;

		if ((nbytes_plnfilename = strlen (tmp)) > PLNFILENAME_MAXLEN) {
			exit_err (_("the maximum length for the plain file name is %d characters."), PLNFILENAME_MAXLEN) ;
		}
		bufsetbyte (buf, pos++, nbytes_plnfilename) ;
		
		for (i = 0 ; i < nbytes_plnfilename ; i++) {
			bufsetbyte (buf, pos++, tmp[i]) ;
		}
	}

	if (args.checksum.value) {
		unsigned char *uc_crc32 = getcrc32 (plnfile) ;
		bufsetbyte (buf, pos++, (int) uc_crc32[0]) ;
		bufsetbyte (buf, pos++, (int) uc_crc32[1]) ;
		bufsetbyte (buf, pos++, (int) uc_crc32[2]) ;
		bufsetbyte (buf, pos++, (int) uc_crc32[3]) ;
	}

	tmpbuf = bufappend (buf, plnfile->plndata) ;
	buffree (plnfile->plndata) ;
	buffree (buf) ;
	plnfile->plndata = tmpbuf ;

	return ;
}

void deassemble_plndata (PLNFILE *plnfile)
{
	char plnfilename[PLNFILENAME_MAXLEN + 1] ;
	unsigned long pos = 0 ;
	int nbytes_plnfilename = 0 ;
	BUFFER *tmp = NULL ;
	int i = 0 ;
	unsigned char *uc_crc32 = NULL ;

	/* read plain file name embedded in stego file */
	nbytes_plnfilename = bufgetbyte (plnfile->plndata, pos++) ;
	for (i = 0 ; i < nbytes_plnfilename ; i++) {
		plnfilename[i] = (char) bufgetbyte (plnfile->plndata, pos++) ;
	}
	plnfilename[i] = '\0' ;

	if (args.plnfn.is_set) {
		if (args.plnfn.value == NULL) {
			/* write pln data to stdout */
			plnfile->filename = NULL ;
		}
		else {
			/* write pln data to file with given name */
			plnfile->filename = s_malloc (strlen (args.plnfn.value) + 1) ;
			strcpy (plnfile->filename, args.plnfn.value) ;
		}
	}
	else {
		/* write pln data to file with name stored in stego file */
		assert (args.plnfn.value == NULL) ;

		if (nbytes_plnfilename == 0) {
			exit_err (_("please specify a name for the plain file (there is none embedded in the stego file).")) ;
		}

		plnfile->filename = s_malloc (strlen (plnfilename) + 1) ;
		strcpy (plnfile->filename, plnfilename) ;
	}

	if (sthdr.checksum == CHECKSUM_CRC32) {
		uc_crc32 = s_malloc (4) ;
		uc_crc32[0] = (unsigned char) bufgetbyte (plnfile->plndata, pos++) ;
		uc_crc32[1] = (unsigned char) bufgetbyte (plnfile->plndata, pos++) ;
		uc_crc32[2] = (unsigned char) bufgetbyte (plnfile->plndata, pos++) ;
		uc_crc32[3] = (unsigned char) bufgetbyte (plnfile->plndata, pos++) ;
	}

	tmp = bufcut (plnfile->plndata, pos, plnfile->plndata->length - 1) ;
	buffree (plnfile->plndata) ;
	plnfile->plndata = tmp ;

	if (sthdr.checksum == CHECKSUM_CRC32) {
		if (checkcrc32 (plnfile, uc_crc32)) {
			pverbose (_("crc32 checksum test ok.")) ;
		}
		else {
			pwarn (_("crc32 checksum failed! extracted data is probably corrupted.")) ;
		}
	}

	return ;
}

/* creates a PLNFILE structure that is used to save the plaindata to disk */
PLNFILE *createplnfile (void)
{
	PLNFILE *plnfile = NULL ;

	plnfile = s_malloc (sizeof *plnfile) ;

	plnfile->filename = NULL ;	/* will be filled later by deassemble_plndata() */
	plnfile->stream = NULL ;	/* will be filled later be deassemble_plndata() */
	plnfile->plndata = NULL ;	/* will be filled later by extractdata() */

	return plnfile ;
}

/* cleans up a CVRFILE structure (closes file, frees memory) */
void cleanupcvrfile (CVRFILE *cvrfile, int freesubstructs)
{
	if (freesubstructs == FSS_YES) {
		free (cvrfile->headers) ;
		if (cvrfile->unsupdata1len != 0) {
			free (cvrfile->unsupdata1) ;
		}
		if (cvrfile->unsupdata2len != 0) {
			free (cvrfile->unsupdata2) ;
		}
		buffree (cvrfile->cvrdata) ;
		if (noncvrbufuse[cvrfile->fileformat]) {
			buffree (cvrfile->noncvrdata) ;
		}
	}
	
	if (cvrfile->filename != "-") {
		fclose (cvrfile->stream) ;
	}
	
	free (cvrfile->filename) ;
	free (cvrfile) ;
}

/* clean up a PLNFILE structure (closes file, frees memory) */
void cleanupplnfile (PLNFILE *plnfile)
{
	if ((plnfile->filename != "-") && (plnfile->stream != NULL)) {
		fclose (plnfile->stream) ;
	}
	if (plnfile->filename != NULL) {
		free (plnfile->filename) ;
	}
	if (plnfile->plndata != NULL) {
		buffree (plnfile->plndata) ;
	}
	free (plnfile) ;
}

/* reads a cover data file into a CVRFILE structure */
CVRFILE *readcvrfile (char *filename)
{
	CVRFILE *cvrfile = NULL ;

	if (args.action.value == ARGS_ACTION_EMBED) {
		if (filename == NULL) {
			pverbose (_("reading cover file from standard input.")) ;
		}
		else {
			pverbose (_("reading cover file \"%s\"."), filename) ;
		}
	}
	else if (args.action.value == ARGS_ACTION_EXTRACT) {
		if (filename == NULL) {
			pverbose (_("reading stego file from standard input.")) ;
		}
		else {
			pverbose (_("reading stego file \"%s\"."), filename) ;
		}
	}
	else {
		assert (0) ;
	}

	/* fill cvrfile structure */
	cvrfile = s_malloc (sizeof *cvrfile) ;

	if (filename == NULL) {
		cvrfile->stream = stdin ;
		cvrfile->filename = NULL ;
	}
	else {
		if ((cvrfile->stream = fopen (filename, "rb")) == NULL) {
			free (cvrfile) ;
			exit_err (_("could not open the file \"%s\"."), filename) ;
		}
		cvrfile->filename = filename ;
	}

	cvrfile->headers = s_malloc (sizeof *cvrfile->headers) ;

	readheaders (cvrfile) ;

	/* read the file (this fills the rest of cvrfile) */
	cvrfile->cvrdata = bufcreate (0) ; /* FIXME - length should be set intelligently in *_readfile */
	if (noncvrbufuse[cvrfile->fileformat]) {
		cvrfile->noncvrdata = bufcreate (0) ;
	}
	else {
		cvrfile->noncvrdata = NULL ;
	}

	switch (cvrfile->fileformat) {
		case FF_BMP:
		bmp_readfile (cvrfile) ;
		break ;

		case FF_WAV:
		wav_readfile (cvrfile) ;
		break ;

		case FF_AU:
		au_readfile (cvrfile) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return cvrfile ;
}

/* writes the file described in the cvrfile structure to disk */
void writestgfile (CVRFILE *stgfile)
{
	if (stgfile->filename == NULL) {
		stgfile->stream = stdout ;
	}
	else {
		if (!args.force.value) {
			/* check if file already exists */
			if (fileexists (stgfile->filename)) {
				if ((args.cvrfn.value == NULL) || (args.plnfn.value == NULL)) {
					exit_err (_("file \"%s\" does already exist."), stgfile->filename) ;
				}
				else {
					if (!pquestion (_("file \"%s\" does already exist. overwrite ?"), stgfile->filename)) {
						exit_err (_("did not write to file \"%s\"."), stgfile->filename) ;
					}
				}
			}
		}

		if ((stgfile->stream = fopen (stgfile->filename, "wb")) == NULL) {
			exit_err (_("could not create stego file \"%s\"."), stgfile->filename) ;
		}
	}
	
	switch (stgfile->fileformat) {
		case FF_BMP:
		bmp_writefile (stgfile) ;
		break ;

		case FF_WAV:
		wav_writefile (stgfile) ;
		break ;

		case FF_AU:
		au_writefile (stgfile) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	if (args.action.value == ARGS_ACTION_EMBED) {
		if (stgfile->filename == NULL) {
			pverbose (_("wrote stego file to standard output.")) ;
		}
		else {
			pmsg (_("wrote stego file to \"%s\"."), stgfile->filename) ;
		}
	}
	else {
		assert (0) ;
	}

	return ;
}
	
/* reads a plain data file into a PLNFILE structure */
PLNFILE *readplnfile (char *filename)
{
	PLNFILE *plnfile = NULL ;
	int c = EOF ;
	unsigned long bufpos = 0 ;

	if (filename == NULL) {
		pverbose (_("reading plain file from standard input.")) ;
	}
	else {
		pverbose (_("reading plain file \"%s\"."), filename) ;
	}

	plnfile = s_malloc (sizeof *plnfile) ;

	if (filename == NULL) {
		plnfile->stream = stdin ;
		plnfile->filename = NULL ;
	}
	else {
		if ((plnfile->stream = fopen (filename, "rb")) == NULL) {
			exit_err (_("could not open file \"%s\"."), filename) ;
		}
		plnfile->filename = filename ;
	}

	plnfile->plndata = bufcreate (0) ; /* FIXME - ? set length more intelligently */

	while ((c = getc (plnfile->stream)) != EOF) {
		bufsetbyte (plnfile->plndata, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			exit_err (_("an error occured while reading the plain data from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the file \"%s\"."), filename) ;
		}
	}

	return plnfile ;
}

/* writes the file described in the plnfile structure to disk */
void writeplnfile (PLNFILE *plnfile)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;

	if (plnfile->filename == NULL) {
		pverbose (_("writing plain file to standard output.")) ;
	}
	else {
		pmsg (_("writing plain file to \"%s\"."), plnfile->filename) ;
	}

	if (plnfile->filename == NULL) {
		plnfile->stream = stdout ;
	}
	else {
		if (!args.force.value) {
			/* check if file already exists */
			if (fileexists (plnfile->filename)) {
				if (args.stgfn.value == NULL) {
					exit_err (_("file \"%s\" does already exist."), plnfile->filename) ;
				}
				else {
					if (!pquestion (_("file \"%s\" does already exist. overwrite ?"), plnfile->filename)) {
						exit_err (_("did not write to file \"%s\"."), plnfile->filename) ;
					}
				}
			}
		}

		if ((plnfile->stream = fopen (plnfile->filename, "wb")) == NULL) {
			exit_err (_("could not create plain file \"%s\"."), plnfile->filename) ;
		}
	}

	while ((c = bufgetbyte (plnfile->plndata, bufpos)) != ENDOFBUF) {
		putc (c, plnfile->stream) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			exit_err (_("an error occured while writing the plain data to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing to the file \"%s\"."), plnfile->filename) ;
		}
	}

	return ;
}

