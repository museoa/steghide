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
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "main.h"
#include "cvrstgfile.h"
#include "ff_bmp.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

static void bmp_readheaders (CVRSTGFILE *file) ;
static void bmpwin_readheaders (CVRSTGFILE *file) ;
static void bmpos2_readheaders (CVRSTGFILE *file) ;
static void bmp_writeheaders (CVRSTGFILE *file) ;
static void bmpwin_writeheaders (CVRSTGFILE *file) ;
static void bmpos2_writeheaders (CVRSTGFILE *file) ;
static void bmp_readdata (CVRSTGFILE *file) ;
static void bmp_writedata (CVRSTGFILE *file) ;
static void bmp_readerror (CVRSTGFILE *file) ;
static void bmp_writeerror (CVRSTGFILE *file) ;
static void bmp_calcRC (CVRSTGFILE *file, unsigned long pos, unsigned long *row, unsigned long *column) ;
static long bmp_calclinelength (CVRSTGFILE *file) ;
static long bmp_getheight (CVRSTGFILE *file) ;

void bmp_readfile (CVRSTGFILE *file)
{
	file->contents = s_malloc (sizeof (BMP_CONTENTS)) ;

	bmp_readheaders (file) ;
	bmp_readdata (file) ;

	return ;
}

void bmp_writefile (CVRSTGFILE *file)
{
	bmp_writeheaders (file) ;
	bmp_writedata (file) ;

	return ;
}

unsigned long bmp_capacity (CVRSTGFILE *file)
{
	unsigned long linelength = 0, retval = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth % 8 == 0) {
			linelength = bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8 ;
		}
		else {
			linelength = (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8) + 1;
		}
		retval = bmp_contents->bmi.win.bmih.biHeight * linelength ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth % 8 == 0) {
			linelength = bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			linelength = (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8) + 1;
		}
		retval = bmp_contents->bmi.os2.bmch.bcHeight * linelength ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void bmp_embedbit (CVRSTGFILE *file, unsigned long pos, int value)
{
	unsigned long row = 0, column = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	assert (value == 0 || value == 1) ;

	bmp_calcRC (file, pos, &row, &column) ;
	bmp_contents->bitmap[row][column] = (bmp_contents->bitmap[row][column] & ~1) | value ;

	return ;
}

int bmp_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	unsigned long row = 0, column = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	bmp_calcRC (file, pos, &row, &column) ;
	return (bmp_contents->bitmap[row][column] & 1) ;
}

void bmp_cleanup (CVRSTGFILE *file)
{
	int i = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (bmp_contents->bmi.win.ncolors > 0) {
			free (bmp_contents->bmi.win.colors) ;
		}
		for (i = 0 ; i < bmp_contents->bmi.win.bmih.biHeight ; i++) {
			free (bmp_contents->bitmap[i]) ;
		}
		free (bmp_contents->bitmap) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (bmp_contents->bmi.os2.ncolors > 0) {
			free (bmp_contents->bmi.os2.colors) ;
		}
		for (i = 0 ; i < bmp_contents->bmi.os2.bmch.bcHeight ; i++) {
			free (bmp_contents->bitmap[i]) ;
		}
		free (bmp_contents->bitmap) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	free (file->contents) ;

	return ;
}

static void bmp_calcRC (CVRSTGFILE *file, unsigned long pos, unsigned long *row, unsigned long *column)
{
	unsigned long width = 0 /* in bytes */, height = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth % 8 == 0) {
			width = bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8 ;
		}
		else {
			width = (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8) + 1;
		}
		height = bmp_contents->bmi.win.bmih.biHeight ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth % 8 == 0) {
			width = bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			width = (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8) + 1;
		}
		height = bmp_contents->bmi.os2.bmch.bcHeight ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	*row = height - (pos / width) - 1 ;
	*column = pos % width ;

	return ;
}

