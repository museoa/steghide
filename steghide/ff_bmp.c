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
#include <assert.h>

#include "main.h"
#include "io.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

/* reads the headers of a bmp file from disk */
void bmp_readheaders (CVRFILE *file)
{
	file->headers->bmp.bmfh.bfType = BMP_ID_BM ;
	file->headers->bmp.bmfh.bfSize = read32_le (file->stream) ;
	file->headers->bmp.bmfh.bfReserved1 = read16_le (file->stream) ;
	file->headers->bmp.bmfh.bfReserved2 = read16_le (file->stream) ;
	file->headers->bmp.bmfh.bfOffBits = read32_le (file->stream) ;
	
	file->headers->bmp.bmxh.Size = read32_le (file->stream) ;
	switch (file->headers->bmp.bmxh.Size) {
		case BMP_SIZE_BMINFOHEADER:
		file->headers->bmp.bmxh.Width = read32_le (file->stream) ;
		file->headers->bmp.bmxh.Height = read32_le (file->stream) ;
		file->headers->bmp.bmxh.Planes = read16_le (file->stream) ;
		file->headers->bmp.bmxh.BitCount = read16_le (file->stream) ;
		if ((file->headers->bmp.bmxh.Compression = read32_le (file->stream)) != BMP_BI_RGB) {
			if (file->filename == NULL) {
				exit_err ("the bitmap data from standard input is compressed which is not supported.") ;
			}
			else {
				exit_err ("the bitmap data in \"%s\" is compressed which is not supported.", file->filename) ;
			}
		}
		file->headers->bmp.bmxh.SizeImage = read32_le (file->stream) ;
		file->headers->bmp.bmxh.XPelsPerMeter = read32_le (file->stream) ;
		file->headers->bmp.bmxh.YPelsPerMeter = read32_le (file->stream) ;
		file->headers->bmp.bmxh.ClrUsed = read32_le (file->stream) ;
		file->headers->bmp.bmxh.ClrImportant = read32_le (file->stream) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		file->headers->bmp.bmxh.Width = read16_le (file->stream) ;
		file->headers->bmp.bmxh.Height = read16_le (file->stream) ;
		file->headers->bmp.bmxh.Planes = read16_le (file->stream) ;
		file->headers->bmp.bmxh.BitCount = read16_le (file->stream) ;
		break ;

		default:
		if (file->filename == NULL) {
			exit_err ("the bmp data from standard input has a format that is not supported.") ;
		}
		else {
			exit_err ("the bmp file \"%s\" has a format that is not supported.", file->filename) ;
		}
		break ;
	}

	if (file->headers->bmp.bmxh.BitCount == 24) {
		file->unsupdata1 = NULL ;
		file->unsupdata1len = 0 ;
	}
	else {
		unsigned char *ptrunsupdata1 = NULL ;
		int i = 0 ;

		file->unsupdata1len = file->headers->bmp.bmfh.bfOffBits - (BMP_SIZE_BMFILEHEADER + file->headers->bmp.bmxh.Size) ;

		file->unsupdata1 = s_malloc (file->unsupdata1len) ;

		ptrunsupdata1 = file->unsupdata1 ;
		for (i = 0 ; i < file->unsupdata1len ; i++) {
			ptrunsupdata1[i] = (unsigned char) getc (file->stream) ;
		}
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the bmp headers from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the bmp headers of the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

/* writes the headers of a bmp file to disk */
void bmp_writeheaders (CVRFILE *file)
{
	write16_le (file->stream, file->headers->bmp.bmfh.bfType) ;
	write32_le (file->stream, file->headers->bmp.bmfh.bfSize) ;
	write16_le (file->stream, file->headers->bmp.bmfh.bfReserved1) ;
	write16_le (file->stream, file->headers->bmp.bmfh.bfReserved2) ;
	write32_le (file->stream, file->headers->bmp.bmfh.bfOffBits) ;

	switch (file->headers->bmp.bmxh.Size) {
		case BMP_SIZE_BMINFOHEADER:
		write32_le (file->stream, file->headers->bmp.bmxh.Size) ;
		write32_le (file->stream, file->headers->bmp.bmxh.Width) ;
		write32_le (file->stream, file->headers->bmp.bmxh.Height) ;
		write16_le (file->stream, file->headers->bmp.bmxh.Planes) ;
		write16_le (file->stream, file->headers->bmp.bmxh.BitCount) ;
		write32_le (file->stream, file->headers->bmp.bmxh.Compression) ;
		write32_le (file->stream, file->headers->bmp.bmxh.SizeImage) ;
		write32_le (file->stream, file->headers->bmp.bmxh.XPelsPerMeter) ;
		write32_le (file->stream, file->headers->bmp.bmxh.YPelsPerMeter) ;
		write32_le (file->stream, file->headers->bmp.bmxh.ClrUsed) ;
		write32_le (file->stream, file->headers->bmp.bmxh.ClrImportant) ;
		break ;

		case BMP_SIZE_BMCOREHEADER:
		write32_le (file->stream, file->headers->bmp.bmxh.Size) ;
		write16_le (file->stream, file->headers->bmp.bmxh.Width) ;
		write16_le (file->stream, file->headers->bmp.bmxh.Height) ;
		write16_le (file->stream, file->headers->bmp.bmxh.Planes) ;
		write16_le (file->stream, file->headers->bmp.bmxh.BitCount) ;
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
			exit_err ("an error occured while writing the bmp headers to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing the bmp headers to the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

/* reads a bmp file from disk into a CVRFILE structure */
void bmp_readfile (CVRFILE *file)
{
	unsigned long posinline = 0, line = 0 ;
	unsigned long bufpos = 0 ;
	int c = EOF ;

	while (line < file->headers->bmp.bmxh.Height) {
		while (posinline < file->headers->bmp.bmxh.BitCount * file->headers->bmp.bmxh.Width / 8) {
			if ((c = getc (file->stream)) == EOF) {
				if (file->filename == NULL) {
					exit_err ("premature end of bmp data from standard input.") ;
				}
				else {
					exit_err ("premature end of bmp file \"%s\".", file->filename) ;
				}
			}
			bufsetbyte (file->cvrbuflhead, bufpos, c) ;
			bufpos++ ;
			posinline++ ;
		}

		/* read padding bytes */
		while (posinline++ % 4 != 0) {
			getc (file->stream) ;
		}

		line++ ;
		posinline = 0 ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while reading the bitmap data from standard input.") ;
		}
		else {
			exit_err ("an error occured while reading the bitmap data of the file \"%s\".", file->filename) ;
		}
	}
	
	return ;
}

/* writes a bmp file from a CVRFILE structure to disk */
void bmp_writefile (CVRFILE *file)
{
	unsigned long posinline = 0, line = 0 ;
	unsigned long bufpos = 0 ;

	bmp_writeheaders (file) ;

	while (line < file->headers->bmp.bmxh.Height) {
		while (posinline < file->headers->bmp.bmxh.BitCount * file->headers->bmp.bmxh.Width / 8) {
			putc (bufgetbyte (file->cvrbuflhead, bufpos), file->stream) ;
			bufpos++ ;
			posinline++ ;
		}

		/* write padding bytes */
		while (posinline++ % 4 != 0) {
			putc (0, file->stream) ;
		}

		line++ ;
		posinline = 0 ;
	}

	if (ferror (file->stream)) {
		if (file->filename == NULL) {
			exit_err ("an error occured while writing the bitmap data to standard output.") ;
		}
		else {
			exit_err ("an error occured while writing the bitmap data to the file \"%s\".", file->filename) ;
		}
	}

	return ;
}

