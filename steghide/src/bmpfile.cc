/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>
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

#include <cstdio>
#include <cstdlib>

#include "colorpalette.h"
#include "common.h"
#include "cvrstgfile.h"
#include "bmpfile.h"
#include "bmpsamplevalue.h"
#include "error.h"

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
	if (Palette != NULL) {
		delete Palette ;
	}
}

BmpFile::SUBFORMAT BmpFile::getSubformat () const
{
	return subformat ;
}

void BmpFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;
	Palette = NULL ;

	readheaders() ;
	readdata() ;
}

void BmpFile::write ()
{
	CvrStgFile::write() ;

	writeheaders() ;
	writedata() ;
}

unsigned long BmpFile::getNumSamples() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biWidth * bmih.biHeight ;
		break ; }

		case OS2: {
			retval = bmch.bcWidth * bmch.bcHeight ;
		break ; }
	}
	return retval ;
}

void BmpFile::replaceSample (SamplePos pos, SampleValue *s)
{
	unsigned long row = 0, column = 0 ;
	unsigned short firstbit = 0 ;
	calcRCB (pos, &row, &column, &firstbit) ;

	unsigned short bitcount = getBitCount() ;
	switch (bitcount) {
		case 1: case 4: case 8: {
			BmpPaletteSampleValue* sample = dynamic_cast<BmpPaletteSampleValue*> (s) ;
			assert (sample != NULL) ;

			for (unsigned short i = 0 ; i < bitcount ; i++) {
				bitmap[row][column] = bitmap[row][column] & (~(1 << (firstbit + i))) ;
				bitmap[row][column] = bitmap[row][column] | ((sample->getIndex() & (1 << i)) << firstbit) ;
			}
		break ; }

		case 24: {
			BmpRGBSampleValue* sample = dynamic_cast<BmpRGBSampleValue*> (s) ;
			assert (sample != NULL) ;

			bitmap[row][column] = sample->getRed() ;
			bitmap[row][column + 1] = sample->getGreen() ;
			bitmap[row][column + 2] = sample->getBlue() ;
		break ; }
	}
}

unsigned int BmpFile::getSamplesPerEBit() const
{
	unsigned int retval = 0 ;
	switch (getBitCount()) {
		case 1: case 4: case 8: {
			retval = 2 ;
		break ; }

		case 24: {
			retval = 1 ;
		break ; }
	}
	return retval ;
}

SampleValue *BmpFile::getSampleValue (SamplePos pos) const
{
	unsigned long row = 0, column = 0 ;
	unsigned short firstbit = 0 ;
	calcRCB (pos, &row, &column, &firstbit) ;

	unsigned short bitcount = getBitCount() ;
	SampleValue *retval = NULL ;
	switch (bitcount) {
		case 1: case 4: case 8: {
			unsigned char index = 0 ;
			for (unsigned short i = 0 ; i < bitcount ; i++) {
				index |= ((bitmap[row][column] & (1 << (firstbit + i))) >> firstbit) ;
			}
			retval = (SampleValue*) new BmpPaletteSampleValue (this, index) ;
		break ; }

		case 24: {
			retval = (SampleValue*) new BmpRGBSampleValue (this, bitmap[row][column], bitmap[row][column + 1], bitmap[row][column + 2]) ;
		break ; }
	}
	return retval ;
}

void BmpFile::calcRCB (SamplePos pos, unsigned long *row, unsigned long *column, unsigned short *firstbit) const
{
	unsigned long width = getWidth(), height = getHeight(), bitcount = getBitCount() ;

	*row = pos / width ;
	assert (*row < height) ;

	pos = pos % width ;	// now searching for pos in a single row
	if (bitcount >= 8) {
		unsigned short bytespersample = bitcount / 8 ;
		
		*column = pos * bytespersample ;
		assert (*column < (width * bytespersample)) ;
		
		*firstbit = 0 ;
	}
	else {
		unsigned short samplesperbyte = 8 / bitcount ;

		*column = pos / samplesperbyte ;
		unsigned long bytesperline = 0 ;
		if ((width % samplesperbyte) == 0) {
			bytesperline = width / samplesperbyte ;
		}
		else {
			bytesperline = (width / samplesperbyte) + 1 ;
		}
		assert (*column < bytesperline) ;

		*firstbit = (pos % samplesperbyte) * bitcount ;
		assert (*firstbit < 8) ;
	}
}

unsigned short BmpFile::getBitCount() const
{
	unsigned short retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biBitCount ;
		break ; }

		case OS2: {
			retval = bmch.bcBitCount ;
		break ; }
	}
	assert (retval == 1 || retval == 4 || retval == 8 || retval == 24) ;
	return retval ;
}

unsigned long BmpFile::getWidth() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN:
			retval = bmih.biWidth ;
		break ;

		case OS2:
			retval = bmch.bcWidth ;
		break ;
	}
	return retval ;
}

unsigned long BmpFile::getHeight() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biHeight ;
		break ; }

		case OS2: {
			retval = bmch.bcHeight ;
		break ; }
	}

	return retval ;
}

