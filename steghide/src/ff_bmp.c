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
#include "io.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

static void bmp_readheaders (CVRSTGFILE *file) ;
static void bmpwin_readheaders (CVRSTGFILE *file)
static void bmpos2_readheaders (CVRSTGFILE *file)
static void bmp_writeheaders (CVRSTGFILE *file) ;
static void bmpwin_writeheaders (CVRSTGFILE *file) ;
static void bmpos2_writeheaders (CVRSTGFILE *file) ;
static void bmp_readdata (CVRSTGFILE *file) ;
static void bmp_writedata (CVRSTGFILE *file) ;
static void bmp_readerror (CVRSTGFILE *file)
static void bmp_calcRC (CVRSTGFILE *file, unsigned long pos, unsigned long *row, unsigned long *column) ;

void bmp_readfile (CVRSTGFILE *file)
{
	file->contents = s_malloc (sizeof BMP_CONTENTS) ;

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

	switch (file->contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth % 8 == 0) {
			linelength = file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8 ;
		}
		else {
			linelength = (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8) + 1;
		}
		retval = file->contents->bmi.win.bmih.biHeight * linelength ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth % 8 == 0) {
			linelength = file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			linelength = (file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth / 8) + 1;
		}
		retval = file->contents->bmi.os2.bmch.bcHeight * linelength ;
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
	unsigned char **ptr = file->contents->bitmap ;

	assert (value == 0 || value == 1) ;

	bmp_calcRC (file, pos, &row, &column) ;
	ptr[row][column] = (ptr[row][column] & ~1) | value ;

	return ;
}

int bmp_extractbit (CVRSTGFILE *file, unsigned long pos)
{
	unsigned long row = 0, column = 0 ;
	unsigned char **ptr = file->contents->bitmap ;

	bmp_calcRC (file, pos, &row, &column) ;
	return (ptr[row][column] & 1) ;
}

