/*
 * steghide 0.4.6b - a steganography program
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "bufmanag.h"
#include "error.h"
#include "hash.h"
#include "main.h"
#include "msg.h"
#include "plnfile.h"
#include "support.h"

/* reads a plain data file into a PLNFILE structure */
PLNFILE *pln_readfile (char *filename)
{
	PLNFILE *plnfile = NULL ;
	int c = EOF ;
	unsigned long bufpos = 0 ;

	if (filename == NULL) {
		VerboseMessage v (_("reading plain file from standard input.")) ;
		v.printMessage() ;
	}
	else {
		VerboseMessage v (_("reading plain file \"%s\"."), filename) ;
		v.printMessage() ;
	}

	plnfile = (PLNFILE *) s_malloc (sizeof *plnfile) ;

	if (filename == NULL) {
		plnfile->stream = stdin ;
		plnfile->filename = NULL ;
	}
	else {
		if ((plnfile->stream = fopen (filename, "rb")) == NULL) {
			throw SteghideError (_("could not open file \"%s\"."), filename) ;
		}
		plnfile->filename = filename ;
	}

	plnfile->plndata = bufcreate (0) ;

	while ((c = getc (plnfile->stream)) != EOF) {
		bufsetbyte (plnfile->plndata, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			throw SteghideError (_("an error occured while reading the plain data from standard input.")) ;
		}
		else {
			throw SteghideError (_("an error occured while reading the file \"%s\"."), filename) ;
		}
	}

	return plnfile ;
}

/* writes the file described in the plnfile structure to disk */
void pln_writefile (PLNFILE *plnfile)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;

	if (plnfile->filename == NULL) {
		VerboseMessage v (_("writing plain file to standard output.")) ;
		v.printMessage() ;
	}
	else {
		Message m (_("writing plain file to \"%s\"."), plnfile->filename) ;
		m.printMessage() ;
	}

	if (plnfile->filename == NULL) {
		plnfile->stream = stdout ;
	}
	else {
		checkforce (plnfile->filename) ;
		if ((plnfile->stream = fopen (plnfile->filename, "wb")) == NULL) {
			throw SteghideError (_("could not create plain file \"%s\"."), plnfile->filename) ;
		}
	}

	while ((c = bufgetbyte (plnfile->plndata, bufpos)) != ENDOFBUF) {
		putc (c, plnfile->stream) ;
		bufpos++ ;
	}

	if (ferror (plnfile->stream)) {
		if (plnfile->filename == NULL) {
			throw SteghideError (_("an error occured while writing the plain data to standard output.")) ;
		}
		else {
			throw SteghideError (_("an error occured while writing to the file \"%s\"."), plnfile->filename) ;
		}
	}

	return ;
}

void assemble_plndata (PLNFILE *plnfile)
{
	BUFFER *buf = NULL, *tmpbuf = NULL ;
	unsigned long pos = 0 ;
	int nbytes_plnfilename = 0 ;
	int i = 0 ;

	buf = bufcreate (1) ;
	
	if ((args->plnfn.getValue() == "") || (!args->embedplnfn.getValue())) {
		/* standard input is used or plain file name embedding has
		   been turned off explicitly */
		bufsetbyte (buf, pos++, 0) ;
	}
	else {
		assert (args->plnfn.is_set()) ;

		char *tmp = (char *) s_malloc (strlen (args->plnfn.getValue().c_str()) + 1) ;
		strcpy (tmp, args->plnfn.getValue().c_str()) ;
		tmp = stripdir (tmp) ;

		if ((nbytes_plnfilename = strlen (tmp)) > PLNFILENAME_MAXLEN) {
			throw SteghideError (_("the maximum length for the plain file name is %d characters."), PLNFILENAME_MAXLEN) ;
		}
		bufsetbyte (buf, pos++, nbytes_plnfilename) ;
		
		for (i = 0 ; i < nbytes_plnfilename ; i++) {
			bufsetbyte (buf, pos++, tmp[i]) ;
		}
	}

	if (args->checksum.getValue()) {
		unsigned char *uc_crc32 = (unsigned char *) getcrc32 (plnfile) ;
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

	if (args->plnfn.is_set()) {
		if (args->plnfn.getValue() == "") {
			/* write pln data to stdout */
			plnfile->filename = NULL ;
		}
		else {
			/* write pln data to file with given name */
			plnfile->filename = (char *) s_malloc (strlen (args->plnfn.getValue().c_str()) + 1) ;
			strcpy (plnfile->filename, args->plnfn.getValue().c_str()) ;
		}
	}
	else {
		/* write pln data to file with name stored in stego file */
		assert (args->plnfn.getValue() == "") ;

		if (nbytes_plnfilename == 0) {
			throw SteghideError (_("please specify a name for the plain file (there is none embedded in the stego file).")) ;
		}

		plnfile->filename = (char *) s_malloc (strlen (plnfilename) + 1) ;
		strcpy (plnfile->filename, plnfilename) ;
	}

	if (sthdr.checksum == CHECKSUM_CRC32) {
		uc_crc32 = (unsigned char *) s_malloc (4) ;
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
			VerboseMessage v (_("crc32 checksum test ok.")) ;
			v.printMessage() ;
		}
		else {
			CriticalWarning w (_("crc32 checksum failed! extracted data is probably corrupted.")) ;
			w.printMessage() ;
		}
	}

	return ;
}

/* creates a PLNFILE structure that is used to save the plaindata to disk */
PLNFILE *pln_createfile (void)
{
	PLNFILE *plnfile = NULL ;

	plnfile = (PLNFILE *) s_malloc (sizeof *plnfile) ;

	plnfile->filename = NULL ;	/* will be filled later by deassemble_plndata() */
	plnfile->stream = NULL ;	/* will be filled later be deassemble_plndata() */
	plnfile->plndata = NULL ;	/* will be filled later with retval from extractdata() */

	return plnfile ;
}


/* clean up a PLNFILE structure (closes file, frees memory) */
void pln_cleanup (PLNFILE *plnfile)
{
	/*
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
	*/
}

