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

#ifndef SH_BMPFILE_H
#define SH_BMPFILE_H

#include <vector>

#include "BinaryIO.h"
#include "ColorPalette.h"
#include "CvrStgFile.h"
#include "common.h"

class BmpFile : public CvrStgFile {
	public:
	BmpFile (void) ;
	BmpFile (BinaryIO *io) ;
	~BmpFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (SamplePos pos) const ;
	unsigned int getSamplesPerEBit (void) const ;

	unsigned short getBitCount (void) const ;
	unsigned long getWidth (void) const ;
	unsigned long getHeight (void) const ;
	ColorPalette *getPalette (void) const ;

	protected:
#ifdef WIN32
#include <windows.h>
#else
	typedef struct struct_BITMAPFILEHEADER {
		unsigned short	bfType ;
		unsigned long	bfSize ;
		unsigned short	bfReserved1 ;
		unsigned short	bfReserved2 ;
		unsigned long	bfOffBits ;
	} BITMAPFILEHEADER ;

	/* windows bmp file format */
	typedef struct struct_BITMAPINFOHEADER {
		unsigned long	biSize ;
		signed long		biWidth ;
		signed long		biHeight ;
		unsigned short	biPlanes ;
		unsigned short	biBitCount ;
		unsigned long	biCompression ;
		unsigned long	biSizeImage ;
		signed long		biXPelsPerMeter ;
		signed long		biYPelsPerMeter ;
		unsigned long	biClrUsed ;
		unsigned long	biClrImportant ;
	} BITMAPINFOHEADER ;

	/* os/2 bmp file format */
	typedef struct struct_BITMAPCOREHEADER {
		unsigned long	bcSize;
		unsigned short	bcWidth;
		unsigned short	bcHeight;
		unsigned short	bcPlanes;
		unsigned short	bcBitCount;
	} BITMAPCOREHEADER ;
#endif /* def WIN32 */

	private:
	static const unsigned int IdBm = 19778 ;
	static const unsigned short SizeBMFILEHEADER = 14 ;
	static const unsigned short SizeBMINFOHEADER = 40 ;
	static const unsigned short SizeBMCOREHEADER = 12 ;
	static const unsigned int COMPRESSION_BI_RGB = 0 ;

	enum SUBFORMAT { WIN, OS2 } ;

	SUBFORMAT subformat ;
	BITMAPFILEHEADER bmfh ;
	BITMAPINFOHEADER bmih ;
	BITMAPCOREHEADER bmch ;
	ColorPalette* Palette ;
	std::vector<std::vector <unsigned char> > bitmap ;
	/// contains bytes that are appended at the end of the bitmap data (some image editors apparently do this)
	std::vector<unsigned char> atend ;

	void readheaders () ;
	void bmpwin_readheaders () ;
	void bmpos2_readheaders () ;
	void writeheaders () ;
	void bmpwin_writeheaders () ;
	void bmpos2_writeheaders () ;
	void readdata () ;
	void writedata () ;
	void calcRCB (SamplePos pos, unsigned long *row, unsigned long *column, unsigned short *firstbit) const ;
	long calcLinelength () ;
	SUBFORMAT getSubformat (void) const ;
} ;

#endif /* ndef SH_BMPFILE_H */