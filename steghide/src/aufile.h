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

#ifndef SH_AUFILE_H
#define SH_AUFILE_H

#include <vector>

#include "binaryio.h"
#include "cvrstgfile.h"

class AuFile : public CvrStgFile {
	public:
	AuFile (void) ;
	AuFile (BinaryIO *io) ;
	~AuFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	unsigned long getNumSamples (void) ;
	void replaceSample (SamplePos pos, CvrStgSample *s) ;
	CvrStgSample* getSample (SamplePos pos) ;
	unsigned int getSamplesPerEBit (void) ;

	protected:
	typedef struct struct_AuHeader {
		char			id[4] ;
		unsigned long	offset ;
		unsigned long	size ;
		unsigned long	encoding ;
		unsigned long	samplerate ;
		unsigned long	channels ;
	} AuHeader ;

	vector<unsigned char> getData (void) ;
	void setData (vector<unsigned char> d) ;

	private:
	static const int HeaderSize = 24 ;

	AuHeader header ;
	unsigned long len_infofield ;
	vector<unsigned char> infofield ;
	vector<unsigned char> data ;

	void readheaders (void) ;
	void readdata (void) ;
	void writeheaders (void) ;
	void writedata (void) ;
} ;

#endif /* ndef SH_AUFILE_H */
