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

#include "main.h"
#include "io.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

void au_readheaders (CVRFILE *file)
{
	strcpy (file->headers->au.id, ".snd") ;
	file->headers->au.offset = read32_be (file->stream) ;
	file->headers->au.size = read32_be (file->stream) ;
	file->headers->au.encoding = read32_be (file->stream) ;
	file->headers->au.samplerate = read32_be (file->stream) ;
	file->headers->au.channels = read32_be (file->stream) ;

	if ((file->unsupdata1len = (file->headers->au.offset - AU_SIZE_HEADER)) != 0) {
		unsigned char *ptrunsupdata1 = NULL ;
		int i = 0 ;

		file->unsupdata1 = s_malloc (file->unsupdata1len) ;

		ptrunsupdata1 = file->unsupdata1 ;
		for (i = 0 ; i < file->unsupdata1len ; i++) {
			ptrunsupdata1[i] = (unsigned char) getc (file->stream) ;
		}
	}
		
	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the au headers from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the au headers from the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

void au_writeheaders (CVRFILE *file)
{
	putc (file->headers->au.id[0], file->stream) ;
	putc (file->headers->au.id[1], file->stream) ;
	putc (file->headers->au.id[2], file->stream) ;
	putc (file->headers->au.id[3], file->stream) ;
	write32_be (file->stream, file->headers->au.offset) ;
	write32_be (file->stream, file->headers->au.size) ;
	write32_be (file->stream, file->headers->au.encoding) ;
	write32_be (file->stream, file->headers->au.samplerate) ;
	write32_be (file->stream, file->headers->au.channels) ;

	if (file->unsupdata1len != 0) {
		unsigned char *ptrunsupdata1 = file->unsupdata1 ;
		int i = 0 ;

		for (i = 0 ; i < file->unsupdata1len ; i++) {
			putc ((int) ptrunsupdata1[i], file->stream) ;
		}
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while writing the au headers to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing the au headers to the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

void au_readfile (CVRFILE *file)
{
	int c = EOF ;
	unsigned long bufpos = 0 ;

	while ((c = getc (file->stream)) != EOF) {
		bufsetbyte (file->cvrdata, bufpos, c) ;
		bufpos++ ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the audio data from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the audio data from the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

void au_writefile (CVRFILE *file)
{
	int c = ENDOFBUF ;
	unsigned long bufpos = 0 ;

	au_writeheaders (file) ;

	while ((c = bufgetbyte (file->cvrdata, bufpos)) != ENDOFBUF) {
		putc (c, file->stream) ;
		bufpos++ ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while writing the audio data to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing the audio data to the file \"%s\".", file->filename) ;
		}
	}

	return ;
}
