/*
 * steghide 0.4.1 - a steganography program
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
				if (args_action == ACTN_EMBED) {
					perr (ERR_CVRUNKNOWNFFORMAT) ;
				}
				else if (args_action == ACTN_EXTRACT) {
					perr (ERR_STGUNKNOWNFFORMAT) ;
				}
			}
		}
		else {
			if (args_action == ACTN_EMBED)
				perr (ERR_CVRUNKNOWNFFORMAT) ;
			else if (args_action == ACTN_EXTRACT)
				perr (ERR_STGUNKNOWNFFORMAT) ;
		}
	}

	return ;
}

/* creates a CVRFILE structure that is used to save the stego data to disk */
CVRFILE *createstgfile (CVRFILE *cvrfile, const char *stgfilename)
{
	CVRFILE *stgfile = NULL ;

	if ((stgfile = malloc (sizeof *stgfile)) == NULL) {
		perr (ERR_MEMALLOC) ;
	}
	
	if (strcmp (stgfilename, "-") == 0) {
		stgfile->stream = stdout ;
	}
	else {
		if ((stgfile->stream = fopen (stgfilename, "wb")) == NULL) {
			if (args_action == ACTN_EMBED) {
				perr (ERR_STGOPEN) ;
			}
			else {
				assert (0) ;
			}
		}
	}
	
	if ((stgfile->filename = malloc (strlen (stgfilename) + 1)) == NULL) {
		perr (ERR_MEMALLOC) ;
	}
	strcpy (stgfile->filename, stgfilename) ;

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
	
	if (sthdr.plnfilename == NULL) {
		/* standard input is used */
		bufsetbyte (buf, pos++, 0) ;
	}
	else {
		tmp = stripdir (sthdr.plnfilename) ;
		free (sthdr.plnfilename) ;
		sthdr.plnfilename = tmp ;

		if ((nbytes_plnfilename = strlen (sthdr.plnfilename)) > PLNFILENAME_MAXLEN) {
			perr (ERR_PLNFILENAMELEN) ;
		}
		bufsetbyte (buf, pos++, nbytes_plnfilename) ;
		
		for (i = 0 ; i < nbytes_plnfilename ; i++) {
			bufsetbyte (buf, pos++, sthdr.plnfilename[i]) ;
		}
	}

	tmpbuf = bufcat (buf, plnfile->plnbuflhead) ;
	buffree (plnfile->plnbuflhead) ;
	buffree (buf) ;
	plnfile->plnbuflhead = tmpbuf ;
}

void deassemble_plndata (PLNFILE *plnfile)
{
	char plnfilename[PLNFILENAME_MAXLEN + 1] ;
	unsigned long pos = 0 ;
	int nbytes_plnfilename = 0 ;
	BUFFER *tmp = NULL ;
	int i = 0 ;

	nbytes_plnfilename = bufgetbyte (plnfile->plnbuflhead, pos++) ;
	if (nbytes_plnfilename == 0) {
		/* standard input was used for plnfile during embedding */
		if (args_fn_pln == NULL) {
			/* no -pf argument was given on command line */
			perr (ERR_NOPLNFILENAME) ;
		}
		else {
			if (strcmp (args_fn_pln, "-") == 0) {
				/* a -pf - argument was given on command line */
				plnfile->stream = stdout ;
				plnfile->filename = NULL ;
			}
			else {
				/* a name for the plain file has been specified on the command line */
				if ((plnfile->stream = fopen (args_fn_pln, "wb")) == NULL) {
					perr (ERR_PLNOPEN) ;
				}

				if ((plnfile->filename = malloc (strlen (args_fn_pln) + 1)) == NULL) {
					perr (ERR_MEMALLOC) ;
				}
				strcpy (plnfile->filename, args_fn_pln) ;

				if ((sthdr.plnfilename = malloc (strlen (args_fn_pln) + 1)) == NULL) {
					perr (ERR_MEMALLOC) ;
				}
				strcpy (sthdr.plnfilename, args_fn_pln) ;
			}
		}
	}
	else {
		for (i = 0 ; i < nbytes_plnfilename ; i++) {
			plnfilename[i] = (char) bufgetbyte (plnfile->plnbuflhead, pos++) ;
		}
		plnfilename[i] = '\0' ;

		if (args_fn_pln == NULL) {
			/* no -pf argument was given on command line */
			if ((plnfile->filename = malloc (strlen (plnfilename) + 1)) == NULL) {
				perr (ERR_MEMALLOC) ;
			}
			strcpy (plnfile->filename, plnfilename) ;

			if ((plnfile->stream = fopen (plnfilename, "wb")) == NULL) {
				perr (ERR_PLNOPEN) ;
			}

			if ((sthdr.plnfilename = malloc (i + 1)) == NULL) {
				perr (ERR_MEMALLOC) ;
			}
			strcpy (sthdr.plnfilename, plnfilename) ;
		}
		else {
			if (strcmp (args_fn_pln, "-") == 0) {
				/* a -pf - argument was given on command line */
				plnfile->stream = stdout ;
				plnfile->filename = NULL ;
			}
			else {
				/* a name for the plain file has been specified on the command line */
				if ((plnfile->stream = fopen (args_fn_pln, "wb")) == NULL) {
					perr (ERR_PLNOPEN) ;
				}

				if ((plnfile->filename = malloc (strlen (args_fn_pln) + 1)) == NULL) {
					perr (ERR_MEMALLOC) ;
				}
				strcpy (plnfile->filename, args_fn_pln) ;

				if ((sthdr.plnfilename = malloc (strlen (args_fn_pln) + 1)) == NULL) {
					perr (ERR_MEMALLOC) ;
				}
				strcpy (sthdr.plnfilename, args_fn_pln) ;
			}
		}
	}

	tmp = bufcut (plnfile->plnbuflhead, pos, buflength (plnfile->plnbuflhead) + 1) ;
	buffree (plnfile->plnbuflhead) ;
	plnfile->plnbuflhead = tmp ;
}

