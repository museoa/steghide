/*
 * steghide 0.4.2 - a steganography program
 * Copyright (C) 2001 Stefan Hetzl <shetzl@teleweb.at>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include <mcrypt.h>

#include "bufmanag.h"
#include "stegano.h"
#include "crypto.h"
#include "hash.h"
#include "support.h"
#include "msg.h"

void encrypt_sthdr (void *buf, int buflen, char *passphrase)
{
    MCRYPT mcryptd ;
    void *key = NULL ;
	int err = -1 ;

	assert (buflen % BLOCKSIZE_BLOWFISH == 0) ;

    if ((mcryptd = mcrypt_module_open (CRYPTOALGO_STHDR, CRYPTOALGODIR, CRYPTOMODE_STHDR, CRYPTOMODEDIR)) == MCRYPT_FAILED) {
		exit_err ("could not open libmcrypt module \"%s\",\"%s\".", CRYPTOALGO_STHDR, CRYPTOMODE_STHDR) ;
    }

    key = getblowfishkey (passphrase) ;

    if ((err = mcrypt_generic_init (mcryptd, key, SIZE_BLOWFISHKEY, NULL)) < 0) {
		mcrypt_perror (err) ;
		exit_err ("could not initialize libmcrypt encryption. see above error messages if any.") ;
    }

    if (mcrypt_generic (mcryptd, buf, buflen) != 0) {
		exit_err ("could not encrypt stego header.") ;
    }

    if (mcrypt_generic_end (mcryptd) < 0) {
		exit_err ("could not finish encryption of stego header.") ;
    }
}

void decrypt_sthdr (void *buf, int buflen, char *passphrase)
{
    MCRYPT mcryptd ;
    void *key = NULL ;
	int err = -1 ;

	assert (buflen % BLOCKSIZE_BLOWFISH == 0) ;

    if ((mcryptd = mcrypt_module_open (CRYPTOALGO_STHDR, CRYPTOALGODIR, CRYPTOMODE_STHDR, CRYPTOMODEDIR)) == MCRYPT_FAILED) {
		exit_err ("could not open libmcrypt module \"%s\",\"%s\".", CRYPTOALGO_STHDR, CRYPTOMODE_STHDR) ;
    }

    key = getblowfishkey (passphrase) ;

    if ((err = mcrypt_generic_init (mcryptd, key, SIZE_BLOWFISHKEY, NULL)) < 0) {
		mcrypt_perror (err) ;
		exit_err ("could not initialize libmcrypt decryption. see above error messages if any.") ;
    }

    if (mdecrypt_generic (mcryptd, buf, buflen) != 0) {
		exit_err ("could not decrypt stego header.") ;
    }

    if (mcrypt_generic_end (mcryptd) < 0) {
		exit_err ("could not finish decryption of stego header.") ;
    }
}

void encrypt_plnfile (PLNFILE *plnfile, char *passphrase)
{
	MCRYPT mcryptd ;
	void *key = NULL ;
	BUFFER *result = NULL ;
	unsigned char buf[BLOCKSIZE_BLOWFISH] ;
	unsigned long blocknum = 0, plnpos = 0, bufpos = 0 ;
	unsigned char IV[BLOCKSIZE_BLOWFISH] ;
	int i = 0, err = -1 ;

	result = createbuflist () ;
	for (i = 0 ; i < BLOCKSIZE_BLOWFISH ; i++) {
		IV[i] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
		bufsetbyte (result, i, IV[i]) ;
	}

	if ((mcryptd = mcrypt_module_open (CRYPTOALGO_DATA, CRYPTOALGODIR, CRYPTOMODE_DATA, CRYPTOMODEDIR)) == MCRYPT_FAILED) {
		exit_err ("could not open libmcrypt module \"%s\",\"%s\".", CRYPTOALGO_STHDR, CRYPTOMODE_STHDR) ;
	}

	key = getblowfishkey (passphrase) ;

	if ((err = mcrypt_generic_init (mcryptd, key, SIZE_BLOWFISHKEY, IV)) < 0) {
		mcrypt_perror (err) ;
		exit_err ("could not initialize libmcrypt encryption. see above error messages if any.") ;
	}

	while (plnpos < buflength (plnfile->plnbuflhead)) {
		bufpos = 0 ;
		while ((bufpos < BLOCKSIZE_BLOWFISH) && (plnpos < buflength (plnfile->plnbuflhead))) {
			buf[bufpos] = (unsigned char) bufgetbyte (plnfile->plnbuflhead, plnpos) ;
			bufpos++ ;
			plnpos++ ;
		}

		if (plnpos == buflength (plnfile->plnbuflhead)) {
			for ( ; bufpos < BLOCKSIZE_BLOWFISH ; bufpos++) {
				buf[bufpos] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
			}
		}

		if (mcrypt_generic (mcryptd, buf, BLOCKSIZE_BLOWFISH) != 0) {
			exit_err ("could not encrypt plain data. failed at block number %lu", blocknum) ;
		}

		for (i = 0 ; i < BLOCKSIZE_BLOWFISH ; i++) {
			bufsetbyte (result, ((blocknum + 1 /* IV ! */) * BLOCKSIZE_BLOWFISH) + i , (int) buf[i]) ; 
		}

		blocknum++ ;
	}

	if (mcrypt_generic_end (mcryptd) < 0) {
		exit_err ("could not finish encryption of plain data.") ;
	}

	buffree (plnfile->plnbuflhead) ;
	plnfile->plnbuflhead = result ;
}

