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
#include "support.h"
#include "msg.h"

static void wav_getchhdr (FILE *file, CHUNKHEADER *chhdr) ;
static void wav_putchhdr (FILE *file, CHUNKHEADER *chhdr) ;

/* reads the headers of a wav file from disk */
void wav_readheaders (CVRFILE *file, unsigned long rifflen)
{
	CHUNKHEADER tmpchhdr = { { '\0', '\0', '\0', '\0' }, 0 } ;
	int i = 0;

	strcpy (file->headers->wav.riffchhdr.id, "RIFF") ;
	file->headers->wav.riffchhdr.len = rifflen ;
	strcpy (file->headers->wav.id_wave, "WAVE") ;

	wav_getchhdr (file->stream, &file->headers->wav.fmtchhdr) ;
	if ((file->headers->wav.fmtch.FormatTag = read16_le (file->stream)) != WAV_FORMAT_PCM) {
		if (file->filename == NULL) {
			exit_err ("the wav file from standard input has a format that is not supported.") ;
		}
		else {
			exit_err ("the wav file \"%s\" has a format that is not supported.", file->filename) ;
		}
	}
	file->headers->wav.fmtch.Channels = read16_le (file->stream) ;
	file->headers->wav.fmtch.SamplesPerSec = read32_le (file->stream) ;
	file->headers->wav.fmtch.AvgBytesPerSec = read32_le (file->stream) ;
	file->headers->wav.fmtch.BlockAlign = read16_le (file->stream) ;
	/* if a number other than a multiple of 8 is used, we cannot hide data,
	   because the least significant bits are always set to zero */
	if ((file->headers->wav.fmtch.BitsPerSample = read16_le (file->stream)) % 8 != 0) {
		if (file->filename == NULL) {
			exit_err ("the bits/sample rate of the wav file from standard input is not a multiple of eight.") ;
		}
		else {
			exit_err ("the bits/sample rate of the wav file \"%s\" is not a multiple of eight.", file->filename) ;
		}
	}

	file->unsupdata1 = NULL ;
	file->unsupdata1len = 0 ;
	wav_getchhdr (file->stream, &tmpchhdr) ;
	while (strncmp (tmpchhdr.id, "data", 4) != 0) {
		unsigned char *ptrunsupdata1 ;

		file->unsupdata1 = s_realloc (file->unsupdata1, (file->unsupdata1len + 8 + tmpchhdr.len)) ;

		ptrunsupdata1 = file->unsupdata1 ;
		for (i = 0; i < 4; i++) {
			ptrunsupdata1[file->unsupdata1len++] = (unsigned char) tmpchhdr.id[i] ;
		}
		cp32ul2uc_le (&ptrunsupdata1[file->unsupdata1len], tmpchhdr.len) ;
		file->unsupdata1len += 4 ;

		for (i = 0; i < tmpchhdr.len; i++) {
			ptrunsupdata1[file->unsupdata1len++] = (unsigned char) getc (file->stream) ;
		}

		wav_getchhdr (file->stream, &tmpchhdr) ;
	}

	strncpy (file->headers->wav.datachhdr.id, tmpchhdr.id, 4) ;
	file->headers->wav.datachhdr.len = tmpchhdr.len ;

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the headers of the wav file from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the headers of the wav file \"%s\".", file->filename) ;
		}
	}

	return ;
}