void bmp_cleanup (CVRSTGFILE *file)
{
	int i = 0 ;

	switch (file->contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		free (file->contents->bmi.win.colors) ;
		for (i = 0 ; i < file->contents->bmi.win.bmih.biHeight ; i++) {
			free (file->contents->bitmap[i]) ;
		}
		free (file->contents->bitmap) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		free (file->contents->bmi.os2.colors) ;
		for (i = 0 ; i < file->contents->bmi.os2.bmch.bcHeight ; i++) {
			free (file->contents->bitmap[i]) ;
		}
		free (file->contents->bitmap) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return ;
}

static void bmp_calcRC (CVRSTGFILE *file, unsigned long pos, unsigned long *row, unsigned long *column)
{
	unsigned long width = 0 /* in bytes */, height = 0 ;

	switch (file->contents->bmi.win.bmih.biSize) {
		case BMP_SIZE_BMINFOHEADER:
		if (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth % 8 == 0) {
			width = file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8 ;
		}
		else {
			width = (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8) + 1;
		}
		height = file->contents->bmi.win.bmih.biHeight ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		if (file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth % 8 == 0) {
			width = file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			width = (file->contents->bmi.os2.bmch.bcBitCount * file->contents->bmi.os2.bmch.bcWidth / 8) + 1;
		}
		height = file->contents->bmi.os2.bmch.bcHeight ;
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

	file->contents->bmfh = s_malloc (sizeof BITMAPFILEHEADER) ;
	file->contents->bmfh.bfType = read16_le (file->stream) ;	/* FIXME - test this ! */
	file->contents->bmfh.bfSize = read32_le (file->stream) ;
	file->contents->bmfh.bfReserved1 = read16_le (file->stream) ;
	file->contents->bmfh.bfReserved2 = read16_le (file->stream) ;
	file->contents->bmfh.bfOffBits = read32_le (file->stream) ;
	
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
	file->contents->bmi.win.bmih = s_malloc (sizeof BITMAPINFOHEADER) ;

	file->contents->bmi.win.bmih.Size = BMP_SIZE_BMINFOHEADER ;
	file->contents->bmi.win.bmih.Width = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.Height = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.Planes = read16_le (file->stream) ;
	assert (file->contents->bmi.win.bmih.Planes == 1) ;
	file->contents->bmi.win.bmih.BitCount = read16_le (file->stream) ;
	assert ((file->contents->bmi.win.bmih.BitCount == 1) ||
			(file->contents->bmi.win.bmih.BitCount == 4) ||
			(file->contents->bmi.win.bmih.BitCount == 8) || 
			(file->contents->bmi.win.bmih.BitCount == 24)) ;
	file->contents->bmi.win.bmih.Compression = read32_le (file->stream) ;
	if (file->contents->bmi.win.bmih.Compression != BMP_BI_RGB) {
		if (file->filename == NULL) {
			exit_err (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			exit_err (_("the bitmap data in \"%s\" is compressed which is not supported."), file->filename) ;
		}
	}
	file->contents->bmi.win.bmih.SizeImage = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.XPelsPerMeter = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.YPelsPerMeter = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.ClrUsed = read32_le (file->stream) ;
	file->contents->bmi.win.bmih.ClrImportant = read32_le (file->stream) ;

	if (file->contents->bmi.win.bmih.BitCount == 24) {
		file->contents->bmi.win.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		int i = 0 ;

		if (file->contents->bmi.win.bmih.ClrUsed != 0) {
			file->contents->bmi.win.ncolors = file->contents->bmi.win.bmih.ClrUsed ;
		}
		else {
			switch (file->contents->bmi.win.bmih.BitCount) {
				case 1:
				pwarn (_("using a black/white bitmap as cover is very insecure!")) ;
				file->contents->bmi.win.ncolors = 2 ;
				break ;

				case 4:
				pwarn (_("using a 16-color bitmap as cover is very insecure!")) ;
				file->contents->bmi.win.ncolors = 16 ;
				break ;

				case 8:
				file->contents->bmi.win.ncolors = 256 ;
				break ;

				default:
				assert (0) ;
				break ;
			}
		}

		file->contents->bmi.win.colors = s_malloc ((sizeof RGBQUAD) * file->contents->bmi.win.ncolors) ;
		for (i = 0 ; i < file->contents->bmi.win.ncolors ; i++) {
			file->contents->bmi.win.colors[i].Blue = (unsigned char) getc (file->stream) ;
			file->contents->bmi.win.colors[i].Green = (unsigned char) getc (file->stream) ;
			file->contents->bmi.win.colors[i].Red = (unsigned char) getc (file->stream) ;
			if ((file->contents->bmi.win.colors[i].Reserved = (unsigned char) getc (file->stream)) != 0) {
				pwarn (_("maybe corrupted windows bmp format (Reserved in RGBQUAD is non-zero)")) ;
			}
		}
	}

	return ;
}

static void bmpos2_readheaders (CVRSTGFILE *file)
{
	file->contents->bmi.os2.bmch = s_malloc (sizeof BITMAPCOREHEADER) ;

	file->contents->bmi.os2.bmch.Size = BMP_SIZE_BMCOREHEADER ;
	file->contents->bmi.os2.bmch.Width = read16_le (file->stream) ;
	file->contents->bmi.os2.bmch.Height = read16_le (file->stream) ;
	file->contents->bmi.os2.bmch.Planes = read16_le (file->stream) ;
	assert (file->contents->bmi.os2.bmch.Planes == 1) ;
	file->contents->bmi.os2.bmch.BitCount = read16_le (file->stream) ;
	assert ((file->contents->bmi.os2.bmch.BitCount == 1) ||
			(file->contents->bmi.os2.bmch.BitCount == 4) ||
			(file->contents->bmi.os2.bmch.BitCount == 8) || 
			(file->contents->bmi.os2.bmch.BitCount == 24)) ;

	if (file->contents->bmi.os2.bmch.BitCount == 24) {
		file->contents->bmi.os2.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		int i = 0 ;

		switch (file->contents->bmi.os2.bmch.BitCount) {
			case 1:
			pwarn (_("using a black/white bitmap as cover is very insecure!")) ;
			file->contents->bmi.os2.ncolors = 2 ;
			break ;

			case 4:
			pwarn (_("using a 16-color bitmap as cover is very insecure!")) ;
			file->contents->bmi.os2.ncolors = 16 ;
			break ;

			case 8:
			file->contents->bmi.os2.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		file->contents->bmi.os2.colors = s_malloc ((sizeof RGBTRIPLE) * file->contents->bmi.os2.ncolors) ;
		for (i = 0 ; i < file->contents->bmi.os2.ncolors ; i++) {
			file->contents->bmi.os2.colors[i].Blue = (unsigned char) getc (file->stream) ;
			file->contents->bmi.os2.colors[i].Green = (unsigned char) getc (file->stream) ;
			file->contents->bmi.os2.colors[i].Red = (unsigned char) getc (file->stream) ;
		}
	}

	return ;
}

/* writes the headers of a bmp file to disk */
static void bmp_writeheaders (CVRSTGFILE *file)
{
	write16_le (file->stream, file->contents->bmfh.bfType) ;
	write32_le (file->stream, file->contents->bmfh.bfSize) ;
	write16_le (file->stream, file->contents->bmfh.bfReserved1) ;
	write16_le (file->stream, file->contents->bmfh.bfReserved2) ;
	write32_le (file->stream, file->contents->bmfh.bfOffBits) ;

	/* FIXME - funktioniert das mit union sicher ? */
	switch (file->contents->bmi.win.bmih.biSize) {
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

	if (file->unsupdata1len != 0) {
		unsigned char *ptrunsupdata1 = file->unsupdata1 ;
		int i = 0 ;

		for (i = 0 ; i < file->unsupdata1len ; i++) {
			putc ((int) ptrunsupdata1[i], file->stream) ;
		}
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
	write32_le (file->stream, file->contents->bmi.win.bmih.biSize) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biWidth) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biHeight) ;
	write16_le (file->stream, file->contents->bmi.win.bmih.biPlanes) ;
	write16_le (file->stream, file->contents->bmi.win.bmih.biBitCount) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biCompression) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biSizeImage) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biXPelsPerMeter) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biYPelsPerMeter) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biClrUsed) ;
	write32_le (file->stream, file->contents->bmi.win.bmih.biClrImportant) ;

	if (file->contents->bmi.win.ncolors > 0) {
		/* a color table exists */
		int i = 0 ;

		for (i = 0 ; i < file->contents->bmi.win.ncolors ; i++) {
			putc ((int) file->contents->bmi.win.colors[i].Blue, file->stream) ;
			putc ((int) file->contents->bmi.win.colors[i].Green, file->stream) ;
			putc ((int) file->contents->bmi.win.colors[i].Red, file->stream) ;
			putc ((int) file->contents->bmi.win.colors[i].Reserved, file->stream) ;
		}
	}

	return ;
}

