/*
 * steghide 0.4.6 - a steganography program
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

#include "error.h"
#include "main.h"
#include "cvrstgfile.h"
#include "bmpfile.h"
#include "bufmanag.h"
#include "support.h"
#include "msg.h"

BmpFile::BmpFile ()
	: CvrStgFile()
{
	// empty
}

BmpFile::BmpFile (BinaryIO *io)
	: CvrStgFile()
{
	read (io) ;
}

// FIXME - die nächsten vier funkt ordentlich machen !
// Daten als Pointer!!
BmpFile::BITMAPFILEHEADER *BmpFile::getBMFH (void)
{
	return &bmfh ;
}

void BmpFile::setBMFH (BITMAPFILEHEADER *x)
{
	bmfh = *x ;
}

BmpFile::BMPINFO *BmpFile::getBmpInfo (void)
{
	return &bmi ;
}

void BmpFile::setBmpInfo (BMPINFO *x)
{
	bmi = *x ;
}

BmpFile::~BmpFile ()
{
	switch (getSubformat()) {
		case WIN:
		if (bmi.win.ncolors > 0) {
			free (bmi.win.colors) ;
		}
		for (int i = 0 ; i < bmi.win.bmih.biHeight ; i++) {
			free (bitmap[i]) ;
		}
		free (bitmap) ;
		break ;

		case OS2:
		if (bmi.os2.ncolors > 0) {
			free (bmi.os2.colors) ;
		}
		for (unsigned int i = 0 ; i < bmi.os2.bmch.bcHeight ; i++) {
			free (bitmap[i]) ;
		}
		free (bitmap) ;
		break ;

		default:
		assert (0) ;
		break ;
	}
}

BmpFile::SUBFORMAT BmpFile::getSubformat ()
{
	SUBFORMAT retval ;

	/* FIXME - funktioniert das mit union sicher ? */
	switch (bmi.win.bmih.biSize) {
		case SizeBMINFOHEADER:
		retval = WIN ;
		break ;

		case SizeBMCOREHEADER:
		retval = OS2 ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void BmpFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	readheaders() ;
	readdata() ;
}

void BmpFile::write ()
{
	CvrStgFile::write() ;

	writeheaders() ;
	writedata() ;
}

unsigned long BmpFile::getCapacity ()
{
	unsigned long linelength = 0, retval = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth % 8 == 0) {
			linelength = bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8 ;
		}
		else {
			linelength = (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8) + 1;
		}
		retval = bmi.win.bmih.biHeight * linelength ;
		break ;

		case OS2:
		if (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth % 8 == 0) {
			linelength = bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			linelength = (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8) + 1;
		}
		retval = bmi.os2.bmch.bcHeight * linelength ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void BmpFile::embedBit (unsigned long pos, int value)
{
	assert (pos <= getCapacity()) ;
	assert (value == 0 || value == 1) ;

	unsigned long row = 0, column = 0 ;
	calcRC (pos, &row, &column) ;
	bitmap[row][column] = (bitmap[row][column] & (unsigned char) ~1) | value ;
}

int BmpFile::extractBit (unsigned long pos)
{
	assert (pos <= getCapacity()) ;

	unsigned long row = 0, column = 0 ;
	calcRC (pos, &row, &column) ;
	return (bitmap[row][column] & 1) ;
}

void BmpFile::calcRC (unsigned long pos, unsigned long *row, unsigned long *column)
{
	unsigned long width = 0 /* in bytes */, height = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth % 8 == 0) {
			width = bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8 ;
		}
		else {
			width = (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8) + 1;
		}
		height = bmi.win.bmih.biHeight ;
		break ;

		case OS2:
		if (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth % 8 == 0) {
			width = bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			width = (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8) + 1;
		}
		height = bmi.os2.bmch.bcHeight ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	*row = height - (pos / width) - 1 ;
	*column = pos % width ;
}