/* writes the headers of a wav file to disk */
void wav_writeheaders (CVRFILE *file)
{
	int i = 0 ;

	wav_putchhdr (file->stream, &file->headers->wav.riffchhdr) ;
	for (i = 0 ; i < 4 ; i++) {
		putc ((int) file->headers->wav.id_wave[i], file->stream) ;
	}

	wav_putchhdr (file->stream, &file->headers->wav.fmtchhdr) ;
	write16_le (file->stream, file->headers->wav.fmtch.FormatTag) ;
	write16_le (file->stream, file->headers->wav.fmtch.Channels) ;
	write32_le (file->stream, file->headers->wav.fmtch.SamplesPerSec) ;
	write32_le (file->stream, file->headers->wav.fmtch.AvgBytesPerSec) ;
	write16_le (file->stream, file->headers->wav.fmtch.BlockAlign) ;
	write16_le (file->stream, file->headers->wav.fmtch.BitsPerSample) ;

	if (file->unsupdata1len > 0) {
		unsigned char *ptrunsupdata1 = file->unsupdata1 ;
		for (i = 0; i < file->unsupdata1len; i++)
			putc ((int) ptrunsupdata1[i], file->stream) ;
	}

	wav_putchhdr (file->stream, &file->headers->wav.datachhdr) ;

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while writing the wav headers to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing the wav headers to the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

/* reads a wav file from disk into a CVRFILE structure */
void wav_readfile (CVRFILE *file)
{
	int noncvrbytes, i ;
	unsigned long cvrpos = 0, noncvrpos = 0 ;

	noncvrbytes = (file->headers->wav.fmtch.BitsPerSample / 8) - 1 ;

	while ((cvrpos + noncvrpos) < file->headers->wav.datachhdr.len) {
		bufsetbyte (file->cvrbuflhead, cvrpos, getc (file->stream)) ;
		cvrpos++ ;

		for (i = 0; i < noncvrbytes; i++) {
			bufsetbyte (file->noncvrbuflhead, noncvrpos, getc (file->stream)) ;
			noncvrpos++ ;
		}
	}

	file->unsupdata2 = NULL ;
	file->unsupdata2len = 0 ;
	if (getc (file->stream) != EOF) {
		unsigned char *ptrunsupdata2 ;
		int c = EOF ;

		fseek (file->stream, -1, SEEK_CUR) ;

		while ((c = getc (file->stream)) != EOF) {
			file->unsupdata2len++ ;
			file->unsupdata2 = s_realloc (file->unsupdata2, file->unsupdata2len) ;
			ptrunsupdata2 = file->unsupdata2 ;
			ptrunsupdata2[file->unsupdata2len - 1] = c ;
		}			
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the audio data from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the audio data of the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

/* writes a wav file from a CVRFILE structure to disk */
void wav_writefile (CVRFILE *file)
{
	int noncvrbytes = 0, i = 0 ;
	unsigned long cvrpos = 0, noncvrpos = 0 ;

	wav_writeheaders (file) ;

	noncvrbytes = (file->headers->wav.fmtch.BitsPerSample / 8) - 1 ;

	while ((cvrpos + noncvrpos) < file->headers->wav.datachhdr.len) {
		putc (bufgetbyte (file->cvrbuflhead, cvrpos), file->stream) ;
		cvrpos++ ;

		for (i = 0; i < noncvrbytes; i++) {
			putc (bufgetbyte (file->noncvrbuflhead, noncvrpos), file->stream) ;
			noncvrpos++ ;
		}
	}

	if (file->unsupdata2len > 0) {
		unsigned char *ptrunsupdata2 = file->unsupdata2 ;
		for (i = 0; i < file->unsupdata2len; i++) {
			putc ((int) ptrunsupdata2[i], file->stream) ;
		}
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

/* reads a wav chunk header from file and puts it indo chhdr */
static void wav_getchhdr (FILE *file, CHUNKHEADER *chhdr)
{
	chhdr->id[0] = getc (file) ;
	chhdr->id[1] = getc (file) ;
	chhdr->id[2] = getc (file) ;
	chhdr->id[3] = getc (file) ;
	chhdr->len = read32_le (file) ;
}

/* writes a wav chunk header from chhdr to file */
static void wav_putchhdr (FILE *file, CHUNKHEADER *chhdr)
{
	putc (chhdr->id[0], file) ;
	putc (chhdr->id[1], file) ;
	putc (chhdr->id[2], file) ;
	putc (chhdr->id[3], file) ;
	write32_le (file, chhdr->len) ;
}