void decrypt_plnfile (PLNFILE *plnfile, char *passphrase)
{
	MCRYPT mcryptd ;
	void *key ;
	BUFFER *result = NULL ;
	unsigned char IV[BLOCKSIZE_BLOWFISH], buf[BLOCKSIZE_BLOWFISH] ;
	unsigned long plnpos = 0, bufpos = 0 ;
	int i = 0, blocknum = 0, err = -1 ;

	assert (buflength (plnfile->plnbuflhead) % BLOCKSIZE_BLOWFISH == 0) ;

	result = createbuflist () ;

	for ( ; plnpos < BLOCKSIZE_BLOWFISH ; plnpos++) {
		IV[plnpos] = bufgetbyte (plnfile->plnbuflhead, plnpos) ;
	}

	if ((mcryptd = mcrypt_module_open (CRYPTOALGO_DATA, CRYPTOALGODIR, CRYPTOMODE_DATA, CRYPTOMODEDIR)) == MCRYPT_FAILED) {
		exit_err ("could not open libmcrypt module \"%s\",\"%s\".", CRYPTOALGO_STHDR, CRYPTOMODE_STHDR) ;
	}

	key = getblowfishkey (passphrase) ;

	if ((err = mcrypt_generic_init (mcryptd, key, SIZE_BLOWFISHKEY, IV)) < 0) {
		mcrypt_perror (err) ;
		exit_err ("could not initialize libmcrypt decryption. see above error messages if any.") ;
	}

	while (plnpos < buflength (plnfile->plnbuflhead)) {
		for (bufpos = 0 ; bufpos < BLOCKSIZE_BLOWFISH ; bufpos++) {
			buf[bufpos] = bufgetbyte (plnfile->plnbuflhead, plnpos) ;
			plnpos++ ;
		}

		if (mdecrypt_generic (mcryptd, buf, BLOCKSIZE_BLOWFISH) != 0) {
			exit_err ("could not decrypt plain data. failed at block number %ul", blocknum) ;
		}

		for (i = 0 ; i < BLOCKSIZE_BLOWFISH ; i++) {
			if (((plnpos - BLOCKSIZE_BLOWFISH) + i) - BLOCKSIZE_BLOWFISH /* IV */ < sthdr.nbytesplain) {
				bufsetbyte (result, blocknum * BLOCKSIZE_BLOWFISH + i, (int) buf[i]) ;
			}
		}

		blocknum++ ;
	}	

	if (mcrypt_generic_end (mcryptd) < 0) {
		exit_err ("could not finish decryption of plain data.") ;
	}
	
	buffree (plnfile->plnbuflhead) ;
	plnfile->plnbuflhead = result ;
}