/* creates a PLNFILE structure that is used to save the plaindata to disk */
PLNFILE *createplnfile (void)
{
	PLNFILE *plnfile = NULL ;

	if ((plnfile = malloc (sizeof *plnfile)) == NULL)
		perr (ERR_MEMALLOC) ;

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
CVRFILE *readcvrfile (const char *filename)
{
	CVRFILE *cvrfile = NULL ;

	/* fill cvrfile structure */
	if ((cvrfile = malloc (sizeof *cvrfile)) == NULL)
		perr (ERR_MEMALLOC) ;

	if (strcmp (filename, "-") == 0)
		cvrfile->stream = stdin ;
	else {
		if ((cvrfile->stream = fopen (filename, "rb")) == NULL) {
			free (cvrfile) ;
			if (args_action == ACTN_EMBED) {
				perr (ERR_CVROPEN) ;
			}
			else if (args_action == ACTN_EXTRACT) {
				perr (ERR_STGOPEN) ;
			}
		}
	}

	if ((cvrfile->filename = malloc (strlen (filename) + 1)) == NULL)
		perr (ERR_MEMALLOC) ;
	strcpy (cvrfile->filename, filename) ;

	if ((cvrfile->headers = malloc (sizeof *cvrfile->headers)) == NULL)
		perr (ERR_MEMALLOC) ;

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
void writecvrfile (CVRFILE *cvrfile)
{
	switch (cvrfile->fileformat) {
		case FF_BMP:
		bmp_writefile (cvrfile) ;
		break ;

		case FF_WAV:
		wav_writefile (cvrfile) ;
		break ;

		case FF_AU:
		au_writefile (cvrfile) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return ;
}
	
/* reads a plain data file into a PLNFILE structure */
PLNFILE *readplnfile (const char *filename)
{
	PLNFILE *plnfile = NULL ;
	int c = EOF ;
	unsigned long bufpos = 0 ;

	if ((plnfile = malloc (sizeof *plnfile)) == NULL) {
		perr (ERR_MEMALLOC) ;
	}

	if (filename == NULL) {
		plnfile->stream = stdin ;
	}
	else if (strcmp (filename, "-") == 0) {
		plnfile->stream = stdin ;
	}
	else {
		if ((plnfile->stream = fopen (filename, "rb")) == NULL) {
			perr (ERR_PLNOPEN) ;
		}
		if ((plnfile->filename = malloc (strlen (filename) + 1)) == NULL)
			perr (ERR_MEMALLOC) ;
		strcpy (plnfile->filename, filename) ;
	}

	plnfile->plnbuflhead = createbuflist () ;

	while ((c = getc (plnfile->stream)) != EOF) {
		bufsetbyte (plnfile->plnbuflhead, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream))
		perr (ERR_PLNREAD) ;

	assemble_plndata (plnfile) ;

	sthdr.nbytesplain = buflength (plnfile->plnbuflhead) ;

	return plnfile ;
}

/* writes the file described in the plnfile structure to disk */
void writeplnfile (PLNFILE *plnfile)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;

	deassemble_plndata (plnfile) ;

	while ((c = bufgetbyte (plnfile->plnbuflhead, bufpos)) != ENDOFBUF) {
		putc (c, plnfile->stream) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		perr (ERR_PLNWRITE) ;
	}

	return ;
}

