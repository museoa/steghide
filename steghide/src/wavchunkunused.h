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

#ifndef SH_WAVCHUNKUNUSED_H
#define SH_WAVCHUNKUNUSED_H

#include "binaryio.h"
#include "wavchunk.h"
#include "wavchunkheader.h"

class WavChunkUnused : public WavChunk {
	public:
	WavChunkUnused (void) :
		WavChunk() {} ;
	WavChunkUnused (WavChunkHeader *chh) :
		WavChunk(chh) {} ;
	WavChunkUnused (WavChunkHeader *chh, BinaryIO *io) :
		WavChunk(chh) { read(io) ; } ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	private:
	vector<BYTE> Data ;
} ;

#endif // ndef SH_WAVCHUNKUNUSED_H
