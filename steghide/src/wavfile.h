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

#include "binaryio.h"
#include "bufmanag.h"
#include "cvrstgfile.h"

class WavFile : public CvrStgFile {
	public:
	WavFile (void) ;
	WavFile (BinaryIO *io) ;
	~WavFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;
	unsigned long getCapacity (void) ;
	void embedBit (unsigned long pos, int bit) ;
	int extractBit (unsigned long pos) ;

	private:
	static const signed short	FormatPCM = 1 ;
	static const unsigned short	SizeFormatChunk = 16 ;
	static const unsigned short	SizeChunkHeader = 8 ;

	typedef struct struct_ChunkHeader {
		char			id[4] ;
		unsigned long	len ;
	} ChunkHeader ;

	typedef struct struct_PCMFormatChunk {
		signed short	FormatTag ;
		unsigned short	Channels ;
		unsigned long	SamplesPerSec ;
		unsigned long	AvgBytesPerSec ;
		unsigned short	BlockAlign ;
		unsigned short	BitsPerSample ;
	} PCMFormatChunk ;

	typedef struct struct_UnsupChunks {
		int		len ;
		void	*data ;
	} UnsupChunks ;

	ChunkHeader riffchhdr ;
	char id_wave[4] ;
	ChunkHeader fmtchhdr ;
	PCMFormatChunk fmtch ;
	UnsupChunks unsupchunks1 ;
	ChunkHeader datachhdr ;
	BUFFER *data ;
	UnsupChunks	unsupchunks2 ;

	void readheaders (void) ;
	void readdata (void) ;
	void writeheaders (void) ;
	void writedata (void) ;
	void calcpos (unsigned long n, unsigned long *bytepos, unsigned int *bitpos) ;
	ChunkHeader *getChhdr (void) ;
	void putChhdr (ChunkHeader *chhdr) ;
} ;

#endif /* ndef SH_FF_WAV_H */