ColorPalette *BmpFile::getPalette() const
{
	assert (getBitCount() != 24) ;
	assert (Palette != NULL) ;
	return Palette ;
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
			case SizeBMINFOHEADER: {
				// this file is in the Windows bmp format
				subformat = WIN ;
				bmpwin_readheaders () ;
			break ; }

			case SizeBMCOREHEADER: {
				// this file is in the OS/2 bmp format
				subformat = OS2 ;
				bmpos2_readheaders () ;
			break ; }

			default: {
				if (getBinIO()->is_std()) {
					throw NotImplementedError (_("the bmp data from standard input has a format that is not supported (biSize: %lu)."), tmpSize) ;
				}
				else {
					throw NotImplementedError (_("the bmp file \"%s\" has a format that is not supported (biSize: %lu)."), getBinIO()->getName().c_str(), tmpSize) ;
				}
			break ; }
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR: {
				throw SteghideError (_("an error occured while reading the bmp headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::FILE_EOF: {
				throw SteghideError (_("premature end of file \"%s\" while reading bmp headers."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::STDIN_ERR: {
				throw SteghideError (_("an error occured while reading the bmp headers from standard input.")) ;
			break ; }

			case BinaryInputError::STDIN_EOF: {
				throw SteghideError (_("premature end of data from standard input while reading bmp headers.")) ;
			break ; }
		}
	}
}

void BmpFile::bmpwin_readheaders ()
{
	bmih.biSize = SizeBMINFOHEADER ;
	bmih.biWidth = getBinIO()->read32_le () ;
	bmih.biHeight = getBinIO()->read32_le () ;
	bmih.biPlanes = getBinIO()->read16_le () ;
	assert (bmih.biPlanes == 1) ;
	bmih.biBitCount = getBinIO()->read16_le () ;
	if ((bmih.biBitCount != 1) && (bmih.biBitCount != 4) && (bmih.biBitCount != 8) && (bmih.biBitCount != 24)) {
		if (getBinIO()->is_std()) {
			throw NotImplementedError (_("the bmp data from standard input has a format that is not supported (biBitCount: %d)."), bmih.biBitCount) ;
		}
		else {
			throw NotImplementedError (_("the bmp file \"%s\" has a format that is not supported (biBitCount: %d)."), getBinIO()->getName().c_str(), bmih.biBitCount) ;
		}
	}
	bmih.biCompression = getBinIO()->read32_le () ;
	if (bmih.biCompression != COMPRESSION_BI_RGB) {
		if (getBinIO()->is_std()) {
			throw NotImplementedError (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			throw NotImplementedError (_("the bitmap data in \"%s\" is compressed which is not supported."), getBinIO()->getName().c_str()) ;
		}
	}
	bmih.biSizeImage = getBinIO()->read32_le () ;
	bmih.biXPelsPerMeter = getBinIO()->read32_le () ;
	bmih.biYPelsPerMeter = getBinIO()->read32_le () ;
	bmih.biClrUsed = getBinIO()->read32_le () ;
	bmih.biClrImportant = getBinIO()->read32_le () ;

	if (bmih.biBitCount != 24) {
		/* a color table exists */
		Palette = new ColorPalette() ;
		unsigned int ncolors = 0 ;
		switch (bmih.biBitCount) {
			case 1: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 2 ;
			break ; }

			case 4: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 16 ;
			break ; }

			case 8: {
				ncolors = 256 ;
			break ; }

			default: {
				assert (0) ;
			break ; }
		}
		if (bmih.biClrUsed != 0) {
			ncolors = bmih.biClrUsed ;
		}

		for (unsigned int i = 0 ; i < ncolors ; i++) {
			unsigned char b = getBinIO()->read8() ;
			unsigned char g = getBinIO()->read8() ;
			unsigned char r = getBinIO()->read8() ;
			if (getBinIO()->read8() != 0) {
				Warning w (_("maybe corrupted windows bmp data (Reserved in RGBQUAD is non-zero).")) ;
				w.printMessage() ;
			}
			Palette->addEntry (r, g, b) ;
		}
	}
}

void BmpFile::bmpos2_readheaders ()
{
	bmch.bcSize = SizeBMCOREHEADER ;
	bmch.bcWidth = getBinIO()->read16_le () ;
	bmch.bcHeight = getBinIO()->read16_le () ;
	bmch.bcPlanes = getBinIO()->read16_le () ;
	assert (bmch.bcPlanes == 1) ;
	bmch.bcBitCount = getBinIO()->read16_le () ;
	assert ((bmch.bcBitCount == 1) ||
			(bmch.bcBitCount == 4) ||
			(bmch.bcBitCount == 8) || 
			(bmch.bcBitCount == 24)) ;

	if (bmch.bcBitCount != 24) {
		/* a color table exists */
		unsigned int ncolors = 0 ;
		Palette = new ColorPalette() ;
		switch (bmch.bcBitCount) {
			case 1: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 2 ;
			break ; }

			case 4: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 16 ;
			break ; }

			case 8: {
				ncolors = 256 ;
			break ; }

			default: {
				assert (0) ;
			break ; }
		}

		for (unsigned int i = 0 ; i < ncolors ; i++) {
			unsigned char b = getBinIO()->read8() ;
			unsigned char g = getBinIO()->read8() ;
			unsigned char r = getBinIO()->read8() ;
			Palette->addEntry (r, g, b) ;
		}
	}
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
			case WIN: {
				bmpwin_writeheaders() ;
			break ; }

			case OS2: {
				bmpos2_writeheaders() ;
			break ; }

			default: {
				assert (0) ;
			break ; }
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR: {
				throw SteghideError (_("an error occured while writing the bmp headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryOutputError::STDOUT_ERR: {
				throw SteghideError (_("an error occured while writing the bmp headers to standard output.")) ;
			break ; }
		}
	}
}

