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
#include "cvrstgfile.h"
#include "ff_wav.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

static void wav_calcpos (CVRSTGFILE *file, unsigned long n, unsigned long *bytepos, int *bitpos) ;
static void wav_readheaders (CVRSTGFILE *file) ;
static void wav_readdata (CVRSTGFILE *file) ;
static void wav_writeheaders (CVRSTGFILE *file) ;
static void wav_writedata (CVRSTGFILE *file) ;
static void wav_getchhdr (FILE *file, CHUNKHEADER *chhdr) ;
static void wav_putchhdr (FILE *file, CHUNKHEADER *chhdr) ;

void wav_readfile (CVRSTGFILE *file)
{
	file->contents = s_malloc (sizeof (WAV_CONTENTS)) ;

	wav_readheaders (file) ;
	wav_readdata (file) ;

	return ;
}

void wav_writefile (CVRSTGFILE *file)
{
	wav_writeheaders (file) ;
	wav_writedata (file) ;

	return ;
}

unsigned long wav_capacity (CVRSTGFILE *file)
{
	unsigned int bytespersample = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;
	unsigned long buflen = wav_contents->data->length ;

	if (wav_contents->fmtch.BitsPerSample % 8 == 0) {
		bytespersample = wav_contents->fmtch.BitsPerSample / 8 ;
	}
	else {
		bytespersample = (wav_contents->fmtch.BitsPerSample / 8) + 1 ;
	}

	assert (buflen % bytespersample == 0) ;

	return buflen / bytespersample ;
}

void wav_embedbit (CVRSTGFILE *file, unsigned long pos, int value)
{
	unsigned long bytepos = 0 ;
	int bitpos = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	assert (value == 0 || value == 1) ;

	wav_calcpos (file, pos, &bytepos, &bitpos) ;
	bufsetbit (wav_contents->data, bytepos, bitpos, value) ;

	return ;
}

int wav_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	unsigned long bytepos = 0 ;
	int bitpos = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	wav_calcpos (file, pos, &bytepos, &bitpos) ;
	return bufgetbit (wav_contents->data, bytepos, bitpos) ;
}

void wav_cleanup (CVRSTGFILE *file)
{
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	free (wav_contents->unsupchunks1.data) ;
	buffree (wav_contents->data) ;
	free (wav_contents->unsupchunks2.data) ;
	free (file->contents) ;
}

/* calculate position in buffer for n-th embedded bit */
static void wav_calcpos (CVRSTGFILE *file, unsigned long n, unsigned long *bytepos, int *bitpos)
{
	unsigned int bytespersample = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	if (wav_contents->fmtch.BitsPerSample % 8 == 0) {
		bytespersample = wav_contents->fmtch.BitsPerSample / 8 ;
	}
	else {
		bytespersample = (wav_contents->fmtch.BitsPerSample / 8) + 1 ;
	}

	*bytepos = n * bytespersample ;
	*bitpos = wav_contents->fmtch.BitsPerSample % 8 ;

	return ;
}

