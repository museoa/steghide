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

#ifndef SH_IO_H
#define SH_IO_H

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

extern int noncvrbufuse[] ;

/* is used for parameter freesubstructs of cleanupcvrfile */
#define FSS_NO	0
#define FSS_YES	1

/*** au file format ***/
typedef struct struct_AUHEADERS {
	char			id[4] ;
	unsigned long	offset ;
	unsigned long	size ;
	unsigned long	encoding ;
	unsigned long	samplerate ;
	unsigned long	channels ;
} AUHEADERS ;

#define AU_SIZE_HEADER		24


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
#endif /* WIN32 */

typedef struct struct_BITMAPXHEADER { /* stores data from BITMAPINFO- and BITMAPCORE- HEADER */
	unsigned long	Size ;
	signed long		Width ;
	signed long		Height ;
	unsigned short	Planes ;
	unsigned short	BitCount ;
	unsigned long	Compression ;
	unsigned long	SizeImage ;
	signed long		XPelsPerMeter ;
	signed long		YPelsPerMeter ;
	unsigned long	ClrUsed ;
	unsigned long	ClrImportant ;
} BITMAPXHEADER ;

typedef struct struct_BMPHEADERS {
	BITMAPFILEHEADER	bmfh ;
	BITMAPXHEADER		bmxh ;
} BMPHEADERS ;

#define BMP_SIZE_BMFILEHEADER	14
#define BMP_SIZE_BMINFOHEADER	40
#define BMP_SIZE_BMCOREHEADER	12
#define BMP_ID_BM				19778
#define BMP_BI_RGB				0


/*** wav file format ***/
typedef struct struct_CHUNKHEADER {
	char			id[4] ;
	unsigned long	len ;
} CHUNKHEADER ;

typedef struct struct_FMTCHUNK_PCM {
	signed short	FormatTag ;
	unsigned short	Channels ;
	unsigned long	SamplesPerSec ;
	unsigned long	AvgBytesPerSec ;
	unsigned short	BlockAlign ;
	unsigned short	BitsPerSample ;
} FMTCHUNK_PCM ;

typedef struct struct_WAVHEADERS {
	CHUNKHEADER		riffchhdr ;
	char			id_wave[4] ;
	CHUNKHEADER		fmtchhdr ;
	FMTCHUNK_PCM	fmtch ;
	CHUNKHEADER		datachhdr ;
} WAVHEADERS ;

#define WAV_FORMAT_PCM		1
#define WAV_SIZE_FMTCHUNK	16


/*** general io ***/
/* CVRHEADERS contains the headers of all supported file formats */
typedef union union_CVRHEADERS {
	BMPHEADERS		bmp ;
	WAVHEADERS		wav ;
	AUHEADERS		au ;
} CVRHEADERS ;

/* constants that indicate the cover file format */
#define FF_UNKNOWN	0
#define FF_BMP		1
#define FF_WAV		2
#define FF_AU		3

/* CVRFILE describes a coverdata file */
#include "bufmanag.h"
typedef struct struct_CVRFILE {
	/* the file's data stream */
	FILE		*stream ;
	/* name of the file */
	char		*filename ;
	/* fileformat of the file (one of the FF_* constants) */
	int			fileformat ;
	/* headers of the file */
	CVRHEADERS	*headers ;
	/* pointer to data of unsupported header structures (legal in fileformat,
	   but not needed by steghide) between start of file and "main data" */
	void		*unsupdata1 ;
	/* length of data of unsupported header structures 1 (in bytes) */
	int			unsupdata1len ;
	/* pointer to data of unsupported header structures between "main data" and end of file */
	void		*unsupdata2 ;
	/* length of data of unsupported header structures 2 (in bytes) */
	int			unsupdata2len ;
	/* first element of linked list of buffers that contain the coverbytes of the file */
	BUFFER		*cvrdata ;
	/* first element of linked list of buffers that contain all other bytes of the file
	   these buffers contain all bytes in which data must not be hidden */
	BUFFER		*noncvrdata ;
} CVRFILE ;

/* PLNFILE describes a plaindata file */
typedef struct struct_PLNFILE {	
	/* file as data stream */
	FILE	*stream ;
	/* name of file */
	char	*filename ;
	/* first element of linked list of buffers that contain the data of the file */
	BUFFER	*plndata ;
} PLNFILE ;

/* general io function prototypes */
void readheaders (CVRFILE *file) ;
CVRFILE *createstgfile(CVRFILE *cvrfile, char *stgfilename) ;
PLNFILE *createplnfile(void) ;
void assemble_plndata (PLNFILE *plnfile) ;
void deassemble_plndata (PLNFILE *plnfile) ;
void cleanupcvrfile(CVRFILE *cvrfile, int freesubstructs) ;
void cleanupplnfile(PLNFILE *plnfile) ;
CVRFILE *readcvrfile(char *filename) ;
void writestgfile(CVRFILE *cvrfile) ;
PLNFILE *readplnfile(char *filename) ;
void writeplnfile(PLNFILE *plnfile) ;

/* au file format function prototypes */
void au_readheaders (CVRFILE *file) ;
void au_writeheaders (CVRFILE *file) ;
void au_readfile (CVRFILE *file) ;
void au_writefile (CVRFILE *file) ;

/* bmp file format function prototypes */
void bmp_readheaders (CVRFILE *file) ;
void bmp_writeheaders (CVRFILE *file) ;
void bmp_readfile (CVRFILE *file) ;
void bmp_writefile (CVRFILE *file) ;

/* wav file format function prototypes */
void wav_readheaders (CVRFILE *file, unsigned long rifflen) ;
void wav_writeheaders (CVRFILE *file) ;
void wav_readfile (CVRFILE *file) ;
void wav_writefile (CVRFILE *file) ;

#endif /* ndef SH_IO_H */
