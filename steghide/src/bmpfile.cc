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

BmpFile::~BmpFile ()
{
}

BmpFile::SUBFORMAT BmpFile::getSubformat () const
{
	return subformat ;
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

unsigned long BmpFile::getCapacity () const
{
	unsigned long linelength = 0, retval = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth % 8 == 0) {
			linelength = bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8 ;
		}
		else {
			linelength = (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8) + 1;
		}
		retval = bmi_win.bmih.biHeight * linelength ;
		break ;

		case OS2:
		if (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth % 8 == 0) {
			linelength = bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8 ;
		}
		else {
			linelength = (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8) + 1;
		}
		retval = bmi_os2.bmch.bcHeight * linelength ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return retval ;
}

void BmpFile::embedBit (unsigned long pos, int value)
{
	assert (pos < getCapacity()) ;
	assert (value == 0 || value == 1) ;

	unsigned long row = 0, column = 0 ;
	calcRC (pos, &row, &column) ;
	bitmap[row][column] = (bitmap[row][column] & (unsigned char) ~0x01) | value ;
}

int BmpFile::extractBit (unsigned long pos) const
{
	assert (pos < getCapacity()) ;

	unsigned long row = 0, column = 0 ;
	calcRC (pos, &row, &column) ;
	return (bitmap[row][column] & 0x01) ;
}

