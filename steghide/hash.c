/*
 * steghide 0.4.4 - a steganography program
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

#include <string.h>

#include <mhash.h>

#include "bufmanag.h"
#include "io.h"
#include "support.h"
#include "msg.h"

unsigned long getseed (char *passphrase)
{
	MHASH hashd ;
	unsigned char *hash = NULL ;
	unsigned char tmp[4] = { '\0', '\0', '\0', '\0' } ;
	unsigned long retval = 0UL ;
	int i = 0 ;

	if ((hashd = mhash_init (MHASH_MD5)) == MHASH_FAILED) {
		exit_err ("could not initialize libmhash MD5 algorithm.") ;
	}
	mhash (hashd, passphrase, strlen (passphrase)) ;
	hash = mhash_end (hashd) ;

	for (i = 0 ; i < 4 ; i++) {
		tmp[i] = hash[0 + i] ^ hash[4 + i] ^ hash[8 + i] ^ hash[12 + i] ;
	}

	cp32uc2ul_be (&retval, tmp) ;

	return retval ;
}

void *getcrc32 (PLNFILE *plnfile)
{
	MHASH hashd ;
	unsigned long i = 0, n = 0 ;
	unsigned char c ;

	if ((hashd = mhash_init (MHASH_CRC32)) == MHASH_FAILED) {
		exit_err ("could not initialize libmhash CRC32 algorithm.") ;
	}

	n = plnfile->plndata->length ;
	for (i = 0 ; i < n ; i++) {
		c = (char) bufgetbyte (plnfile->plndata, i) ;
		mhash (hashd, &c, 1) ;
	}

	return mhash_end (hashd) ;
}

int checkcrc32 (PLNFILE *plnfile, void *crc32)
{
	unsigned char *uc_crc32_1 = crc32 ;
	unsigned char *uc_crc32_2 = getcrc32 (plnfile) ;
	int i = 0, retval = 1 ;

	for (i = 0 ; i < 4 ; i++) {
		if (uc_crc32_1[i] != uc_crc32_2[i]) {
			retval = 0 ;
		}
	}

	return retval ;
}

void *getblowfishkey (char *passphrase)
{
	MHASH hashd ;

	if ((hashd = mhash_init (MHASH_MD5)) == MHASH_FAILED) {
		exit_err ("could not initialize libmhash MD5 algorithm.") ;
	}

	mhash (hashd, passphrase, strlen (passphrase)) ;

	return mhash_end (hashd) ;
}