static void bmpos2_writeheaders (CVRSTGFILE *file)
{
	write32_le (file->stream, file->contents->bmi.os2.bmch.bcSize) ;
	write16_le (file->stream, file->contents->bmi.os2.bmch.bcWidth) ;
	write16_le (file->stream, file->contents->bmi.os2.bmch.bcHeight) ;
	write16_le (file->stream, file->contents->bmi.os2.bmch.bcPlanes) ;
	write16_le (file->stream, file->contents->bmi.os2.bmch.bcBitCount) ;

	if (file->contents->bmi.os2.ncolors > 0) {
		/* a color table exists */
		int i = 0 ;

		for (i = 0 ; i < file->contents->bmi.os2.ncolors ; i++) {
			putc ((int) file->contents->bmi.os2.colors[i].Blue, file->stream) ;
			putc ((int) file->contents->bmi.os2.colors[i].Green, file->stream) ;
			putc ((int) file->contents->bmi.os2.colors[i].Red, file->stream) ;
		}
	}
	
	return ;
}

/* reads a bmp file from disk into a CVRSTGFILE structure */
static void bmp_readdata (CVRFILE *file)
{
	unsigned long line = 0, linelength = 0 ;
	int i = 0, paddinglength = 0 ;
	int c = EOF ;

	/* auch für os2 FIXME */
	if (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth % 8 == 0) {
		linelength = file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8 ;
	}
	else {
		linelength = (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8) + 1;
	}

	if (linelength % 4 == 0) {
		paddinglength = 0 ;
	}
	else {
		paddinglength = 4 - (linelength % 4) ;
	}

	file->contents->bitmap = s_malloc ((sizeof void*) * file->contents->bmi.win.bmih.biHeight) ;

	for (line = file->contents->bmi.win.bmih.biHeight ; line >= 0 ; line--) {
		file->contents->bitmap[line] = s_malloc (linelength) ;
		if (fread (file->contents->bitmap[line], linelength, 1, file->stream) != 1) {
			bmp_readerror (file) ;
		}

		for (i = 0 ; i < paddinglength ; i++) {
			if ((c = getc (file->stream)) != 0) {
				if (c == EOF) {
					bmp_readerror (file) ;
				}
				else {
					pwarn (_("maybe corrupted bmp format (padding byte set to non-zero)")).
				}
			}
		}
	}

	return ;
}

/* writes a bmp file from a CVRSTGFILE structure to disk */
static void bmp_writedata (CVRFILE *file)
{
	unsigned long line = 0, linelength = 0 ;
	int i = 0, paddinglength = 0 ;
	
	/* auch für os2 FIXME */
	if (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth % 8 == 0) {
		linelength = file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8 ;
	}
	else {
		linelength = (file->contents->bmi.win.bmih.biBitCount * file->contents->bmi.win.bmih.biWidth / 8) + 1;
	}

	if (linelength % 4 == 0) {
		paddinglength = 0 ;
	}
	else {
		paddinglength = 4 - (linelength % 4) ;
	}

	for (line = file->contents->bmi.win.bmih.biHeight ; line >= 0 ; line--) {
		if (fwrite (file->contents->bitmap[line], linelength, 1, file->stream) != 1) {
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