/* reads the headers of a bmp file from disk */
void BmpFile::readheaders ()
{
	try {
		bmfh.bfType = IdBm ;
		bmfh.bfSize = BinIO->read32_le() ;
		bmfh.bfReserved1 = BinIO->read16_le() ;
		bmfh.bfReserved2 = BinIO->read16_le() ;
		bmfh.bfOffBits = BinIO->read32_le() ;
		
		unsigned long tmpSize = BinIO->read32_le() ;
		switch (tmpSize) {
			case SizeBMINFOHEADER:	/* file has windows bmp format */
			bmpwin_readheaders () ;
			break ;

			case SizeBMCOREHEADER:	/* file has OS/2 bmp format */
			bmpos2_readheaders () ;
			break ;

			default:
			if (BinIO->is_std()) {
				throw SteghideError (_("the bmp data from standard input has a format that is not supported.")) ;
			}
			else {
				throw SteghideError (_("the bmp file \"%s\" has a format that is not supported."), BinIO->getName().c_str()) ;
			}
			break ;
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the bmp headers from the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading bmp headers."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::STDIN_ERR:
			throw SteghideError (_("an error occured while reading the bmp headers from standard input.")) ;
			break ;

			case BinaryInputError::STDIN_EOF:
			throw SteghideError (_("premature end of data from standard input while reading bmp headers.")) ;
			break ;
		}
	}
}

