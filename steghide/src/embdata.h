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

#ifndef SH_EMBDATA_H
#define SH_EMBDATA_H

#include "bitstring.h"
#include "encalgo.h"
#include "encmode.h"

class EmbData {
	public:
	enum MODE { EMBED, EXTRACT, UNDEF } ;
	enum STATE { READCRYPT, READLENOFNEMBBITS, READNEMBBITS, READENCRYPTED, END } ;
	EmbData (void) ;
	EmbData (MODE m, std::string fn = "") ;

	BitString getBitString (void) ;

	bool finished (void) ;
	unsigned long getNumBitsNeeded (void) ;
	void addBits (BitString bits) ;

	void setEncAlgo (EncryptionAlgorithm a) ;
	EncryptionAlgorithm getEncAlgo (void) const ;
	
	void setEncMode (EncryptionMode m) ;
	EncryptionMode getEncMode (void) const ;

	void setCompression (bool c) ;
	bool getCompression (void) const ;

	void setChecksum (bool c) ;
	bool getChecksum (void) const ;

	/**
	 * read the file FileName and put contents into Data
	 **/
	void read (void) ;

	/**
	 * write the contents of Data into FileName
	 **/
	void write (void) ;

	protected:
	std::string stripDir (std::string s) ;
	/**
	 * return minimal number of bits needed to save x
	 **/
	unsigned int nbits (unsigned long x) ;

	private:
	/// number of bits needed to code nbytes
	static const unsigned int NBitsLenOfNEmbBits = 5 ;
	/// number of bits need to code length of filename
	static const unsigned int NBitsLenOfFileName = 8 ;
	/// size of a crc32 checksum in bits
	static const unsigned int NBitsCrc32 = 32 ;
	/// maximum size of embedded file name
	static const unsigned int EmbFileNameMaxSize = 255 ;

	MODE Mode ;
	STATE State ;
	
	unsigned long NEmbBits ;
	unsigned long NumBitsNeeded ;

	EncryptionAlgorithm EncAlgo ;
	EncryptionMode EncMode ;
	bool Compression ;
	bool Checksum ;
	std::string FileName ;
	/// contains the actual message to be embedded
	std::vector<unsigned char> Data ;
} ;

#endif // ndef SH_EMBDATA_H
