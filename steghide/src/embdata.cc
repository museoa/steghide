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

#include "common.h"
#include "binaryio.h"
#include "embdata.h"
#include "error.h"
#include "mcryptpp.h"
#include "mhashpp.h"

EmbData::EmbData ()
{
	Mode = UNDEF ;
}

EmbData::EmbData (MODE m, string fn)
{
	Mode = m ;
	FileName = fn ;

	switch (Mode) {
		case EMBED: {
			read() ;
		break ; }

		case EXTRACT: {
			NumBitsNeeded = MCryptpp::sizeAlgorithm + MCryptpp::sizeMode ;
			State = READCRYPT ;
		break ; }

		default: {
			assert (0) ;
		break ; }
	}
}

bool EmbData::finished ()
{
	assert (Mode == EXTRACT) ;
	return (State == END) ;
}

unsigned long EmbData::getNumBitsNeeded ()
{
	assert (Mode == EXTRACT) ;
	return NumBitsNeeded ;
}

void EmbData::addBits (BitString bits)
{
	assert (Mode == EXTRACT) ;
	assert (bits.getLength() == NumBitsNeeded) ;

	switch (State) {
		case READCRYPT: {
			CryptAlgo = (MCryptpp::Algorithm) bits.getValue (0, MCryptpp::sizeAlgorithm) ;
			CryptMode = (MCryptpp::Mode) bits.getValue (MCryptpp::sizeAlgorithm, MCryptpp::sizeMode) ;
			NumBitsNeeded = NBitsLenOfNEmbBits ;
			State = READLENOFNEMBBITS ;
		break ; }

		case READLENOFNEMBBITS: {
			NumBitsNeeded = bits.getValue (0, NBitsLenOfNEmbBits) ;
			State = READNEMBBITS ;
		break ; }

		case READNEMBBITS: {
			NEmbBits = bits.getValue (0, NumBitsNeeded) ;
			NumBitsNeeded = MCryptpp::getEncryptedSize (CryptAlgo, CryptMode, NEmbBits) ;
			State = READENCRYPTED ;
		break ; }

		case READENCRYPTED: {
			BitString decrypted ;
			if (CryptAlgo == MCryptpp::NONE) {
				decrypted = bits ;
			}
			else {
				MCryptpp crypto (CryptAlgo, CryptMode) ;
				decrypted = crypto.decrypt (bits, Args.Passphrase.getValue()) ;
			}
			// TODO - zlib inflate here - decrypted becomes plain

			unsigned long pos = 0 ;

			Compression = (bool) decrypted[pos++] ;
			if (Compression) {
				throw SteghideError (_("the embedded data is compressed. this is not implemented in this version (file corruption ?).")) ;
			}

			Checksum = (bool) decrypted[pos++] ;
			unsigned long extcrc32 = 0 ;
			if (Checksum) {
				extcrc32 = decrypted.getValue (pos, NBitsCrc32) ;
				pos += NBitsCrc32 ;
			}

			unsigned int lenoffilename = (unsigned int) decrypted.getValue (pos, NBitsLenOfFileName) ;
			pos += 8 ;
			string filename = "" ;
			for (unsigned int i = 0 ; i < lenoffilename ; i++) {
				filename += (char) decrypted.getValue (pos, 8) ;
				pos += 8 ;
			}

			if (Args.ExtFn.is_set()) {
				if (Args.ExtFn.getValue() == "") {
					// write extracted data to stdout
					FileName = "" ;
				}
				else {
					// file name given by extracting user overrides embedded file name
					FileName = Args.ExtFn.getValue() ;
				}
			}
			else {
				// write extracted data to file with embedded file name
				assert (Args.ExtFn.getValue() == "") ;
				if (lenoffilename == 0) {
					throw SteghideError (_("please specify a file name for the extracted data (there is no name embedded in the stego file).")) ;
				}
				FileName = filename ;
			}

			// TODO - hier einige assertions
			assert ((NEmbBits - pos) % 8 == 0) ;

			unsigned long extdatalen = (NEmbBits - pos) / 8 ;
			Data.clear() ;
			for (unsigned int i = 0 ; i < extdatalen ; i++) {
				Data.push_back ((unsigned char) decrypted.getValue (pos, 8)) ;
				pos += 8 ;
			}

			// the random padding data at the end of decrypted is ignored

			if (Checksum) {
				// test if checksum is ok
				MHashpp hash (MHASH_CRC32) ;
				for (vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
					hash << *i ;
				}
				hash << endhash ;
				unsigned long calccrc32 = hash.getHashBits().getValue(0, NBitsCrc32) ;

				if (calccrc32 == extcrc32) {
					VerboseMessage v (_("crc32 checksum test ok.")) ;
					v.printMessage() ;
				}
				else {
					CriticalWarning w (_("crc32 checksum failed! extracted data is probably corrupted.")) ;
					w.printMessage() ;
				}
			}

			NumBitsNeeded = 0 ;
			State = END ;
		break ; }

		case END:
		default: {
			assert (0) ;
		break ; }
	}
}