/* reads the headers of a bmp file from disk */
static void bmp_readheaders (CVRSTGFILE *file)
{
	unsigned long tmpSize = 0 ;
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	bmp_contents->bmfh.bfType = read16_le (file->stream) ;
	bmp_contents->bmfh.bfSize = read32_le (file->stream) ;
	bmp_contents->bmfh.bfReserved1 = read16_le (file->stream) ;
	bmp_contents->bmfh.bfReserved2 = read16_le (file->stream) ;
	bmp_contents->bmfh.bfOffBits = read32_le (file->stream) ;
	
	tmpSize = read32_le (file->stream) ;
	switch (tmpSize) {
		case BMP_SIZE_BMINFOHEADER:	/* file has windows bmp format */
		bmpwin_readheaders (file) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:	/* file has OS/2 bmp format */
		bmpos2_readheaders (file) ;
		break ;

		default:
		if (file->filename == NULL) {
			exit_err (_("the bmp data from standard input has a format that is not supported.")) ;
		}
		else {
			exit_err (_("the bmp file \"%s\" has a format that is not supported."), file->filename) ;
		}
		break ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the bmp headers from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the bmp headers of the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

static void bmpwin_readheaders (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	bmp_contents->bmi.win.bmih.biSize = BMP_SIZE_BMINFOHEADER ;
	bmp_contents->bmi.win.bmih.biWidth = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biHeight = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biPlanes = read16_le (file->stream) ;
	assert (bmp_contents->bmi.win.bmih.biPlanes == 1) ;
	bmp_contents->bmi.win.bmih.biBitCount = read16_le (file->stream) ;
	assert ((bmp_contents->bmi.win.bmih.biBitCount == 1) ||
			(bmp_contents->bmi.win.bmih.biBitCount == 4) ||
			(bmp_contents->bmi.win.bmih.biBitCount == 8) || 
			(bmp_contents->bmi.win.bmih.biBitCount == 24)) ;
	bmp_contents->bmi.win.bmih.biCompression = read32_le (file->stream) ;
	if (bmp_contents->bmi.win.bmih.biCompression != BMP_BI_RGB) {
		if (file->filename == NULL) {
			exit_err (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			exit_err (_("the bitmap data in \"%s\" is compressed which is not supported."), file->filename) ;
		}
	}
	bmp_contents->bmi.win.bmih.biSizeImage = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biXPelsPerMeter = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biYPelsPerMeter = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biClrUsed = read32_le (file->stream) ;
	bmp_contents->bmi.win.bmih.biClrImportant = read32_le (file->stream) ;

	if (bmp_contents->bmi.win.bmih.biBitCount == 24) {
		bmp_contents->bmi.win.colors = NULL ;
		bmp_contents->bmi.win.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		int i = 0 ;

		switch (bmp_contents->bmi.win.bmih.biBitCount) {
			case 1:
			if (args.action.value == ARGS_ACTION_EMBED) {
				pwarn (_("using a black/white bitmap as cover is very insecure!")) ;
			}
			bmp_contents->bmi.win.ncolors = 2 ;
			break ;

			case 4:
			if (args.action.value == ARGS_ACTION_EMBED) {
				pwarn (_("using a 16-color bitmap as cover is very insecure!")) ;
			}
			bmp_contents->bmi.win.ncolors = 16 ;
			break ;

			case 8:
			bmp_contents->bmi.win.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}
		if (bmp_contents->bmi.win.bmih.biClrUsed != 0) {
			bmp_contents->bmi.win.ncolors = bmp_contents->bmi.win.bmih.biClrUsed ;
		}

		bmp_contents->bmi.win.colors = s_malloc ((sizeof (RGBQUAD)) * bmp_contents->bmi.win.ncolors) ;
		for (i = 0 ; i < bmp_contents->bmi.win.ncolors ; i++) {
			bmp_contents->bmi.win.colors[i].rgbBlue = (unsigned char) getc (file->stream) ;
			bmp_contents->bmi.win.colors[i].rgbGreen = (unsigned char) getc (file->stream) ;
			bmp_contents->bmi.win.colors[i].rgbRed = (unsigned char) getc (file->stream) ;
			if ((bmp_contents->bmi.win.colors[i].rgbReserved = (unsigned char) getc (file->stream)) != 0) {
				pwarn (_("maybe corrupted windows bmp format (Reserved in RGBQUAD is non-zero)")) ;
			}
		}
	}

	return ;
}

static void bmpos2_readheaders (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	bmp_contents->bmi.os2.bmch.bcSize = BMP_SIZE_BMCOREHEADER ;
	bmp_contents->bmi.os2.bmch.bcWidth = read16_le (file->stream) ;
	bmp_contents->bmi.os2.bmch.bcHeight = read16_le (file->stream) ;
	bmp_contents->bmi.os2.bmch.bcPlanes = read16_le (file->stream) ;
	assert (bmp_contents->bmi.os2.bmch.bcPlanes == 1) ;
	bmp_contents->bmi.os2.bmch.bcBitCount = read16_le (file->stream) ;
	assert ((bmp_contents->bmi.os2.bmch.bcBitCount == 1) ||
			(bmp_contents->bmi.os2.bmch.bcBitCount == 4) ||
			(bmp_contents->bmi.os2.bmch.bcBitCount == 8) || 
			(bmp_contents->bmi.os2.bmch.bcBitCount == 24)) ;

	if (bmp_contents->bmi.os2.bmch.bcBitCount == 24) {
		bmp_contents->bmi.os2.colors = NULL ;
		bmp_contents->bmi.os2.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		int i = 0 ;

		switch (bmp_contents->bmi.os2.bmch.bcBitCount) {
			case 1:
			if (args.action.value == ARGS_ACTION_EMBED) {
				pwarn (_("using a black/white bitmap as cover is very insecure!")) ;
			}
			bmp_contents->bmi.os2.ncolors = 2 ;
			break ;

			case 4:
			if (args.action.value == ARGS_ACTION_EMBED) {
				pwarn (_("using a 16-color bitmap as cover is very insecure!")) ;
			}
			bmp_contents->bmi.os2.ncolors = 16 ;
			break ;

			case 8:
			bmp_contents->bmi.os2.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		bmp_contents->bmi.os2.colors = s_malloc ((sizeof (RGBTRIPLE)) * bmp_contents->bmi.os2.ncolors) ;
		for (i = 0 ; i < bmp_contents->bmi.os2.ncolors ; i++) {
			bmp_contents->bmi.os2.colors[i].rgbtBlue = (unsigned char) getc (file->stream) ;
			bmp_contents->bmi.os2.colors[i].rgbtGreen = (unsigned char) getc (file->stream) ;
			bmp_contents->bmi.os2.colors[i].rgbtRed = (unsigned char) getc (file->stream) ;
		}
	}

	return ;
}

/* writes the headers of a bmp file to disk */
static void bmp_writeheaders (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	write16_le (file->stream, bmp_contents->bmfh.bfType) ;
	write32_le (file->stream, bmp_contents->bmfh.bfSize) ;
	write16_le (file->stream, bmp_contents->bmfh.bfReserved1) ;
	write16_le (file->stream, bmp_contents->bmfh.bfReserved2) ;
	write32_le (file->stream, bmp_contents->bmfh.bfOffBits) ;

	/* FIXME - funktioniert das mit union sicher ? */
	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		bmpwin_writeheaders (file) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		bmpos2_writeheaders (file) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while writing the bmp headers to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing the bmp headers to the file \"%s\"."), file->filename) ;
		}
	}

	return ;
}