void BmpFile::calcRC (unsigned long pos, unsigned long *row, unsigned long *column) const
{
	unsigned long width = 0 /* in bytes */, height = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth % 8 == 0) {
			width = bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8 ;
		}
		else {
			width = (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8) + 1;
		}
		height = bmi_win.bmih.biHeight ;
		break ;

		case OS2:
		if (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth % 8 == 0) {
			width = bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8 ;
		}
		else {
			width = (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8) + 1;
		}
		height = bmi_os2.bmch.bcHeight ;
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
		bmfh.bfSize = getBinIO()->read32_le() ;
		bmfh.bfReserved1 = getBinIO()->read16_le() ;
		bmfh.bfReserved2 = getBinIO()->read16_le() ;
		bmfh.bfOffBits = getBinIO()->read32_le() ;
		
		unsigned long tmpSize = getBinIO()->read32_le() ;
		switch (tmpSize) {
			case SizeBMINFOHEADER:	/* file has windows bmp format */
			subformat = WIN ;
			bmpwin_readheaders () ;
			break ;

			case SizeBMCOREHEADER:	/* file has OS/2 bmp format */
			subformat = OS2 ;
			bmpos2_readheaders () ;
			break ;

			default:
			if (getBinIO()->is_std()) {
				throw SteghideError (_("the bmp data from standard input has a format that is not supported.")) ;
			}
			else {
				throw SteghideError (_("the bmp file \"%s\" has a format that is not supported."), getBinIO()->getName().c_str()) ;
			}
			break ;
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the bmp headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading bmp headers."), getBinIO()->getName().c_str()) ;
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
	bmi_win.bmih.biSize = SizeBMINFOHEADER ;
	bmi_win.bmih.biWidth = getBinIO()->read32_le () ;
	bmi_win.bmih.biHeight = getBinIO()->read32_le () ;
	bmi_win.bmih.biPlanes = getBinIO()->read16_le () ;
	assert (bmi_win.bmih.biPlanes == 1) ;
	bmi_win.bmih.biBitCount = getBinIO()->read16_le () ;
	assert ((bmi_win.bmih.biBitCount == 1) ||
			(bmi_win.bmih.biBitCount == 4) ||
			(bmi_win.bmih.biBitCount == 8) || 
			(bmi_win.bmih.biBitCount == 24)) ;
	bmi_win.bmih.biCompression = getBinIO()->read32_le () ;
	if (bmi_win.bmih.biCompression != BI_RGB) {
		if (getBinIO()->is_std()) {
			throw SteghideError (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			throw SteghideError (_("the bitmap data in \"%s\" is compressed which is not supported."), getBinIO()->getName().c_str()) ;
		}
	}
	bmi_win.bmih.biSizeImage = getBinIO()->read32_le () ;
	bmi_win.bmih.biXPelsPerMeter = getBinIO()->read32_le () ;
	bmi_win.bmih.biYPelsPerMeter = getBinIO()->read32_le () ;
	bmi_win.bmih.biClrUsed = getBinIO()->read32_le () ;
	bmi_win.bmih.biClrImportant = getBinIO()->read32_le () ;

	if (bmi_win.bmih.biBitCount == 24) {
		bmi_win.colors.clear() ;
		bmi_win.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		switch (bmi_win.bmih.biBitCount) {
			case 1:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi_win.ncolors = 2 ;
			break ;

			case 4:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi_win.ncolors = 16 ;
			break ;

			case 8:
			bmi_win.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}
		if (bmi_win.bmih.biClrUsed != 0) {
			bmi_win.ncolors = bmi_win.bmih.biClrUsed ;
		}

		bmi_win.colors = vector<RGBQUAD> (bmi_win.ncolors) ;
		for (unsigned int i = 0 ; i < bmi_win.ncolors ; i++) {
			bmi_win.colors[i].rgbBlue = getBinIO()->read8() ;
			bmi_win.colors[i].rgbGreen = getBinIO()->read8() ;
			bmi_win.colors[i].rgbRed = getBinIO()->read8() ;
			if ((bmi_win.colors[i].rgbReserved = getBinIO()->read8()) != 0) {
				Warning w (_("maybe corrupted windows bmp data (Reserved in RGBQUAD is non-zero).")) ;
				w.printMessage() ;
			}
		}
	}

	return ;
}

void BmpFile::bmpos2_readheaders ()
{
	bmi_os2.bmch.bcSize = SizeBMCOREHEADER ;
	bmi_os2.bmch.bcWidth = getBinIO()->read16_le () ;
	bmi_os2.bmch.bcHeight = getBinIO()->read16_le () ;
	bmi_os2.bmch.bcPlanes = getBinIO()->read16_le () ;
	assert (bmi_os2.bmch.bcPlanes == 1) ;
	bmi_os2.bmch.bcBitCount = getBinIO()->read16_le () ;
	assert ((bmi_os2.bmch.bcBitCount == 1) ||
			(bmi_os2.bmch.bcBitCount == 4) ||
			(bmi_os2.bmch.bcBitCount == 8) || 
			(bmi_os2.bmch.bcBitCount == 24)) ;

	if (bmi_os2.bmch.bcBitCount == 24) {
		bmi_os2.colors.clear() ;
		bmi_os2.ncolors = 0 ;
	}
	else {
		/* a color table exists */
		switch (bmi_os2.bmch.bcBitCount) {
			case 1:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi_os2.ncolors = 2 ;
			break ;

			case 4:
			if (args->command.getValue() == EMBED) {
				Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
				w.printMessage() ;
			}
			bmi_os2.ncolors = 16 ;
			break ;

			case 8:
			bmi_os2.ncolors = 256 ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		bmi_os2.colors = vector<RGBTRIPLE> (bmi_os2.ncolors) ;
		for (unsigned int i = 0 ; i < bmi_os2.ncolors ; i++) {
			bmi_os2.colors[i].rgbtBlue = getBinIO()->read8() ;
			bmi_os2.colors[i].rgbtGreen = getBinIO()->read8() ;
			bmi_os2.colors[i].rgbtRed = getBinIO()->read8() ;
		}
	}

	return ;
}

/* writes the headers of a bmp file to disk */
void BmpFile::writeheaders ()
{
	try {
		getBinIO()->write16_le (bmfh.bfType) ;
		getBinIO()->write32_le (bmfh.bfSize) ;
		getBinIO()->write16_le (bmfh.bfReserved1) ;
		getBinIO()->write16_le (bmfh.bfReserved2) ;
		getBinIO()->write32_le (bmfh.bfOffBits) ;

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
			throw SteghideError (_("an error occured while writing the bmp headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the bmp headers to standard output.")) ;
			break ;
		}
	}
}

void BmpFile::bmpwin_writeheaders ()
{
	getBinIO()->write32_le (bmi_win.bmih.biSize) ;
	getBinIO()->write32_le (bmi_win.bmih.biWidth) ;
	getBinIO()->write32_le (bmi_win.bmih.biHeight) ;
	getBinIO()->write16_le (bmi_win.bmih.biPlanes) ;
	getBinIO()->write16_le (bmi_win.bmih.biBitCount) ;
	getBinIO()->write32_le (bmi_win.bmih.biCompression) ;
	getBinIO()->write32_le (bmi_win.bmih.biSizeImage) ;
	getBinIO()->write32_le (bmi_win.bmih.biXPelsPerMeter) ;
	getBinIO()->write32_le (bmi_win.bmih.biYPelsPerMeter) ;
	getBinIO()->write32_le (bmi_win.bmih.biClrUsed) ;
	getBinIO()->write32_le (bmi_win.bmih.biClrImportant) ;

	if (bmi_win.ncolors > 0) {
		for (unsigned int i = 0 ; i < bmi_win.ncolors ; i++) {
			getBinIO()->write8 (bmi_win.colors[i].rgbBlue) ;
			getBinIO()->write8 (bmi_win.colors[i].rgbGreen) ;
			getBinIO()->write8 (bmi_win.colors[i].rgbRed) ;
			getBinIO()->write8 (bmi_win.colors[i].rgbReserved) ;
		}
	}
}

void BmpFile::bmpos2_writeheaders ()
{
	getBinIO()->write32_le (bmi_os2.bmch.bcSize) ;
	getBinIO()->write16_le (bmi_os2.bmch.bcWidth) ;
	getBinIO()->write16_le (bmi_os2.bmch.bcHeight) ;
	getBinIO()->write16_le (bmi_os2.bmch.bcPlanes) ;
	getBinIO()->write16_le (bmi_os2.bmch.bcBitCount) ;

	if (bmi_os2.ncolors > 0) {
		for (unsigned int i = 0 ; i < bmi_os2.ncolors ; i++) {
			getBinIO()->write8 (bmi_os2.colors[i].rgbtBlue) ;
			getBinIO()->write8 (bmi_os2.colors[i].rgbtGreen) ;
			getBinIO()->write8 (bmi_os2.colors[i].rgbtRed) ;
		}
	}
}

/* returns the number of bytes used to store the pixel data of one scan line */
long BmpFile::calcLinelength ()
{
	long retval = 0 ;

	switch (getSubformat()) {
		case WIN:
		if (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth % 8 == 0) {
			retval = bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8 ;
		}
		else {
			retval = (bmi_win.bmih.biBitCount * bmi_win.bmih.biWidth / 8) + 1;
		}
		break ;

		case OS2:
		if (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth % 8 == 0) {
			retval = bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8 ;
		}
		else {
			retval = (bmi_os2.bmch.bcBitCount * bmi_os2.bmch.bcWidth / 8) + 1;
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
		retval = bmi_win.bmih.biHeight ;
		break ;

		case OS2:
		retval = bmi_os2.bmch.bcHeight ;
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

		bitmap = vector<vector<unsigned char> > (height) ;
		for (long line = height - 1 ; line >= 0 ; line--) {
			for (long posinline = 0 ; posinline < linelength ; posinline++) {
				bitmap[line].push_back (getBinIO()->read8()) ;
			}

			for (int i = 0 ; i < paddinglength ; i++) {
				if (getBinIO()->read8() != 0) {
					Warning w (_("maybe corrupted bmp data (padding byte set to non-zero).")) ;
					w.printMessage() ;
				}
			}
		}

		while (!getBinIO()->eof()) {
			atend.push_back (getBinIO()->read8()) ;
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			throw SteghideError (_("an error occured while reading the bmp data from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ;

			case BinaryInputError::FILE_EOF:
			throw SteghideError (_("premature end of file \"%s\" while reading bmp data."), getBinIO()->getName().c_str()) ;
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
			assert (bitmap[line].size() == (unsigned long) linelength) ;
			for (long posinline = 0 ; posinline < linelength ; posinline++) {
				getBinIO()->write8 (bitmap[line][posinline]) ;
			}

			for (unsigned int i = 0 ; i < paddinglength ; i++) {
				getBinIO()->write8 (0) ;
			}
		}

		for (vector<unsigned char>::iterator i = atend.begin() ; i != atend.end() ; i++) {
			getBinIO()->write8 (*i) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			throw SteghideError (_("an error occured while writing the bitmap data to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ;

			case BinaryOutputError::STDOUT_ERR:
			throw SteghideError (_("an error occured while writing the bitmap data to standard output.")) ;
			break ;
		}

	}
}
