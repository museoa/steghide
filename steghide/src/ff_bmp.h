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

#ifndef SH_FF_BMP_H
#define SH_FF_BMP_H

#include "cvrstgfile.h"

/*** bmp file format ***/
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

typedef struct struct_RGBQUAD {
	unsigned char	rgbBlue ;
	unsigned char	rgbGreen ;
	unsigned char	rgbRed ;
	unsigned char	rgbReserved ;
} RGBQUAD ;

/* os/2 bmp file format */
typedef struct struct_BITMAPCOREHEADER {
    unsigned long	bcSize;
    unsigned short	bcWidth;
    unsigned short	bcHeight;
    unsigned short	bcPlanes;
    unsigned short	bcBitCount;
} BITMAPCOREHEADER ;

typedef struct struct_RGBTRIPLE {
	unsigned char	rgbtBlue ;
	unsigned char	rgbtGreen ;
	unsigned char	rgbtRed ;
} RGBTRIPLE ;
#endif /* def WIN32 */

typedef struct struct_BMPINFO_WIN {
	BITMAPINFOHEADER	bmih ;
	unsigned int		ncolors ;
	RGBQUAD				*colors ;
} BMPINFO_WIN ;

typedef struct struct_BMPINFO_OS2 {
	BITMAPCOREHEADER	bmch ;
	unsigned int		ncolors ;
	RGBTRIPLE			*colors ;
} BMPINFO_OS2 ;

typedef union union_BMPINFO {
	BMPINFO_WIN	win ;
	BMPINFO_OS2	os2 ;
} BMPINFO ;

typedef struct struct_BMP_CONTENTS {
	BITMAPFILEHEADER	bmfh ;
	BMPINFO				bmi ;
	unsigned char		**bitmap ;
} BMP_CONTENTS ;

#define BMP_SIZE_BMFILEHEADER	14
#define BMP_SIZE_BMINFOHEADER	40
#define BMP_SIZE_BMCOREHEADER	12
#define BMP_ID_BM				19778
#define BMP_BI_RGB				0

/* function prototypes */
void bmp_readfile (CVRSTGFILE *file) ;
void bmp_writefile (CVRSTGFILE *file) ;
unsigned long bmp_capacity (CVRSTGFILE *file) ;
void bmp_embedbit (CVRSTGFILE *file, unsigned long pos, int value) ;
int bmp_extractbit (CVRSTGFILE *file, unsigned long pos) ;
void bmp_cleanup (CVRSTGFILE *file) ;

#endif /* ndef SH_FF_BMP_H */
