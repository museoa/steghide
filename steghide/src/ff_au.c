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
#include "support.h"
#include "msg.h"

static void au_readheaders (CVRFILE *file) ;
static void au_writeheaders (CVRFILE *file) ;
static void au_readdata (CVRFILE *file) ;
static void au_writedata (CVRFILE *file) ;

/* TODO for au file format:
   wieso wird audio enconding nicht verwendet - muss nicht immer 8 bit sein !! */
void au_readfile (CVRFILE *file)
{
	file->contents = s_malloc (sizeof AU_CONTENTS) ;

	au_readheader (file) ;
	au_readdata (file) ;

	return ;
}

void au_writefile (CVRFILE *file)
{
	au_writeheader (file) ;
	au_writedata (file) ;

	return ;
}

unsigned long au_capacity (CVRSTGFILE *file)
{
	return file->contents->data.length ;
}

void au_embedbit (CVRSTGFILE *file, unsigned long pos, int value)
{
	bufsetbit (file->contents->data, pos, 0, value) ;
	return ;
}

int au_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	return bufgetbit (file->contents->data, pos, 0) ;
}

void au_cleanup (CVRSTGFILE *file)
{
	free (file->contents->header) ;
	free (file->contents->infofield) ;
	buffree (file->contents->data) ;
}

static void au_readheaders (CVRFILE *file)
{
	file->contents->header = s_malloc (sizeof *file->contents->header) ;

	file->contents->header->id[0] = getc (file->stream) ;
	file->contents->header->id[1] = getc (file->stream) ;
	file->contents->header->id[2] = getc (file->stream) ;
	file->contents->header->id[3] = getc (file->stream) ;
	assert (strncmp (file->contents->header->id, ".snd", 4) == 0) ;

	file->contents->header->offset = read32_be (file->stream) ;
	file->contents->header->size = read32_be (file->stream) ;
	file->contents->header->encoding = read32_be (file->stream) ;
	file->contents->header->samplerate = read32_be (file->stream) ;
	file->contents->header->channels = read32_be (file->stream) ;

	if ((file->contents->len_infofield = (file->contents->header->offset - AU_SIZE_HEADER)) != 0) {
		unsigned char *ptr = NULL ;
		int i = 0 ;

		file->infofield = s_malloc (file->len_infofield) ;

		ptr = file->infofield ;
		for (i = 0 ; i < file->len_infofield ; i++) {
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

static void au_writeheaders (CVRFILE *file)
{
	putc (file->contents->header->id[0], file->stream) ;
	putc (file->contents->header->id[1], file->stream) ;
	putc (file->contents->header->id[2], file->stream) ;
	putc (file->contents->header->id[3], file->stream) ;
	write32_be (file->stream, file->contents->header->offset) ;
	write32_be (file->stream, file->contents->header->size) ;
	write32_be (file->stream, file->contents->header->encoding) ;
	write32_be (file->stream, file->contents->header->samplerate) ;
	write32_be (file->stream, file->contents->header->channels) ;

	if (file->len_infofield != 0) {
		unsigned char *ptr = file->infofield ;
		int i = 0 ;

		for (i = 0 ; i < file->len_infofield ; i++) {
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

static void au_readdata (CVRFILE *file)
{
	int c = EOF ;
	unsigned long bufpos = 0 ;

	/* if available, use size of audio data (in bytes) to create buffer */
	/* FIXME - wie oft 0xFFFFFFFF ?? */
	if (file->contents->header->size == 0xFFFFFFFF) {
		file->contents->data = bufcreate (0) ;
	}
	else {
		file->contents->data = bufcreate (file->contents->header->size) ; /* FIXME - test this */
	}

	while ((c = getc (file->stream)) != EOF) {
		bufsetbyte (file->cvrdata, bufpos, c) ;
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

static void au_writedata (CVRFILE *file)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;

	while ((c = bufgetbyte (file->cvrdata, bufpos)) != ENDOFBUF) {
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
