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

#include "bufmanag.h"
#include "ff_au.h"
#include "main.h"
#include "cvrstgfile.h"
#include "support.h"
#include "msg.h"

static void au_readheaders (CVRSTGFILE *file) ;
static void au_writeheaders (CVRSTGFILE *file) ;
static void au_readdata (CVRSTGFILE *file) ;
static void au_writedata (CVRSTGFILE *file) ;

/* TODO for au file format:
   wieso wird audio enconding nicht verwendet - muss nicht immer 8 bit sein !! */
void au_readfile (CVRSTGFILE *file)
{
	file->contents = s_malloc (sizeof (AU_CONTENTS)) ;

	au_readheaders (file) ;
	au_readdata (file) ;

	return ;
}

void au_writefile (CVRSTGFILE *file)
{
	au_writeheaders (file) ;
	au_writedata (file) ;

	return ;
}

unsigned long au_capacity (CVRSTGFILE *file)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	return au_contents->data->length ;
}

void au_embedbit (CVRSTGFILE *file, unsigned long pos, int value)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	assert (value == 0 || value == 1) ;
	bufsetbit (au_contents->data, pos, 0, value) ;

	return ;
}

int au_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	return bufgetbit (au_contents->data, pos, 0) ;
}

void au_cleanup (CVRSTGFILE *file)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	free (au_contents->header) ;
	free (au_contents->infofield) ;
	buffree (au_contents->data) ;
	free (file->contents) ;
}

static void au_readheaders (CVRSTGFILE *file)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	au_contents->header = s_malloc (sizeof *au_contents->header) ;

	au_contents->header->id[0] = getc (file->stream) ;
	au_contents->header->id[1] = getc (file->stream) ;
	au_contents->header->id[2] = getc (file->stream) ;
	au_contents->header->id[3] = getc (file->stream) ;
	assert (strncmp (au_contents->header->id, ".snd", 4) == 0) ;

	au_contents->header->offset = read32_be (file->stream) ;
	au_contents->header->size = read32_be (file->stream) ;
	au_contents->header->encoding = read32_be (file->stream) ;
	au_contents->header->samplerate = read32_be (file->stream) ;
	au_contents->header->channels = read32_be (file->stream) ;

	if ((au_contents->len_infofield = (au_contents->header->offset - AU_SIZE_HEADER)) != 0) {
		unsigned char *ptr = NULL ;
		int i = 0 ;

		au_contents->infofield = s_malloc (au_contents->len_infofield) ;

		ptr = au_contents->infofield ;
		for (i = 0 ; i < au_contents->len_infofield ; i++) {
			ptr[i] = (unsigned char) getc (file->stream) ;
		}
	}
		
	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the au headers from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the au headers from the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

static void au_writeheaders (CVRSTGFILE *file)
{
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	putc (au_contents->header->id[0], file->stream) ;
	putc (au_contents->header->id[1], file->stream) ;
	putc (au_contents->header->id[2], file->stream) ;
	putc (au_contents->header->id[3], file->stream) ;
	write32_be (file->stream, au_contents->header->offset) ;
	write32_be (file->stream, au_contents->header->size) ;
	write32_be (file->stream, au_contents->header->encoding) ;
	write32_be (file->stream, au_contents->header->samplerate) ;
	write32_be (file->stream, au_contents->header->channels) ;

	if (au_contents->len_infofield != 0) {
		unsigned char *ptr = au_contents->infofield ;
		int i = 0 ;

		for (i = 0 ; i < au_contents->len_infofield ; i++) {
			putc ((int) ptr[i], file->stream) ;
		}
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while writing the au headers to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing the au headers to the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

static void au_readdata (CVRSTGFILE *file)
{
	int c = EOF ;
	unsigned long bufpos = 0 ;
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	/* if available, use size of audio data (in bytes) to create buffer */
	/* FIXME - wie oft 0xFFFFFFFF ?? */
	if (au_contents->header->size == 0xFFFFFFFF) {
		au_contents->data = bufcreate (0) ;
	}
	else {
		au_contents->data = bufcreate (au_contents->header->size) ; /* FIXME - test this */
	}

	while ((c = getc (file->stream)) != EOF) {
		bufsetbyte (au_contents->data, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the audio data from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the audio data from the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

static void au_writedata (CVRSTGFILE *file)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;
	AU_CONTENTS *au_contents = ((AU_CONTENTS *) file->contents) ;

	while ((c = bufgetbyte (au_contents->data, bufpos)) != ENDOFBUF) {
		putc (c, file->stream) ;
		bufpos++ ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while writing the audio data to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing the audio data to the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}