void EmbData::setCryptAlgo (MCryptpp::Algorithm a)
{
	CryptAlgo = a ;
}
	
MCryptpp::Algorithm EmbData::getCryptAlgo (void)
{
	return CryptAlgo ;
}
	
void EmbData::setCryptMode (MCryptpp::Mode m) 
{
	CryptMode = m ;
}
	
MCryptpp::Mode EmbData::getCryptMode (void)
{
	return CryptMode ;
}
	
void EmbData::setCompression (bool c)
{
	Compression = c ;
}
	
bool EmbData::getCompression (void)
{
	return Compression ;
}
	
void EmbData::setChecksum (bool c)
{
	Checksum = c ;
}
	
bool EmbData::getChecksum (void)
{
	return Checksum ;
}

void EmbData::read ()
{
	assert (Mode == EMBED) ;

	BinaryIO io (FileName, BinaryIO::READ) ;
	while (!io.eof()) {
		Data.push_back (io.read8()) ;
	}
	io.close() ;
}

void EmbData::write ()
{
	assert (Mode == EXTRACT) ;

	BinaryIO io (FileName, BinaryIO::WRITE) ;
	for (vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
		io.write8 (*i) ;
	}
	io.close() ;
}

BitString EmbData::getBitString ()
{
	assert (Mode == EMBED) ;

	BitString main ;
	
	main.append (Compression) ;
	
	main.append (Checksum) ;
	if (Checksum) {
		MHashpp hash (MHASH_CRC32) ;
		for (vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
			hash << *i ;
		}
		hash << endhash ;
		main.append (hash.getHashBits()) ;
	}
	
	if ((Args.EmbFn.getValue() == "") || (!Args.EmbedEmbFn.getValue())) {
		// standard input is used or plain file name embedding has been turned off explicitly
		main.append ((unsigned char) 0, NBitsLenOfFileName) ;
	}
	else {
		string tmp = stripDir (FileName) ;
		if (tmp.size() > EmbFileNameMaxSize) {
			throw SteghideError (_("the maximum length for the embedded file name is %d characters."), EmbFileNameMaxSize) ;
		}
		main.append (tmp.size(), NBitsLenOfFileName) ;
		main.append (tmp) ;
	}
	
	main.append (Data) ;

	// put header together - nembbits contains main.getLength() before encryption
	BitString hdr ;
	hdr.append((unsigned char) CryptAlgo, MCryptpp::sizeAlgorithm).append((unsigned char) CryptMode, MCryptpp::sizeMode) ;
	hdr.append(nbits(main.getLength()), NBitsLenOfNEmbBits).append(main.getLength(), nbits(main.getLength())) ;

	// TODO - zlib deflate here
	if (CryptAlgo != MCryptpp::NONE) {
		MCryptpp crypto (CryptAlgo, CryptMode) ;
		main = crypto.encrypt (main, Args.Passphrase.getValue()) ;
	}

	return hdr.append(main) ;
}

string EmbData::stripDir (string s)
{
	unsigned int start = 0 ;
	if ((start = s.find_last_of ("/\\")) == string::npos) {
		start = 0 ;
	}
	else {
		start += 1 ;
	}
	return s.substr (start, string::npos) ;
}

unsigned int EmbData::nbits (unsigned long x)
{
	unsigned int n = 0 ;
	unsigned long y = 1 ;

	if (x == 0) {
		return 1 ;
	}

	while (x > (y - 1)) {
		y = y * 2 ;
		n = n + 1 ;
	}

	return n ;
}
