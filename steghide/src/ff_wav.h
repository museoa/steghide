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

#ifndef SH_FF_WAV_H
#define SH_FF_WAV_H

#include "cvrstgfile.h"

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

typedef struct struct_UNSUPCHUNKS {
	int		len ;
	void	*data ;
} UNSUPCHUNKS ;

typedef struct struct_WAV_CONTENTS {
	CHUNKHEADER		riffchhdr ;
	char			id_wave[4] ;
	CHUNKHEADER		fmtchhdr ;
	FMTCHUNK_PCM	fmtch ;
	UNSUPCHUNKS		unsupchunks1 ;
	CHUNKHEADER		datachhdr ;
	BUFFER			*data ;
	UNSUPCHUNKS		unsupchunks2 ;
} WAV_CONTENTS ;

#define WAV_FORMAT_PCM		1
#define WAV_SIZE_FMTCHUNK	16

/* function prototypes */
void wav_readfile (CVRSTGFILE *file) ;
void wav_writefile (CVRSTGFILE *file) ;
unsigned long wav_capacity (CVRSTGFILE *file) ;
void wav_embedbit (CVRSTGFILE *file, unsigned long pos, int value) ;
int wav_extractbit (CVRSTGFILE *file, unsigned long pos) ;
void wav_cleanup (CVRSTGFILE *file) ;

#endif /* ndef SH_FF_WAV_H */
