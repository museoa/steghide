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

#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>

#include <mcrypt.h>

#include "common.h"
#include "error.h"
#include "mcryptpp.h"
#include "mhashkeygenpp.h"
#include "randomsource.h"

MCryptpp::MCryptpp ()
{
	ModuleOpen = false ;
}

MCryptpp::MCryptpp (Algorithm a, Mode m)
{
	open (getAlgorithmName (a), getModeName (m)) ;
}

MCryptpp::MCryptpp (string algo, string mode)
{
	assert (isValidAlgorithm (algo)) ;
	assert (isValidAlgorithm (mode)) ;

	open (algo, mode) ;
}

MCryptpp::~MCryptpp ()
{
	if (ModuleOpen) {
		close() ;
	}
}

void MCryptpp::open (string a, string m)
{
	char algo[a.size() + 1], mode[m.size() + 1] ;
	strcpy (algo, a.c_str()) ;
	strcpy (mode, m.c_str()) ;
	if ((MCryptD = mcrypt_module_open (algo, MCRYPTPP_LIBDIR, mode, MCRYPTPP_LIBDIR)) == MCRYPT_FAILED) {
		throw SteghideError (_("could not open libmcrypt module \"%s\",\"%s\"."), algo, mode) ;
	}
	ModuleOpen = true ;
}

void MCryptpp::close ()
{
	mcrypt_module_close (MCryptD) ;
	ModuleOpen = false ;
}

BitString MCryptpp::encrypt (BitString p, string pp)
{
	//DEBUG p.padRandom (8 * mcrypt_enc_get_block_size (MCryptD)) ; // blocksize is 1 for stream algorithms
	p.pad (8 * mcrypt_enc_get_block_size (MCryptD), 0) ;
	vector<unsigned char> ciphertext = _encrypt (p.getBytes(), pp) ;
	return BitString (ciphertext) ;
}

BitString MCryptpp::decrypt (BitString c, string pp)
{
	assert (c.getLength() % (8 * mcrypt_enc_get_block_size (MCryptD)) == 0) ;
	vector<unsigned char> plaintext = _decrypt (c.getBytes(), pp) ;
	return BitString (plaintext) ;
}

// TODO - in welchen version von mhash ist keygen_mcrypt kompatibel ?? nmav schreiben
void* MCryptpp::createKey (string pp)
{
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	MHashKeyGenpp keygen (KEYGEN_MCRYPT, MHASH_MD5, keysize) ;
	vector<unsigned char> key = keygen.createKey (pp) ;
	unsigned char *retval = (unsigned char *) s_malloc (keysize) ;
	for (unsigned int i = 0 ; i < keysize ; i++) {
		retval[i] = key[i] ;
	}
	return retval ;
}
vector<unsigned char> MCryptpp::_encrypt (vector<unsigned char> p, string pp)
{
	// genereate key and IV (if needed)
	void *key = createKey (pp) ;
	unsigned char *IV = NULL ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		//DEBUG vector<unsigned char> rndIV = RndSrc.getBytes (ivsize) ;
		vector<unsigned char> rndIV = vector<unsigned char> (ivsize, 0) ;
		IV = (unsigned char *) s_malloc (ivsize) ;
		for (unsigned int i = 0 ; i < ivsize ; i++) {
			IV[i] = rndIV[i] ;
		}
	}

	// initialize libmcrypt thread
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	int err = -1 ;
	if ((err = mcrypt_generic_init (MCryptD, key, keysize, IV)) < 0) {
		mcrypt_perror (err) ;
		throw SteghideError (_("could not initialize libmcrypt encryption. see above error messages if any.")) ;
	}

	// copy plaintext
	unsigned int plntextlen = p.size() ;
	assert (plntextlen % mcrypt_enc_get_block_size (MCryptD) == 0) ;
	unsigned char *plntext = (unsigned char *) s_malloc (plntextlen) ;
	for (unsigned int i = 0 ; i < plntextlen ; i++) {
		plntext[i] = p[i] ;
	}
	
	// encrypt plaintext
	if (mcrypt_generic (MCryptD, plntext, plntextlen) != 0) {
		throw SteghideError (_("could not encrypt data.")) ;
	}

	// create the return value
	vector<unsigned char> retval ;
	unsigned int i = 0 ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		retval = vector<unsigned char> (ivsize + plntextlen) ;
		for ( ; i < ivsize ; i++) {
			retval[i] = IV[i] ;
		}
	}
	else {
		retval = vector<unsigned char> (plntextlen) ;
	}
	for (unsigned int j = 0 ; j < plntextlen ; i++, j++) {
		retval[i] = plntext[j] ;
	}

	// clean up
	if (mcrypt_generic_deinit (MCryptD) < 0) {
		throw SteghideError (_("could not finish libmcrypt encryption.")) ;
	}
	free (plntext) ;
	free (key) ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		free (IV) ;
	}

	return retval ;
}