void BmpFile::bmpwin_writeheaders ()
{
	getBinIO()->write32_le (bmih.biSize) ;
	getBinIO()->write32_le (bmih.biWidth) ;
	getBinIO()->write32_le (bmih.biHeight) ;
	getBinIO()->write16_le (bmih.biPlanes) ;
	getBinIO()->write16_le (bmih.biBitCount) ;
	getBinIO()->write32_le (bmih.biCompression) ;
	getBinIO()->write32_le (bmih.biSizeImage) ;
	getBinIO()->write32_le (bmih.biXPelsPerMeter) ;
	getBinIO()->write32_le (bmih.biYPelsPerMeter) ;
	getBinIO()->write32_le (bmih.biClrUsed) ;
	getBinIO()->write32_le (bmih.biClrImportant) ;

	if (Palette != NULL) {
		for (unsigned int i = 0 ; i < Palette->getSize() ; i++) {
			getBinIO()->write8 (Palette->getBlue(i)) ;
			getBinIO()->write8 (Palette->getGreen(i)) ;
			getBinIO()->write8 (Palette->getRed(i)) ;
			getBinIO()->write8 (0) ;
		}
	}
}

void BmpFile::bmpos2_writeheaders ()
{
	getBinIO()->write32_le (bmch.bcSize) ;
	getBinIO()->write16_le (bmch.bcWidth) ;
	getBinIO()->write16_le (bmch.bcHeight) ;
	getBinIO()->write16_le (bmch.bcPlanes) ;
	getBinIO()->write16_le (bmch.bcBitCount) ;

	if (Palette != NULL) {
		for (unsigned int i = 0 ; i < Palette->getSize() ; i++) {
			getBinIO()->write8 (Palette->getBlue(i)) ;
			getBinIO()->write8 (Palette->getGreen(i)) ;
			getBinIO()->write8 (Palette->getRed(i)) ;
		}
	}
}

/* returns the number of bytes used to store the pixel data of one scan line */
long BmpFile::calcLinelength ()
{
	long retval = 0 ;

	switch (getSubformat()) {
		case WIN: {
			if (bmih.biBitCount * bmih.biWidth % 8 == 0) {
				retval = bmih.biBitCount * bmih.biWidth / 8 ;
			}
			else {
				retval = (bmih.biBitCount * bmih.biWidth / 8) + 1;
			}
		break ; }

		case OS2: {
			if (bmch.bcBitCount * bmch.bcWidth % 8 == 0) {
				retval = bmch.bcBitCount * bmch.bcWidth / 8 ;
			}
			else {
				retval = (bmch.bcBitCount * bmch.bcWidth / 8) + 1;
			}
		break ; }
		
		default: {
			assert (0) ;
		break ; }
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

		bitmap = std::vector<std::vector<unsigned char> > (height) ;
		for (long line = height - 1 ; line >= 0 ; line--) {
			for (long posinline = 0 ; posinline < linelength ; posinline++) {
				bitmap[line].push_back (getBinIO()->read8()) ;
			}

			for (int i = 0 ; i < paddinglength ; i++) {
				if (getBinIO()->read8() != 0) {
					Warning w (_("maybe corrupted bmp data (padding byte at 0x%lx set to non-zero)."), getBinIO()->getPos() - 1) ;
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
			case BinaryInputError::FILE_ERR: {
				throw SteghideError (_("an error occured while reading the bmp data from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::FILE_EOF: {
				throw SteghideError (_("premature end of file \"%s\" while reading bmp data."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::STDIN_ERR: {
				throw SteghideError (_("an error occured while reading the bmp data from standard input.")) ;
			break ; }

			case BinaryInputError::STDIN_EOF: {
				throw SteghideError (_("premature end of bmp data from standard input.")) ;
			break ; }
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

		for (std::vector<unsigned char>::iterator i = atend.begin() ; i != atend.end() ; i++) {
			getBinIO()->write8 (*i) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR: {
				throw SteghideError (_("an error occured while writing the bitmap data to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryOutputError::STDOUT_ERR: {
				throw SteghideError (_("an error occured while writing the bitmap data to standard output.")) ;
			break ; }
		}
	}
}
