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

#ifndef SH_CVRSTGFILE_H
#define SH_CVRSTGFILE_H

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

typedef struct struct_CVRSTGFILE {
	FILE *stream ;
	char *filename ;
	int fileformat ;
	void *contents ;
} CVRSTGFILE ;

/* constants that indicate the cover file format */
#define FF_UNKNOWN	0
#define FF_BMP		1
#define FF_WAV		2
#define FF_AU		3

/* function prototypes */
CVRSTGFILE *cvrstg_readfile (char *filename) ;
void cvrstg_writefile (CVRSTGFILE *file) ;
void cvrstg_transform (CVRSTGFILE *file, char *stgfilename) ;
unsigned long cvrstg_capacity (CVRSTGFILE *file) ;
void cvrstg_embedbit (CVRSTGFILE *file, unsigned long pos, int value) ;
int cvrstg_extractbit (CVRSTGFILE *file, unsigned long pos) ;
void cvrstg_cleanup (CVRSTGFILE *file) ;

#if 0
extern int noncvrbufuse[] ;

/* is used for parameter freesubstructs of cleanupcvrfile */
#define FSS_NO	0
#define FSS_YES	1


/*** general io ***/
/* CVRHEADERS contains the headers of all supported file formats */
typedef union union_CVRHEADERS {
	BMPHEADERS		bmp ;
	WAVHEADERS		wav ;
	AUHEADERS		au ;
} CVRHEADERS ;

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
#endif /* if 0 */

#endif /* ndef SH_CVRSTGFILE_H */