vector<unsigned char> MCryptpp::_decrypt (vector<unsigned char> c, string pp)
{
	// generate key
	void *key = createKey (pp) ;
	unsigned char *IV = NULL ;
	unsigned int cstart = 0 ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		unsigned int ivsize = mcrypt_enc_get_iv_size (MCryptD) ;
		IV = (unsigned char *) s_malloc (ivsize) ;
		for (unsigned int i = 0 ; i < ivsize ; i++) {
			assert (c[i] == 0) ; // DEBUG
			IV[i] = c[i] ;
		}
		cstart = ivsize ;
	}

	// initialize libmcrypt thread
	unsigned int keysize = mcrypt_enc_get_key_size (MCryptD) ;
	int err = -1 ;
	if ((err = mcrypt_generic_init (MCryptD, key, keysize, IV)) < 0) {
		mcrypt_perror (err) ;
		throw SteghideError (_("could not initialize libmcrypt decryption. see above error messages if any.")) ;
	}

	// copy ciphertext
	unsigned long ciphertextlen = c.size() - cstart ;
	assert (ciphertextlen % mcrypt_enc_get_block_size (MCryptD) == 0) ;
	unsigned char *ciphertext = (unsigned char *) s_malloc (ciphertextlen) ;
	for (unsigned int i = 0 ; i < ciphertextlen ; i++) {
		ciphertext[i] = c[cstart + i] ;
	}

	// decrypt ciphertext
	if (mdecrypt_generic (MCryptD, ciphertext, ciphertextlen) != 0) {
		throw SteghideError (_("could not decrypt data.")) ;
	}

	// create return value
	vector<unsigned char> retval (ciphertextlen) ;
	for (unsigned int i = 0 ; i < ciphertextlen ; i++) {
		retval[i] = ciphertext[i] ;
	}
	
	// clean up
	if (mcrypt_generic_deinit (MCryptD) < 0) {
		throw SteghideError (_("could not finish libmcrypt decryption.")) ;
	}
	free (ciphertext) ;
	free (key) ;
	if (mcrypt_enc_mode_has_iv (MCryptD)) {
		free (IV) ;
	}

	return retval ;
}

string MCryptpp::getAlgorithmName ()
{
	assert (ModuleOpen) ;
	char *name = mcrypt_enc_get_algorithms_name (MCryptD) ;
	string retval = string (name) ;
	free (name) ;
	return retval ;
}

string MCryptpp::getModeName ()
{
	assert (ModuleOpen) ;
	char *name = mcrypt_enc_get_modes_name (MCryptD) ;
	string retval = string (name) ;
	free (name) ;
	return retval ;
}

string MCryptpp::getAlgorithmName (Algorithm a)
{
	string retval ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumAlgoTranslations ; i++) {
		if (AlgoTranslations[i].algo == a) {
			retval = string (AlgoTranslations[i].name) ;
			found = true ;
		}
	}
	assert (found) ;
	return retval ;
}

string MCryptpp::getModeName (Mode m)
{
	string retval ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumModeTranslations ; i++) {
		if (ModeTranslations[i].mode == m) {
			retval = string (ModeTranslations[i].name) ;
			found = true ;
		}
	}
	assert (found) ;
	return retval ;
}

MCryptpp::Algorithm MCryptpp::getAlgorithm (string name)
{
	Algorithm retval ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumAlgoTranslations ; i++) {
		if (AlgoTranslations[i].name == name) {
			retval = AlgoTranslations[i].algo ;
			found = true ;
		}
	}
	assert (found) ;
	return retval ;
}

MCryptpp::Mode MCryptpp::getMode (string name)
{
	Mode retval ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumModeTranslations ; i++) {
		if (ModeTranslations[i].name == name) {
			retval = ModeTranslations[i].mode ;
			found = true ;
		}
	}
	assert (found) ;
	return retval ;
}

