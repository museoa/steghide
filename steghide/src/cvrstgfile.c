/*
 * steghide 0.4.5 - a steganography program
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
#include "ff_bmp.h"
#include "ff_wav.h"
#include "ff_au.h"

/* function prototypes */
static int detectff (CVRSTGFILE *file) ;

CVRSTGFILE *cvrstg_readfile (char *filename)
{
	CVRSTGFILE *file = NULL ;

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

	/* fill CVRSTGFILE structure */
	file = s_malloc (sizeof *file) ;

	if (filename == NULL) {
		file->stream = stdin ;
		file->filename = NULL ;
	}
	else {
		if ((file->stream = fopen (filename, "rb")) == NULL) {
			free (file) ;
			exit_err (_("could not open the file \"%s\"."), filename) ;
		}
		file->filename = filename ;
	}

	file->fileformat = detectff (file) ;
	switch (file->fileformat) {
		case FF_UNKNOWN:
		if (file->filename == NULL) {
			exit_err (_("the file format of the data from standard input is not supported.")) ;
		}
		else {
			exit_err (_("the file format of the file \"%s\" is not supported."), file->filename) ;
		}
		break ;

		case FF_BMP:
		bmp_readfile (file) ;
		break ;

		case FF_WAV:
		wav_readfile (file) ;
		break ;

		case FF_AU:
		au_readfile (file) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return cvrfile ;
}

/* writes the file described in the cvrstgfile structure to disk */
void cvrstg_writefile (CVRSTGFILE *file)
{
	if (file->filename == NULL) {
		pverbose (_("writing stego file to standard output.")) ;
	}
	else {
		pverbose (_("writing stego file \"%s\"."), file->filename) ;
	}

	switch (file->fileformat) {
		case FF_BMP:
		bmp_writefile (file) ;
		break ;

		case FF_WAV:
		wav_writefile (file) ;
		break ;

		case FF_AU:
		au_writefile (file) ;
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

/* 'creates' a stego file from a cover file */
void cvrstg_transform (CVRSTGFILE *file, char *stgfilename)
{
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
		if (!args.force.value) {
			/* check if file already exists */
			if (fileexists (file->filename)) {
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

		if ((file->stream = fopen (file->filename, "wb")) == NULL) {
			exit_err (_("could not create stego file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

unsigned long cvrstg_capacity (CVRSTGFILE *file)
{
	unsigned long retval = 0 ;

	switch (file->fileformat) {
		case FF_BMP:
		retval = bmp_capacity (file) ;
		break ;

		case FF_WAV:
		retval = wav_capacity (file) ;
		break ;

		case FF_AU:
		retval = au_capacity (file) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void cvrstg_embedbit (CVRSTGFILE *file, unsigned long pos, int value)
{
	switch (file->fileformat) {
		case FF_BMP:
		bmp_embedbit (file, pos, value) ;
		break ;

		case FF_WAV:
		wav_embedbit (file, pos, value) ;
		break ;

		case FF_AU
		au_embedbit (file, pos, value) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return ;
}

int cvrstg_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	int retval = -1 ;

	switch (file->fileformat) {
		case FF_BMP:
		retval = bmp_extractbit (file, pos) ;
		break ;

		case FF_WAV:
		retval = wav_extractbit (file, pos) ;
		break ;

		case FF_AU:
		retval = au_extractbit (file, pos) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void cvrstg_cleanup (CVRSTGFILE *file)
{
	switch (file->fileformat) {
		case FF_BMP:
		bmp_cleanup (file) ;
		break ;

		case FF_WAV:
		wav_cleanup (file) ;
		break ;

		case FF_AU:
		au_cleanup (file) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	if (file->filename != NULL) {
		if (fclose (file->stream) != 0) {
			exit_err (_("could not close file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

/* auto-detects file format while reading headers */
static void detectff (CVRFILE *file)
{
	char buf[4] = { '\0', '\0', '\0', '\0' } ;
	int i = 0, retval = FF_UNKNOWN ;
	
	/* FIXME - return value von getc überprüfen, danach ein ferror !! */
	/* ? auch in read32_le,... funktionen ? */

	for (i = 0 ; i < 2 ; i++) {
		buf[i] = (char) getc (file->stream) ;
	}

	if (strncmp ("BM", buf, 2) == 0) {
		retval = FF_BMP ;
	}
	else {
		for (i = 2 ; i < 4 ; i++) {
			buf[i] = (char) getc (file->stream) ;
		}

		if (strncmp (".snd", buf, 4) == 0) {
			retval = FF_AU ;
		}
		else if (strncmp ("RIFF", buf, 4) == 0) {
			unsigned long rifflen ;

			rifflen = read32_le (file->stream) ;
			for (i = 0 ; i < 4 ; i++) {
				buf[i] = getc (file->stream) ;
			}
			if (strncmp ("WAVE", buf, 4) == 0) {
				retval = FF_WAV ;
			}
		}
	}

	rewind (file->stream) ;

	return retval ;
}
