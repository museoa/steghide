/*
 * steghide 0.5.1 - a steganography program
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

#ifndef SH_WAVFILE_H
#define SH_WAVFILE_H

#include <vector>

#include "binaryio.h"
#include "cvrstgfile.h"

class WavFile : public CvrStgFile {
	public:
	WavFile (void) ;
	WavFile (BinaryIO *io) ;
	~WavFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	unsigned long getNumSamples (void) ;
	void replaceSample (SamplePos pos, CvrStgSample *s) ;
	CvrStgSample* getSample (SamplePos pos) ;
	unsigned int getSamplesPerEBit (void) ;

	unsigned short getBitsPerSample (void) ;

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
	/// this vector contains the wav data if BitsPerSample <= 8
	vector<unsigned char> data_small ;
	/// this vector contains the wav data if BitsPerSample >8
	vector<int> data_large ;	// it is assumed that an int can hold 32 bits
	
	UnsupChunks	unsupchunks2 ;

	void readheaders (void) ;
	void readdata (void) ;
	void writeheaders (void) ;
	void writedata (void) ;
	void calcpos (SamplePos n, unsigned long *bytepos, unsigned short *firstbitpos) const ;
	ChunkHeader *getChhdr (void) ;
	void putChhdr (ChunkHeader *chhdr) ;
	void *s_realloc (void *ptr, size_t size) ;
	void cp32ul2uc_le (unsigned char *dest, unsigned long src) ;
	/**
	 * get the position of the first bit (of the first byte) containing the actual sample data
	 * \return the bit position (where 0 is the lsb and 7 the msb)
	 **/
	unsigned short getFirstBitPosinSample (void) ;
	unsigned short getBytesPerSample (void) ;
} ;

#endif // ndef SH_WAVFILE_H
