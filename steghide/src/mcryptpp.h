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

#ifndef SH_MCRYPTPP_H
#define SH_MCRYPTPP_H

#include <string>

#include <mcrypt.h>

#include "bitstring.h"

#ifdef WIN32
#define MCRYPTPP_LIBDIR	"./modules/"
#else
#define MCRYPTPP_LIBDIR NULL
#endif

class MCryptpp {
	public:
	/// number of bits needed to code the algorithm	
	static const unsigned int sizeAlgorithm = 5 ;
	enum Algorithm {
		NONE = 0,
		TWOFISH = 1,
		RIJNDAEL128 = 2,
		RIJNDAEL192 = 3,
		RIJNDAEL256 = 4,
		SAFERPLUS = 5,
		RC2 = 6,
		XTEA = 7,
		SERPENT = 8,
		SAFERSK64 = 9,
		SAFERSK128 = 10,
		CAST256 = 11,
		LOKI97 = 12,
		GOST = 13,
		THREEWAY = 14,
		CAST128 = 15,
		BLOWFISH = 16,
		DES = 17,
		TRIPLEDES = 18,
		ENIGMA = 19,
		ARCFOUR = 20,
		PANAMA = 21,
		WAKE = 22
	} ;

	/// number of bits needed to code the mode
	static const unsigned int sizeMode = 3 ;
	enum Mode {
		ECB = 0,
		CBC = 1,
		OFB = 2,
		CFB = 3,
		NOFB = 4,
		NCFB = 5,
		CTR = 6,
		STREAM = 7
	} ;

	MCryptpp (void) ;

	/**
	 * construct a MCryptpp object
	 * \param a the algorithm to be used for en/decryption
	 * \param m the mode to be used for en/decryption
	 **/
	MCryptpp (Algorithm a, Mode m) ;

	/**
	 * construct a MCryptpp object
	 * \param algo the name of an algorithm to be used for en/decryption
	 * \param mode the name of a mode to be used for en/decryption
	 **/
	MCryptpp (string algo, string mode) ;

	~MCryptpp (void) ;

	/**
	 * open the libmcrypt module algo/mode
	 * \param a the name of an encryption algorithm supported by libmcrypt
	 * \param m the name of a mode supported by libmcrypt and the given encryption algorithm
	 **/
	void open (string a, string m) ;

	/**
	 * close the opened libmcrypt module
	 **/
	void close (void) ;

	BitString encrypt (BitString p, string pp) ;
	BitString decrypt (BitString c, string pp) ;

	string getAlgorithmName (void) ;
	string getModeName (void) ;
	static string getAlgorithmName (Algorithm a) ;
	static string getModeName (Mode m) ;

	Algorithm getAlgorithm (void) ;
	Mode getMode (void) ;
	static Algorithm getAlgorithm (string name) ;
	static Mode getMode (string name) ;

	/**
	 * get the size of an encryption result
	 * \param a the algorithm to be used for encryption
	 * \param m the mode to be used for encryption
	 * \param plnsize the size of the plaintext (in bits)
	 * \return the size the ciphertext would have (in bits and including the IV)
	 **/
	static unsigned long getEncryptedSize (Algorithm a, Mode m, unsigned long plnsize) ;

	static vector<string> getListModes (void) ;
	static vector<string> getListAlgorithms (void) ;

	static bool isValidAlgorithm (string s) ;
	static bool isValidMode (string s) ;

	static bool AlgoSupportsMode (string algo, string mode) ;

	// ? - macht das sinn ?
	// TODO wenn einmal so funktioniert: salt (unverändert durch verstecken !!) verwenden,
	// um Schlüssel außer von passphrase auch von cvrstg-Datei abhängig zu machen - dazu:
	// in CvrStgObject neue Funktion vector<unsigned char> getCleanSample (unsigned int n) - retourniert daten eines samples bis auf
	// den Bereich, in dem versteckt wird (also z.B. Daten, wo LSB = 0 gesetzt wurden)
	// auch für permutation!

	protected:
	void *createKey (string pp) ;

	/**
	 * do the actual encryption
	 * \param p the plaintext to be encrypted
	 * \param pp the passphrase
	 * \return the encrypted data (with the IV as first block (if an IV is used by this mode))
	 *
	 * The size of p must be a multiple of the blocksize of the encryption algorithm.
	 **/
	vector<unsigned char> _encrypt (vector<unsigned char> p, string pp) ;

	/**
	 * do the actual decryption
	 * \param c the ciphertext to be decrypted (with the IV as first block (if an IV is used by this mode))
	 * \param pp the passphrase
	 * \return the decrypted data
	 *
	 * The size of c must be a multiple of the blocksize of the encryption algorithm.
	 **/
	vector<unsigned char> _decrypt (vector<unsigned char> c, string pp) ;

	private:
	/// true iff CryptD contains a valid encryption descriptor
	bool ModuleOpen ;
	MCRYPT MCryptD ;

	void *s_malloc (size_t size) ;

	typedef struct struct_AlgoTranslation {
		Algorithm algo ;
		char* name ;
	} AlgoTranslation ;
	typedef struct struct_ModeTranslation {
		Mode mode ;
		char* name ;
	} ModeTranslation ;

	static const unsigned int NumAlgoTranslations = 23 ;
	static const AlgoTranslation AlgoTranslations[] ;

	static const unsigned int NumModeTranslations = 8 ;
	static const ModeTranslation ModeTranslations[] ;
} ;

/* TODO: einmal mit jedem algo und jedem mode ausprobieren - um tippfehler zu vermeiden
   */

#endif // SH_MCRYPTPP_H