unsigned long MCryptpp::getEncryptedSize (Algorithm a, Mode m, unsigned long plnsize)
{
	unsigned long retval = 0 ;

	if (a == NONE) {
		retval = plnsize ;
	}
	else {
		string tmp1 = getAlgorithmName (a), tmp2 = getModeName (m) ;
		char algo[tmp1.size() + 1], mode[tmp2.size() + 1] ;
		strcpy (algo, tmp1.c_str()) ;
		strcpy (mode, tmp2.c_str()) ;

		MCRYPT td ;
		if ((td = mcrypt_module_open (algo, MCRYPTPP_LIBDIR, mode, MCRYPTPP_LIBDIR)) == MCRYPT_FAILED) {
			throw SteghideError (_("could not open libmcrypt module \"%s\",\"%s\"."), algo, mode) ;
		}

		if (mcrypt_enc_mode_has_iv (td)) {
			retval += (8 * mcrypt_enc_get_iv_size(td)) ;
		}

		unsigned long blocks = 0 ;
		const unsigned long blocksize = 8 * mcrypt_enc_get_block_size(td) ; // is 1 for stream algorithms
		if (plnsize % blocksize == 0) {
			blocks = plnsize / blocksize ;
		}
		else {
			blocks = (plnsize / blocksize) + 1;
		}
		retval += (blocks * blocksize) ;

		mcrypt_module_close (td) ;
	}

	return retval ;
}

vector<string> MCryptpp::getListAlgorithms ()
{
	int size = 0 ;
	char **list = mcrypt_list_algorithms (MCRYPTPP_LIBDIR, &size) ;

	vector<string> retval ;
	for (int i = 0 ; i < size ; i++) {
		retval.push_back (string (list[i])) ;
	}
	mcrypt_free_p (list, size) ;

	return retval ;
}

vector<string> MCryptpp::getListModes ()
{
	int size = 0 ;
	char **list = mcrypt_list_modes (MCRYPTPP_LIBDIR, &size) ;

	vector<string> retval ;
	for (int i = 0 ; i < size ; i++) {
		retval.push_back (string (list[i])) ;
	}
	mcrypt_free_p (list, size) ;

	return retval ;
}

bool MCryptpp::isValidAlgorithm (string s)
{
	vector<string> algos = getListAlgorithms() ;
	bool retval = false ;
	if (find (algos.begin(), algos.end(), s) != algos.end()) {
		retval = true ;
	}
	return retval ;
}

bool MCryptpp::isValidMode (string s)
{
	vector<string> modes = getListModes() ;
	bool retval = false ;
	if (find (modes.begin(), modes.end(), s) != modes.end()) {
		retval = true ;
	}
	return retval ;
}

bool MCryptpp::AlgoSupportsMode (string a, string m)
{
	char algo[a.size() + 1], mode[m.size() + 1] ;
	strcpy (algo, a.c_str()) ;
	strcpy (mode, m.c_str()) ;
	return (mcrypt_module_is_block_algorithm (algo, MCRYPTPP_LIBDIR) ==
			mcrypt_module_is_block_algorithm_mode (mode, MCRYPTPP_LIBDIR)) ;
}

void *MCryptpp::s_malloc (size_t size)
{
	void *retval = NULL ;
	if ((retval = malloc (size)) == NULL) {
		throw SteghideError (_("could not allocate memory.")) ;
	}
	return retval ;
}

const MCryptpp::AlgoTranslation MCryptpp::AlgoTranslations[] = {
	{ NONE, "none" },
	{ TWOFISH, "twofish" },
	{ RIJNDAEL128, "rijndael-128" },
	{ RIJNDAEL192, "rijndael-192" },
	{ RIJNDAEL256, "rijndael-256" },
	{ SAFERPLUS, "saferplus" },
	{ RC2, "rc2" },
	{ XTEA, "xtea" },
	{ SERPENT, "serpent" },
	{ SAFERSK64, "safer-sk64" },
	{ SAFERSK128, "safer-sk128" },
	{ CAST256, "cast-256" },
	{ LOKI97, "loki97" },
	{ GOST, "gost" },
	{ THREEWAY, "threeway" },
	{ CAST128, "cast-128" },
	{ BLOWFISH, "blowfish" },
	{ DES, "des" },
	{ TRIPLEDES, "tripledes" },
	{ ENIGMA, "enigma" },
	{ ARCFOUR, "arcfour" },
	{ PANAMA, "panama" },
	{ WAKE, "wake" }
} ;

const MCryptpp::ModeTranslation MCryptpp::ModeTranslations[] = {
	{ ECB, "ecb" },
	{ CBC, "cbc" },
	{ OFB, "ofb" },
	{ CFB, "cfb" },
	{ NOFB, "nofb" },
	{ NCFB, "ncfb" },
	{ CTR, "ctr" },
	{ STREAM, "stream" }
} ;