/* reads a wav file from disk into a CVRFILE structure */
static void wav_readdata (CVRSTGFILE *file)
{
	int c = EOF ;
	unsigned long pos = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	wav_contents->data = bufcreate (0) ; /* FIXME - set length smarter */

	while ((c = getc (file->stream)) != EOF) {
		bufsetbyte (wav_contents->data, pos++, c) ;
	}

	wav_contents->unsupchunks2.data = NULL ;
	wav_contents->unsupchunks2.len = 0 ;
	if (getc (file->stream) != EOF) {
		unsigned char *ptrunsupdata2 ;
		int c = EOF ;

		/* FIXME - geht das bei stdin ?? */
		fseek (file->stream, -1, SEEK_CUR) ;

		while ((c = getc (file->stream)) != EOF) {
			wav_contents->unsupchunks2.len++ ;
			wav_contents->unsupchunks2.data = s_realloc (wav_contents->unsupchunks2.data, wav_contents->unsupchunks2.len) ;
			ptrunsupdata2 = wav_contents->unsupchunks2.data ;
			ptrunsupdata2[wav_contents->unsupchunks2.len - 1] = c ;
		}			
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the audio data from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the audio data of the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

/* writes a wav file from a CVRFILE structure to disk */
static void wav_writedata (CVRSTGFILE *file)
{
	int i = 0, c = EOF ;
	unsigned long pos = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	while ((c = bufgetbyte (wav_contents->data, pos++)) != ENDOFBUF) {
		putc (c, file->stream) ;
	}

	if (wav_contents->unsupchunks2.len > 0) {
		unsigned char *ptrunsupdata2 = wav_contents->unsupchunks2.data ;
		for (i = 0; i < wav_contents->unsupchunks2.len; i++) {
			putc ((int) ptrunsupdata2[i], file->stream) ;
		}
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

/* reads the headers of a wav file from disk */
static void wav_readheaders (CVRSTGFILE *file)
{
	CHUNKHEADER tmpchhdr = { { '\0', '\0', '\0', '\0' }, 0 } ;
	int i = 0;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	wav_getchhdr (file->stream, &wav_contents->riffchhdr) ;
	wav_contents->id_wave[0] = getc (file->stream) ;
	wav_contents->id_wave[1] = getc (file->stream) ;
	wav_contents->id_wave[2] = getc (file->stream) ;
	wav_contents->id_wave[3] = getc (file->stream) ;

	/* FIXME - ev. hier noch nach "fmt " checken */
	wav_getchhdr (file->stream, &wav_contents->fmtchhdr) ;
	if ((wav_contents->fmtch.FormatTag = read16_le (file->stream)) != WAV_FORMAT_PCM) {
		if (file->filename == NULL) {
			exit_err (_("the wav file from standard input has a format that is not supported.")) ;
		}
		else {
			exit_err (_("the wav file \"%s\" has a format that is not supported."), file->filename) ;
		}
	}
	wav_contents->fmtch.Channels = read16_le (file->stream) ;
	wav_contents->fmtch.SamplesPerSec = read32_le (file->stream) ;
	wav_contents->fmtch.AvgBytesPerSec = read32_le (file->stream) ;
	wav_contents->fmtch.BlockAlign = read16_le (file->stream) ;
	/* if a number other than a multiple of 8 is used, we cannot hide data,
	   because the least significant bits are always set to zero - FIXME */
	if ((wav_contents->fmtch.BitsPerSample = read16_le (file->stream)) % 8 != 0) {
		if (file->filename == NULL) {
			exit_err (_("the bits/sample rate of the wav file from standard input is not a multiple of eight.")) ;
		}
		else {
			exit_err (_("the bits/sample rate of the wav file \"%s\" is not a multiple of eight."), file->filename) ;
		}
	}

	wav_contents->unsupchunks1.data = NULL ;
	wav_contents->unsupchunks1.len = 0 ;
	wav_getchhdr (file->stream, &tmpchhdr) ;
	while (strncmp (tmpchhdr.id, "data", 4) != 0) {
		unsigned char *ptrunsupdata1 ;

		wav_contents->unsupchunks1.data = s_realloc (wav_contents->unsupchunks1.data, (wav_contents->unsupchunks1.len + WAV_SIZE_CHHDR + tmpchhdr.len)) ;

		ptrunsupdata1 = wav_contents->unsupchunks1.data ;
		for (i = 0; i < 4; i++) {
			ptrunsupdata1[wav_contents->unsupchunks1.len++] = (unsigned char) tmpchhdr.id[i] ;
		}
		cp32ul2uc_le (&ptrunsupdata1[wav_contents->unsupchunks1.len], tmpchhdr.len) ;
		wav_contents->unsupchunks1.len += 4 ;

		for (i = 0; i < tmpchhdr.len; i++) {
			ptrunsupdata1[wav_contents->unsupchunks1.len++] = (unsigned char) getc (file->stream) ;
		}

		wav_getchhdr (file->stream, &tmpchhdr) ;
	}

	strncpy (wav_contents->datachhdr.id, tmpchhdr.id, 4) ;
	wav_contents->datachhdr.len = tmpchhdr.len ;

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the headers of the wav file from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the headers of the wav file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

/* writes the headers of a wav file to disk */
static void wav_writeheaders (CVRSTGFILE *file)
{
	int i = 0 ;
	WAV_CONTENTS *wav_contents = ((WAV_CONTENTS *) file->contents) ;

	wav_putchhdr (file->stream, &wav_contents->riffchhdr) ;
	for (i = 0 ; i < 4 ; i++) {
		putc ((int) wav_contents->id_wave[i], file->stream) ;
	}

	wav_putchhdr (file->stream, &wav_contents->fmtchhdr) ;
	write16_le (file->stream, wav_contents->fmtch.FormatTag) ;
	write16_le (file->stream, wav_contents->fmtch.Channels) ;
	write32_le (file->stream, wav_contents->fmtch.SamplesPerSec) ;
	write32_le (file->stream, wav_contents->fmtch.AvgBytesPerSec) ;
	write16_le (file->stream, wav_contents->fmtch.BlockAlign) ;
	write16_le (file->stream, wav_contents->fmtch.BitsPerSample) ;

	if (wav_contents->unsupchunks1.len > 0) {
		unsigned char *ptrunsupdata1 = wav_contents->unsupchunks1.data ;
		for (i = 0; i < wav_contents->unsupchunks1.len; i++)
			putc ((int) ptrunsupdata1[i], file->stream) ;
	}

	wav_putchhdr (file->stream, &wav_contents->datachhdr) ;

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while writing the wav headers to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing the wav headers to the file \"%s\"."), file->filename) ;
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
