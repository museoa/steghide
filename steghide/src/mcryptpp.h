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

#ifdef USE_LIBMCRYPT

#ifndef SH_MCRYPTPP_H
#define SH_MCRYPTPP_H

#include <string>

#include <mcrypt.h>

#include "bitstring.h"
#include "encalgo.h"
#include "encmode.h"

#ifdef WIN32
#define MCRYPTPP_LIBDIR	"./modules/"
#else
#define MCRYPTPP_LIBDIR NULL
#endif

class MCryptpp {
	public:
	MCryptpp (void) ;
	MCryptpp (EncryptionAlgorithm a, EncryptionMode m) ;

	~MCryptpp (void) ;

	/**
	 * open the libmcrypt module algo/mode
	 * \param a the encryption algorithm
	 * \param m the encryption mode (must be supported by the given encryption algorithm)
	 **/
	void open (EncryptionAlgorithm a, EncryptionMode m) ;

	/**
	 * close the opened libmcrypt module
	 **/
	void close (void) ;

	BitString encrypt (BitString p, std::string pp) ;
	BitString decrypt (BitString c, std::string pp) ;

	EncryptionAlgorithm getAlgorithm (void) const ;
	EncryptionMode getMode (void) const ;

	/**
	 * get the size of an encryption result
	 * \param a the algorithm to be used for encryption
	 * \param m the mode to be used for encryption
	 * \param plnsize the size of the plaintext (in bits)
	 * \return the size the ciphertext would have (in bits and including the IV)
	 **/
	static unsigned long getEncryptedSize (EncryptionAlgorithm a, EncryptionMode m, unsigned long plnsize) ;

	static std::vector<std::string> getListModes (void) ;
	static std::vector<std::string> getListAlgorithms (void) ;

	static bool AlgoSupportsMode (EncryptionAlgorithm a, EncryptionMode m) ;

	protected:
	void *createKey (std::string pp) ;

	/**
	 * do the actual encryption
	 * \param p the plaintext to be encrypted
	 * \param pp the passphrase
	 * \return the encrypted data (with the IV as first block (if an IV is used by this mode))
	 *
	 * The size of p must be a multiple of the blocksize of the encryption algorithm.
	 **/
	std::vector<unsigned char> _encrypt (std::vector<unsigned char> p, std::string pp) ;

	/**
	 * do the actual decryption
	 * \param c the ciphertext to be decrypted (with the IV as first block (if an IV is used by this mode))
	 * \param pp the passphrase
	 * \return the decrypted data
	 *
	 * The size of c must be a multiple of the blocksize of the encryption algorithm.
	 **/
	std::vector<unsigned char> _decrypt (std::vector<unsigned char> c, std::string pp) ;

	private:
	/// true iff CryptD contains a valid encryption descriptor
	bool ModuleOpen ;
	MCRYPT MCryptD ;

	void *s_malloc (size_t size) ;
} ;

/* TODO: test with every algorithm and every mode - to avoid typos
   */

#endif // ndef SH_MCRYPTPP_H

#endif // def USE_LIBMCRYPT