static void bmpwin_writeheaders (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	write32_le (file->stream, bmp_contents->bmi.win.bmih.biSize) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biWidth) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biHeight) ;
	write16_le (file->stream, bmp_contents->bmi.win.bmih.biPlanes) ;
	write16_le (file->stream, bmp_contents->bmi.win.bmih.biBitCount) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biCompression) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biSizeImage) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biXPelsPerMeter) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biYPelsPerMeter) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biClrUsed) ;
	write32_le (file->stream, bmp_contents->bmi.win.bmih.biClrImportant) ;

	if (bmp_contents->bmi.win.ncolors > 0) {
		/* a color table exists */
		int i = 0 ;

		for (i = 0 ; i < bmp_contents->bmi.win.ncolors ; i++) {
			putc ((int) bmp_contents->bmi.win.colors[i].rgbBlue, file->stream) ;
			putc ((int) bmp_contents->bmi.win.colors[i].rgbGreen, file->stream) ;
			putc ((int) bmp_contents->bmi.win.colors[i].rgbRed, file->stream) ;
			putc ((int) bmp_contents->bmi.win.colors[i].rgbReserved, file->stream) ;
		}
	}

	return ;
}

static void bmpos2_writeheaders (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;

	write32_le (file->stream, bmp_contents->bmi.os2.bmch.bcSize) ;
	write16_le (file->stream, bmp_contents->bmi.os2.bmch.bcWidth) ;
	write16_le (file->stream, bmp_contents->bmi.os2.bmch.bcHeight) ;
	write16_le (file->stream, bmp_contents->bmi.os2.bmch.bcPlanes) ;
	write16_le (file->stream, bmp_contents->bmi.os2.bmch.bcBitCount) ;

	if (bmp_contents->bmi.os2.ncolors > 0) {
		/* a color table exists */
		int i = 0 ;

		for (i = 0 ; i < bmp_contents->bmi.os2.ncolors ; i++) {
			putc ((int) bmp_contents->bmi.os2.colors[i].rgbtBlue, file->stream) ;
			putc ((int) bmp_contents->bmi.os2.colors[i].rgbtGreen, file->stream) ;
			putc ((int) bmp_contents->bmi.os2.colors[i].rgbtRed, file->stream) ;
		}
	}
	
	return ;
}