void BmpFile::bmpwin_readheaders ()
{
	bmi.win.bmih.biSize = SizeBMINFOHEADER ;
	bmi.win.bmih.biWidth = BinIO->read32_le () ;
	bmi.win.bmih.biHeight = BinIO->read32_le () ;
	bmi.win.bmih.biPlanes = BinIO->read16_le () ;
	assert (bmi.win.bmih.biPlanes == 1) ;
	bmi.win.bmih.biBitCount = BinIO->read16_le () ;
	assert ((bmi.win.bmih.biBitCount == 1) ||
			(bmi.win.bmih.biBitCount == 4) ||
			(bmi.win.bmih.biBitCount == 8) || 
			(bmi.win.bmih.biBitCount == 24)) ;
	bmi.win.bmih.biCompression = BinIO->read32_le () ;
	if (bmi.win.bmih.biCompression != BI_RGB) {
		if (BinIO->is_std()) {
			throw SteghideError (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			throw SteghideError (_("the bitmap data in \"%s\" is compressed which is not supported."), BinIO->getName().c_str()) ;
		}
	}
	bmi.win.bmih.biSizeImage = BinIO->read32_le () ;
	bmi.win.bmih.biXPelsPerMeter = BinIO->read32_le () ;
	bmi.win.bmih.biYPelsPerMeter = BinIO->read32_le () ;
	bmi.win.bmih.biClrUsed = BinIO->read32_le () ;
	bmi.win.bmih.biClrImportant = BinIO->read32_le () ;

	if (bmi.win.bmih.biBitCount == 24) {
		bmi.win.colors = NULL ;
		bmi.win.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		switch (bmi.win.bmih.biBitCount) {
			case 1:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi.win.ncolors = 2 ;
			break ;

			case 4:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi.win.ncolors = 16 ;
			break ;

			case 8:
			bmi.win.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}
		if (bmi.win.bmih.biClrUsed != 0) {
			bmi.win.ncolors = bmi.win.bmih.biClrUsed ;
		}

		bmi.win.colors = (RGBQUAD *) s_malloc ((sizeof (RGBQUAD)) * bmi.win.ncolors) ;
		for (unsigned int i = 0 ; i < bmi.win.ncolors ; i++) {
			bmi.win.colors[i].rgbBlue = BinIO->read8() ;
			bmi.win.colors[i].rgbGreen = BinIO->read8() ;
			bmi.win.colors[i].rgbRed = BinIO->read8() ;
			if ((bmi.win.colors[i].rgbReserved = BinIO->read8()) != 0) {
				Warning w (_("maybe corrupted windows bmp data (Reserved in RGBQUAD is non-zero).")) ;
				w.printMessage() ;
			}
		}
	}

	return ;
}

void BmpFile::bmpos2_readheaders ()
{
	bmi.os2.bmch.bcSize = SizeBMCOREHEADER ;
	bmi.os2.bmch.bcWidth = BinIO->read16_le () ;
	bmi.os2.bmch.bcHeight = BinIO->read16_le () ;
	bmi.os2.bmch.bcPlanes = BinIO->read16_le () ;
	assert (bmi.os2.bmch.bcPlanes == 1) ;
	bmi.os2.bmch.bcBitCount = BinIO->read16_le () ;
	assert ((bmi.os2.bmch.bcBitCount == 1) ||
			(bmi.os2.bmch.bcBitCount == 4) ||
			(bmi.os2.bmch.bcBitCount == 8) || 
			(bmi.os2.bmch.bcBitCount == 24)) ;

	if (bmi.os2.bmch.bcBitCount == 24) {
		bmi.os2.colors = NULL ;
		bmi.os2.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		switch (bmi.os2.bmch.bcBitCount) {
			case 1:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi.os2.ncolors = 2 ;
			break ;

			case 4:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi.os2.ncolors = 16 ;
			break ;

			case 8:
			bmi.os2.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		bmi.os2.colors = (RGBTRIPLE *) s_malloc ((sizeof (RGBTRIPLE)) * bmi.os2.ncolors) ;
		for (unsigned int i = 0 ; i < bmi.os2.ncolors ; i++) {
			bmi.os2.colors[i].rgbtBlue = BinIO->read8() ;
			bmi.os2.colors[i].rgbtGreen = BinIO->read8() ;
			bmi.os2.colors[i].rgbtRed = BinIO->read8() ;
		}
	}

	return ;
}

/* writes the headers of a bmp file to disk */
void BmpFile::writeheaders ()
{
	try {
		BinIO->write16_le (bmfh.bfType) ;
		BinIO->write32_le (bmfh.bfSize) ;
		BinIO->write16_le (bmfh.bfReserved1) ;
		BinIO->write16_le (bmfh.bfReserved2) ;
		BinIO->write32_le (bmfh.bfOffBits) ;

		switch (getSubformat()) {
			case WIN:
			bmpwin_writeheaders() ;
			break ;

			case OS2:
			bmpos2_writeheaders() ;
			break ;

			default:
			assert (0) ;
			break ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the bmp headers to the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the bmp headers to standard output.")) ;
			break ;
		}
	}
}

void BmpFile::bmpwin_writeheaders ()
{
	BinIO->write32_le (bmi.win.bmih.biSize) ;
	BinIO->write32_le (bmi.win.bmih.biWidth) ;
	BinIO->write32_le (bmi.win.bmih.biHeight) ;
	BinIO->write16_le (bmi.win.bmih.biPlanes) ;
	BinIO->write16_le (bmi.win.bmih.biBitCount) ;
	BinIO->write32_le (bmi.win.bmih.biCompression) ;
	BinIO->write32_le (bmi.win.bmih.biSizeImage) ;
	BinIO->write32_le (bmi.win.bmih.biXPelsPerMeter) ;
	BinIO->write32_le (bmi.win.bmih.biYPelsPerMeter) ;
	BinIO->write32_le (bmi.win.bmih.biClrUsed) ;
	BinIO->write32_le (bmi.win.bmih.biClrImportant) ;

	if (bmi.win.ncolors > 0) {
		for (unsigned int i = 0 ; i < bmi.win.ncolors ; i++) {
			BinIO->write8 (bmi.win.colors[i].rgbBlue) ;
			BinIO->write8 (bmi.win.colors[i].rgbGreen) ;
			BinIO->write8 (bmi.win.colors[i].rgbRed) ;
			BinIO->write8 (bmi.win.colors[i].rgbReserved) ;
		}
	}
}

void BmpFile::bmpos2_writeheaders ()
{
	BinIO->write32_le (bmi.os2.bmch.bcSize) ;
	BinIO->write16_le (bmi.os2.bmch.bcWidth) ;
	BinIO->write16_le (bmi.os2.bmch.bcHeight) ;
	BinIO->write16_le (bmi.os2.bmch.bcPlanes) ;
	BinIO->write16_le (bmi.os2.bmch.bcBitCount) ;

	if (bmi.os2.ncolors > 0) {
		for (unsigned int i = 0 ; i < bmi.os2.ncolors ; i++) {
			BinIO->write8 (bmi.os2.colors[i].rgbtBlue) ;
			BinIO->write8 (bmi.os2.colors[i].rgbtGreen) ;
			BinIO->write8 (bmi.os2.colors[i].rgbtRed) ;
		}
	}
}

/* returns the number of bytes used to store the pixel data of one scan line */
long BmpFile::calcLinelength ()
{
	long retval = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth % 8 == 0) {
			retval = bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8 ;
		}
		else {
			retval = (bmi.win.bmih.biBitCount * bmi.win.bmih.biWidth / 8) + 1;
		}
		break ;

		case OS2:
		if (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth % 8 == 0) {
			retval = bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8 ;
		}
		else {
			retval = (bmi.os2.bmch.bcBitCount * bmi.os2.bmch.bcWidth / 8) + 1;
		}
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

long BmpFile::getHeight ()
{
	long retval = 0 ;

	switch (getSubformat()) {
		case WIN:
		retval = bmi.win.bmih.biHeight ;
		break ;

		case OS2:
		retval = bmi.os2.bmch.bcHeight ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

/* reads a bmp file from disk into a CVRSTGFILE structure */
void BmpFile::readdata ()
{
	try {
		long linelength = calcLinelength () ;
		long height = getHeight () ;

		int paddinglength = 0 ;
		if (linelength % 4 == 0) {
			paddinglength = 0 ;
		}
		else {
			paddinglength = 4 - (linelength % 4) ;
		}

		bitmap = (unsigned char **) s_malloc ((sizeof (unsigned char*)) * height) ;

		for (long line = height - 1 ; line >= 0 ; line--) {
			bitmap[line] = (unsigned char *) s_malloc (linelength) ;
			for (long posinline = 0 ; posinline < linelength ; posinline++) {
				bitmap[line][posinline] = BinIO->read8() ;
			}

			for (int i = 0 ; i < paddinglength ; i++) {
				if (BinIO->read8() != 0) {
					Warning w (_("maybe corrupted bmp data (padding byte set to non-zero).")) ;
					w.printMessage() ;
				}
			}
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the bmp data from the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading bmp data."), BinIO->getName().c_str()) ;
			break ;

			case BinaryInputError::STDIN_ERR:
			throw SteghideError (_("an error occured while reading the bmp data from standard input.")) ;
			break ;

			case BinaryInputError::STDIN_EOF:
			throw SteghideError (_("premature end of bmp data from standard input.")) ;
			break ;
		}
	}
}

void BmpFile::writedata ()
{
	try {
		long linelength = calcLinelength () ;
		long height = getHeight () ;

		unsigned int paddinglength = 0 ;
		if (linelength % 4 == 0) {
			paddinglength = 0 ;
		}
		else {
			paddinglength = 4 - (linelength % 4) ;
		}

		for (long line = height - 1 ; line >= 0 ; line--) {
			for (long posinline = 0 ; posinline < linelength ; posinline++) {
				BinIO->write8 (bitmap[line][posinline]) ;
			}

			for (unsigned int i = 0 ; i < paddinglength ; i++) {
				BinIO->write8 (0) ;
			}
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the bitmap data to the file \"%s\"."), BinIO->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the bitmap data to standard output.")) ;
			break ;
		}

	}
}
