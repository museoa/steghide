/*
 * steghide 0.4.2 - a steganography program
 * Copyright (C) 2001 Stefan Hetzl <shetzl@teleweb.at>
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

#include "main.h"
#include "io.h"
#include "bufmanag.h"
#include "msg.h"
#include "support.h"
#include "stegano.h"
#include "hash.h"

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
					exit_err ("the file format of the data from standard input is not supported.") ;
				}
				else {
					exit_err ("the file format of the file \"%s\" is not supported.", file->filename) ;
				}
			}
		}
		else {
			if (file->filename == NULL) {
				exit_err ("the file format of the data from standard input is not supported.") ;
			}
			else {
				exit_err ("the file format of the file \"%s\" is not supported.", file->filename) ;
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
		pverbose ("writing stego file to standard output.") ;
	}
	else {
		pverbose ("writing stego file \"%s\".", stgfilename) ;
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
	stgfile->cvrbuflhead = cvrfile->cvrbuflhead ;
	if (noncvrbufuse[stgfile->fileformat]) {
		stgfile->noncvrbuflhead = cvrfile->noncvrbuflhead ;
	}
	else {
		stgfile->noncvrbuflhead = NULL ;
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

	buf = createbuflist () ;
	
	if (args.plnfn.value == NULL) {
		/* standard input is used */
		bufsetbyte (buf, pos++, 0) ;
	}
	else {
		assert (args.plnfn.is_set) ;

		tmp = stripdir (args.plnfn.value) ;

		if ((nbytes_plnfilename = strlen (tmp)) > PLNFILENAME_MAXLEN) {
			exit_err ("the maximum length for the plain file name is %d characters.", PLNFILENAME_MAXLEN) ;
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

	tmpbuf = bufcat (buf, plnfile->plnbuflhead) ;
	buffree (plnfile->plnbuflhead) ;
	buffree (buf) ;
	plnfile->plnbuflhead = tmpbuf ;

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
	nbytes_plnfilename = bufgetbyte (plnfile->plnbuflhead, pos++) ;
	for (i = 0 ; i < nbytes_plnfilename ; i++) {
		plnfilename[i] = (char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
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
			exit_err ("please specify a name for the plain file (there is none embedded in the stego file).") ;
		}

		plnfile->filename = s_malloc (strlen (plnfilename) + 1) ;
		strcpy (plnfile->filename, plnfilename) ;
	}

#if 0
	nbytes_plnfilename = bufgetbyte (plnfile->plnbuflhead, pos++) ;
	if (nbytes_plnfilename == 0) {
		/* no file name embedded in stego file */
		if (args.plnfn.value == NULL) {
			/* no -pf argument was given on command line */
			exit_err ("please specify a name for the plain file (there is none embedded in the stego file).") ;
		}
		else {
			if (strcmp (args.plnfn.value, "-") == 0) {
				/* a -pf - argument was given on command line */
				plnfile->filename = NULL ;
			}
			else {
				/* a name for the plain file has been specified on the command line */
				plnfile->filename = s_malloc (strlen (args.plnfn.value) + 1) ;
				strcpy (plnfile->filename, args.plnfn.value) ;

				sthdr.plnfilename = s_malloc (strlen (args.plnfn.value) + 1) ;
				strcpy (sthdr.plnfilename, args.plnfn.value) ;
			}
		}
	}
	else {
		for (i = 0 ; i < nbytes_plnfilename ; i++) {
			plnfilename[i] = (char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
		}
		plnfilename[i] = '\0' ;

		if (args.plnfn.value == NULL) {
			/* no -pf argument was given on command line */
			plnfile->filename = s_malloc (strlen (plnfilename) + 1) ;
			strcpy (plnfile->filename, plnfilename) ;

			sthdr.plnfilename = s_malloc (i + 1) ;
			strcpy (sthdr.plnfilename, plnfilename) ;
		}
		else {
			if (strcmp (args.plnfn.value, "-") == 0) {
				/* a -pf - argument was given on command line */
				plnfile->filename = NULL ;
			}
			else {
				/* a name for the plain file has been specified on the command line */
				plnfile->filename = s_malloc (strlen (args.plnfn.value) + 1) ;
				strcpy (plnfile->filename, args.plnfn.value) ;

				sthdr.plnfilename = s_malloc (strlen (args.plnfn.value) + 1) ;
				strcpy (sthdr.plnfilename, args.plnfn.value) ;
			}
		}
	}
#endif

	if (sthdr.checksum == CHECKSUM_CRC32) {
		uc_crc32 = s_malloc (4) ;
		uc_crc32[0] = (unsigned char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
		uc_crc32[1] = (unsigned char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
		uc_crc32[2] = (unsigned char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
		uc_crc32[3] = (unsigned char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
	}

	tmp = bufcut (plnfile->plnbuflhead, pos, buflength (plnfile->plnbuflhead) + 1) ;
	buffree (plnfile->plnbuflhead) ;
	plnfile->plnbuflhead = tmp ;

	if (sthdr.checksum == CHECKSUM_CRC32) {
		if (!checkcrc32 (plnfile, uc_crc32)) {
			pwarn ("crc32 checksum failed! extracted data is probably corrupted.") ;
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
	plnfile->plnbuflhead = NULL ;	/* will be filled later by extractdata() */

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
		buffree (cvrfile->cvrbuflhead) ;
		if (noncvrbufuse[cvrfile->fileformat]) {
			buffree (cvrfile->noncvrbuflhead) ;
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
	if (plnfile->plnbuflhead != NULL) {
		buffree (plnfile->plnbuflhead) ;
	}
	free (plnfile) ;
}

/* reads a cover data file into a CVRFILE structure */
CVRFILE *readcvrfile (char *filename)
{
	CVRFILE *cvrfile = NULL ;

	if (args.action.value == ARGS_ACTION_EMBED) {
		if (filename == NULL) {
			pverbose ("reading cover file from standard input.") ;
		}
		else {
			pverbose ("reading cover file \"%s\".", filename) ;
		}
	}
	else if (args.action.value == ARGS_ACTION_EXTRACT) {
		if (filename == NULL) {
			pverbose ("reading stego file from standard input.") ;
		}
		else {
			pverbose ("reading stego file \"%s\".", filename) ;
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
			exit_err ("could not open the file \"%s\".", filename) ;
		}
		cvrfile->filename = filename ;
	}

	cvrfile->headers = s_malloc (sizeof *cvrfile->headers) ;

	readheaders (cvrfile) ;

	/* read the file (this fills the rest of cvrfile) */
	cvrfile->cvrbuflhead = createbuflist () ;
	if (noncvrbufuse[cvrfile->fileformat]) {
		cvrfile->noncvrbuflhead = createbuflist () ;
	}
	else {
		cvrfile->noncvrbuflhead = NULL ;
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
				/* FIXME - wieso bei std verwendung kein pquestion ? */
				if ((args.cvrfn.value == NULL) || ((args.plnfn.value == NULL) || (strcmp (args.plnfn.value, "-") == 0))) {
					exit_err ("file \"%s\" does already exist.", stgfile->filename) ;
				}
				else {
					if (!pquestion ("file \"%s\" does already exist. overwrite ?", stgfile->filename)) {
						exit_err ("did not write to file \"%s\".", stgfile->filename) ;
					}
				}
			}
		}

		if ((stgfile->stream = fopen (stgfile->filename, "wb")) == NULL) {
			exit_err ("could not create stego file \"%s\".", stgfile->filename) ;
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
			pverbose ("wrote stego file to standard output.") ;
		}
		else {
			pmsg ("wrote stego file to \"%s\".", stgfile->filename) ;
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
		pverbose ("reading plain file from standard input.") ;
	}
	else {
		pverbose ("reading plain file \"%s\".", filename) ;
	}

	plnfile = s_malloc (sizeof *plnfile) ;

	if (filename == NULL) {
		plnfile->stream = stdin ;
		plnfile->filename = NULL ;
	}
	else {
		if ((plnfile->stream = fopen (filename, "rb")) == NULL) {
			exit_err ("could not open file \"%s\".", filename) ;
		}
		plnfile->filename = filename ;
	}

	plnfile->plnbuflhead = createbuflist () ;

	while ((c = getc (plnfile->stream)) != EOF) {
		bufsetbyte (plnfile->plnbuflhead, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			exit_err ("an error occured while reading the plain data from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the file \"%s\".", filename) ;
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
		pverbose ("writing plain file to standard output.") ;
	}
	else {
		pmsg ("writing plain file to \"%s\".", plnfile->filename) ;
	}

	if (plnfile->filename == NULL) {
		plnfile->stream = stdout ;
	}
	else {
		if (!args.force.value) {
			/* check if file already exists */
			if (fileexists (plnfile->filename)) {
				if (args.stgfn.value == NULL) {
					exit_err ("file \"%s\" does already exist.", plnfile->filename) ;
				}
				else {
					if (!pquestion ("file \"%s\" does already exist. overwrite ?", plnfile->filename)) {
						exit_err ("did not write to file \"%s\".", plnfile->filename) ;
					}
				}
			}
		}

		if ((plnfile->stream = fopen (plnfile->filename, "wb")) == NULL) {
			exit_err ("could not create plain file \"%s\".", plnfile->filename) ;
		}
	}

	while ((c = bufgetbyte (plnfile->plnbuflhead, bufpos)) != ENDOFBUF) {
		putc (c, plnfile->stream) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			exit_err ("an error occured while writing the plain data to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing to the file \"%s\".", plnfile->filename) ;
		}
	}

	return ;
}

