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

EmbData::EmbData (MODE m, std::string fn)
{
	Mode = m ;
	FileName = fn ;

	switch (Mode) {
		case EMBED: {
			read() ;
		break ; }

		case EXTRACT: {
			NumBitsNeeded = EncryptionAlgorithm::IRep_size + EncryptionMode::IRep_size ;
			State = READCRYPT ;
		break ; }

		default: {
			myassert (0) ;
		break ; }
	}
}

bool EmbData::finished ()
{
	myassert (Mode == EXTRACT) ;
	return (State == END) ;
}

unsigned long EmbData::getNumBitsNeeded ()
{
	myassert (Mode == EXTRACT) ;
	return NumBitsNeeded ;
}

void EmbData::addBits (BitString bits)
{
	myassert (Mode == EXTRACT) ;
	myassert (bits.getLength() == NumBitsNeeded) ;

	switch (State) {
		case READCRYPT: {
			unsigned int algo = (unsigned int) bits.getValue (0, EncryptionAlgorithm::IRep_size) ;
			if (EncryptionAlgorithm::isValidIntegerRep (algo)) {
				EncAlgo.setValue ((EncryptionAlgorithm::IRep) algo) ;
			}
			unsigned int mode = (unsigned int) bits.getValue (EncryptionAlgorithm::IRep_size, EncryptionMode::IRep_size) ;
			if (EncryptionMode::isValidIntegerRep (mode)) {
				EncMode.setValue ((EncryptionMode::IRep) mode) ;
			}
			NumBitsNeeded = NBitsLenOfNEmbBits ;
			State = READLENOFNEMBBITS ;
#ifndef USE_LIBMCRYPT
			if (EncAlgo.getIntegerRep() != EncryptionAlgorithm::NONE) {
				throw SteghideError (_("the embedded data is encrypted but steghide has been compiled without encryption support.")) ;
			}
#endif
		break ; }

		case READLENOFNEMBBITS: {
			NumBitsNeeded = bits.getValue (0, NBitsLenOfNEmbBits) ;
			State = READNEMBBITS ;
		break ; }

		case READNEMBBITS: {
			NEmbBits = bits.getValue (0, NumBitsNeeded) ;
#ifdef USE_LIBMCRYPT
			NumBitsNeeded = MCryptpp::getEncryptedSize (EncAlgo, EncMode, NEmbBits) ;
#else
			myassert (EncAlgo.getIntegerRep() == EncryptionAlgorithm::NONE) ;
			NumBitsNeeded = NEmbBits ;
#endif
			State = READENCRYPTED ;
		break ; }

		case READENCRYPTED: {
#ifdef USE_LIBMCRYPT
			BitString decrypted ;
			if (EncAlgo.getIntegerRep() == EncryptionAlgorithm::NONE) {
				decrypted = bits ;
			}
			else {
				MCryptpp crypto (EncAlgo, EncMode) ;
				decrypted = crypto.decrypt (bits, Args.Passphrase.getValue()) ;
			}
#else
			BitString decrypted = bits ;
#endif

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
			std::string filename = "" ;
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
				myassert (Args.ExtFn.getValue() == "") ;
				if (lenoffilename == 0) {
					throw SteghideError (_("please specify a file name for the extracted data (there is no name embedded in the stego file).")) ;
				}
				FileName = filename ;
			}

			myassert ((NEmbBits - pos) % 8 == 0) ;

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
				for (std::vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
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
			myassert (0) ;
		break ; }
	}
}

void EmbData::setEncAlgo (EncryptionAlgorithm a)
{
	EncAlgo = a ;
}
	
EncryptionAlgorithm EmbData::getEncAlgo () const
{
	return EncAlgo ;
}
	
void EmbData::setEncMode (EncryptionMode m) 
{
	EncMode = m ;
}
	
EncryptionMode EmbData::getEncMode () const
{
	return EncMode ;
}
	
void EmbData::setCompression (bool c)
{
	Compression = c ;
}
	
bool EmbData::getCompression (void) const
{
	return Compression ;
}
	
void EmbData::setChecksum (bool c)
{
	Checksum = c ;
}
	
bool EmbData::getChecksum (void) const
{
	return Checksum ;
}

void EmbData::read ()
{
	myassert (Mode == EMBED) ;

	BinaryIO io (FileName, BinaryIO::READ) ;
	while (!io.eof()) {
		Data.push_back (io.read8()) ;
	}
	io.close() ;
}

void EmbData::write ()
{
	myassert (Mode == EXTRACT) ;

	BinaryIO io (FileName, BinaryIO::WRITE) ;
	for (std::vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
		io.write8 (*i) ;
	}
	io.close() ;
}

BitString EmbData::getBitString ()
{
	myassert (Mode == EMBED) ;

	BitString main ;
	
	main.append (Compression) ;
	
	main.append (Checksum) ;
	if (Checksum) {
		MHashpp hash (MHASH_CRC32) ;
		for (std::vector<unsigned char>::iterator i = Data.begin() ; i != Data.end() ; i++) {
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
		std::string tmp = stripDir (FileName) ;
		if (tmp.size() > EmbFileNameMaxSize) {
			throw SteghideError (_("the maximum length for the embedded file's name is %d characters."), EmbFileNameMaxSize) ;
		}
		main.append ((UWORD16) tmp.size(), NBitsLenOfFileName) ;
		main.append (tmp) ;
	}
	
	main.append (Data) ;

	// put header together - nembbits contains main.getLength() before encryption
	BitString hdr ;
	hdr.append((UWORD16) EncAlgo.getIntegerRep(), EncryptionAlgorithm::IRep_size) ;
	hdr.append((UWORD16) EncMode.getIntegerRep(), EncryptionMode::IRep_size) ;
	hdr.append((UWORD16) nbits(main.getLength()), NBitsLenOfNEmbBits).append((UWORD16) main.getLength(), nbits(main.getLength())) ;

	// TODO - zlib deflate here
	
#ifdef USE_LIBMCRYPT
	if (EncAlgo.getIntegerRep() != EncryptionAlgorithm::NONE) {
		MCryptpp crypto (EncAlgo, EncMode) ;
		main = crypto.encrypt (main, Args.Passphrase.getValue()) ;
	}
#else
	myassert (EncAlgo.getIntegerRep() == EncryptionAlgorithm::NONE) ;
#endif

	return hdr.append(main) ;
}

std::string EmbData::stripDir (std::string s)
{
	unsigned int start = 0 ;
	if ((start = s.find_last_of ("/\\")) == std::string::npos) {
		start = 0 ;
	}
	else {
		start += 1 ;
	}
	return s.substr (start, std::string::npos) ;
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