/* returns the number of bytes used to store the pixel data of one scan line */
static long bmp_calclinelength (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;
	long retval = 0 ;

	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth % 8 == 0) {
			retval = bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8 ;
		}
		else {
			retval = (bmp_contents->bmi.win.bmih.biBitCount * bmp_contents->bmi.win.bmih.biWidth / 8) + 1;
		}
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth % 8 == 0) {
			retval = bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			retval = (bmp_contents->bmi.os2.bmch.bcBitCount * bmp_contents->bmi.os2.bmch.bcWidth / 8) + 1;
		}
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

static long bmp_getheight (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;
	long retval = 0 ;

	switch (bmp_contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		retval = bmp_contents->bmi.win.bmih.biHeight ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		retval = bmp_contents->bmi.os2.bmch.bcHeight ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

/* reads a bmp file from disk into a CVRSTGFILE structure */
static void bmp_readdata (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;
	long line = 0, linelength = 0, height = 0 ;
	int i = 0, paddinglength = 0 ;
	int c = EOF ;

	linelength = bmp_calclinelength (file) ;
	height = bmp_getheight (file) ;

	if (linelength % 4 == 0) {
		paddinglength = 0 ;
	}
	else {
		paddinglength = 4 - (linelength % 4) ;
	}

	bmp_contents->bitmap = s_malloc ((sizeof (void*)) * height) ;

	for (line = height - 1 ; line >= 0 ; line--) {
		bmp_contents->bitmap[line] = s_malloc (linelength) ;
		if (fread (bmp_contents->bitmap[line], 1, linelength, file->stream) != linelength) {
			bmp_readerror (file) ;
		}

		for (i = 0 ; i < paddinglength ; i++) {
			if ((c = getc (file->stream)) != 0) {
				if (c == EOF) {
					bmp_readerror (file) ;
				}
				else {
					pwarn (_("maybe corrupted bmp format (padding byte set to non-zero)")) ;
				}
			}
		}
	}

	return ;
}

/* writes a bmp file from a CVRSTGFILE structure to disk */
static void bmp_writedata (CVRSTGFILE *file)
{
	BMP_CONTENTS *bmp_contents = ((BMP_CONTENTS *) file->contents) ;
	long line = 0, linelength = 0, height = 0 ;
	int i = 0, paddinglength = 0 ;
	
	linelength = bmp_calclinelength (file) ;
	height = bmp_getheight (file) ;

	if (linelength % 4 == 0) {
		paddinglength = 0 ;
	}
	else {
		paddinglength = 4 - (linelength % 4) ;
	}

	for (line = height - 1 ; line >= 0 ; line--) {
		if (fwrite (bmp_contents->bitmap[line], 1, linelength, file->stream) != linelength) {
			bmp_writeerror (file) ;
		}

		for (i = 0 ; i < paddinglength ; i++) {
			if (putc (0, file->stream) == EOF) {
				bmp_writeerror (file) ;
			}
		}
	}

	return ;
}

static void bmp_readerror (CVRSTGFILE *file)
{
	if (feof (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("premature end of bmp data from standard input.")) ;
		}
		else {
			exit_err (_("premature end of bmp file \"%s\"."), file->filename) ;
		}
	}
	else if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while reading the bitmap data from standard input.")) ;
		}
		else {
			exit_err (_("an error occured while reading the bitmap data of the file \"%s\"."), file->filename) ;
		}
	}
	else {
		assert (0) ;
	}

	return ;
}

static void bmp_writeerror (CVRSTGFILE *file)
{
	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err (_("an error occured while writing the bitmap data to standard output.")) ;
		}
		else {
			exit_err (_("an error occured while writing the bitmap data to the file \"%s\"."), file->filename) ;
		}
	}
	else {
		assert (0) ;
	}

	return ;
}
