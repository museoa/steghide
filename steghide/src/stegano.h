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

#ifndef SH_STEGANO_H
#define SH_STEGANO_H

#include "bufmanag.h"
#include "crypto.h"

/* stego header structure definition */

/* distribution method sub-structures */
typedef struct struct_DMTDINFO_CNSTI {
	/* interval length between two hiding bits */
	unsigned int	interval_len ;
} DMTDINFO_CNSTI ;

typedef struct struct_DMTDINFO_PRNDI {
	/* seed for random number generator */
	unsigned long	seed ;
	/* largest value the random number generator will create */
	unsigned int	interval_maxlen ;
} DMTDINFO_PRNDI ;

typedef union union_DMTDINFO {
	DMTDINFO_CNSTI	cnsti ;
	DMTDINFO_PRNDI	prndi ;
} DMTDINFO ;

/* stego header structure
   this structure describes the plaindata and how it is embedded */
typedef struct struct_STEGOHEADER {
	/* length of plain data (before encryption with name and crc) in bytes */
	unsigned long	nbytesplain ;

	/* distribution method (how to calculate positions of secret bits) */
	unsigned int	dmtd ;
	/* dmtdinfo-specific parameters */
	DMTDINFO		dmtdinfo ;

	/* mask that is used to determine bits that contain data
	   if stgmask is for example 1, only the least significant bit will be used */
	unsigned int	mask ;

	/* encryption */
	unsigned int	encryption ;

	/* compression - NOT YET IMPLEMENTED */
	unsigned int	compression ;

	/* checksum */
	unsigned int	checksum ;
} STEGOHEADER ;

/* size of stego header and components when embedded in stego file */
#define SIZE_NBITS_NBYTESPLAIN	5
#define MAXSIZE_NBYTESPLAIN	32
#define SIZE_DMTD	2
#define SIZE_DMTDINFO_CNSTI_NBITS_ILEN	4
#define MAXSIZE_DMTDINFO_CNSTI_ILEN	16
#define SIZE_DMTDINFO_PRNDI_SEED	32
#define SIZE_DMTDINFO_PRNDI_NBITS_IMLEN 4
#define MAXSIZE_DMTDINFO_PRNDI_IMLEN	16
#define SIZE_MASKUSED	1
#define SIZE_MASK	8
#define SIZE_ENCRYPTION	1
#define SIZE_COMPRESSION	1
#define SIZE_CHECKSUM	1

#define MAXSIZE_STHDR SIZE_NBITS_NBYTESPLAIN + MAXSIZE_NBYTESPLAIN + SIZE_DMTD + SIZE_DMTDINFO_PRNDI_SEED + \
		SIZE_DMTDINFO_PRNDI_NBITS_IMLEN + SIZE_DMTDINFO_PRNDI_IMLEN + SIZE_MASKUSED + SIZE_MASK + SIZE_ENCRYPTION + \
		SIZE_COMPRESSION + SIZE_CHECKSUM

#define STHDR_NBLOCKS_BLOWFISH 2
#define STHDR_NBYTES_BLOWFISH (BLOCKSIZE_BLOWFISH * STHDR_NBLOCKS_BLOWFISH)

/* distribution method constants */
#define DMTD_CNSTI	1
#define DMTD_PRNDI	2

#define DMTD_CNSTI_MAX_ILEN		65535
#define DMTD_PRNDI_MAX_IMLEN	65535

/* mask constants */
#define DEFAULTMASK	0x01
#define MAXMASK	255

/* encryption constants */
#define ENC_NONE	0
#define ENC_MCRYPT	1

/* compression constants */
#define COMPR_NONE	0

/* checksum constants */
#define CHECKSUM_NONE	0
#define CHECKSUM_CRC32	1

/* max len for plain file name */
#define PLNFILENAME_MAXLEN 255

/* stego header */
extern STEGOHEADER sthdr ;

/* function prototypes */
void embeddata (BUFFER *cvrdata, unsigned long firstcvrpos, BUFFER *plndata) ;
BUFFER *extractdata (BUFFER *stgdata, unsigned long firststgpos) ;
void embedsthdr (BUFFER *cvrdata, int dmtd, DMTDINFO dmtdinfo, int enc, char *passphrase, unsigned long *firstplnpos) ;
void extractsthdr (BUFFER *stgdata, int dmtd, DMTDINFO dmtdinfo, int enc, char *passphrase, unsigned long *firstplnpos) ;
#ifdef DEBUG
void dmtd_reset (unsigned int dmtd, DMTDINFO dmtdinfo, unsigned long resetpos) ;
unsigned long dmtd_nextpos (void) ;
#endif
unsigned long calc_ubfirstplnpos (int dmtd, DMTDINFO dmtdinfo, int enc, unsigned long nbytesplain) ;
void setmaxilen (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos) ;

#endif /* ndef SH_STEGANO_H */
