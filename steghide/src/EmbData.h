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

#include <string>
#include <vector>

#include "common.h"

#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"

class BitString ;

class EmbData {
	public:
	enum MODE { EMBED, EXTRACT } ;
	enum STATE { READ_ENCINFO, READ_NPLAINBITS, READ_ENCRYPTED, END } ;

	/**
	 * construct a new EmbData object
	 * \param m the mode (EMBED or EXTRACT)
	 * \param pp the passphrase
	 * \param fn the filename (only need for mode EMBED)
	 **/
	EmbData (MODE m, std::string pp, std::string fn = "") ;

	BitString getBitString (void) ;

	bool finished (void) ;
	unsigned long getNumBitsNeeded (void) ;
	void addBits (BitString bits) ;

	void setEncAlgo (EncryptionAlgorithm a) ;
	EncryptionAlgorithm getEncAlgo (void) const ;
	
	void setEncMode (EncryptionMode m) ;
	EncryptionMode getEncMode (void) const ;

	void setCompression (int c) ;
	int getCompression (void) const ;

	void setChecksum (bool c) ;
	bool getChecksum (void) const ;

	void setData (const std::vector<BYTE> data)
		{ Data = data ; } ;

	std::vector<BYTE> getData (void) const
		{ return Data ; } ;

	std::string getFileName (void) const
		{ return FileName ; } ;

	/// the minimum size of the part of the generatred BitString that is not the data
	static const unsigned int MinStegoHeaderSize = 50 ;

	protected:
	std::string stripDir (std::string s) ;

	private:
	/// number of bits used to code the number of plain bits
	static const unsigned int NBitsNPlainBits = 32 ;
	/// number of bits used to code the number of uncompressed bits
	static const unsigned int NBitsNUncompressedBits = 32 ;
	/// size of a crc32 checksum in bits
	static const unsigned int NBitsCrc32 = 32 ;

	MODE Mode ;
	STATE State ;
	
	unsigned long NPlainBits ;

	unsigned long NumBitsNeeded ;

	std::string Passphrase ;

	EncryptionAlgorithm EncAlgo ;
	EncryptionMode EncMode ;
	/// compression level: 0(none),1(best speed),...,9(best compression)
	int Compression ;
	/// will a checksum be embedded ?
	bool Checksum ;
	std::string FileName ;
	/// contains the actual message to be embedded
	std::vector<BYTE> Data ;
} ;

#endif // ndef SH_EMBDATA_H
