/*
 * steghide 0.4.1 - a steganography program
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

#ifndef SH_MSG_H
#define SH_MSG_H

typedef struct struct_ERRMSG {
	int 	n ;
	char	msg[256] ;
} ERRMSG ;

/* error constants */
#define ERR_OK					0	/* no error */

#define ERR_NOACTION			101 /* action has not been specified */
#define ERR_NOMETHOD			102 /* method has not been specified */
#define ERR_STDINTWICE			103 /* stdin cannot be used twice */
#define ERR_ACTIONTWICE			104 /* action has been specified twice */
#define ERR_METHODTWICE			105 /* method has been specified twice */
#define ERR_METHODUNKNOWN		106	/* unknown method has been specified */
#define ERR_DATANOALGO			107 /* encryption algorithm has not been specified */
#define ERR_DATANOMODE			108 /* mode for encryption has not been specified */
#define ERR_DATANOHASH			109 /* hash function has not been specified */
#define ERR_DATANOPASSPHRASE	110 /* passphrase has not been specified */
#define ERR_DATATWOALGOS		111	/* two names for encryption algorithms were found */
#define ERR_DATATWOMODES		112	/* two names for encryption modes were found */
#define ERR_DATATWOHASHES		113 /* two names for hash functions were found */
#define ERR_DATATWOPASSPHRASES	114 /* two passphrases were found */
#define ERR_DATALIBMCRYPTALGO	115	/* libmcrypt doesn't know algorithm */
#define ERR_DATALIBMCRYPTMODE	116	/* libmcrypt doesn't know mode */
#define ERR_STHDRNOALGO			117 /* encryption algorithm has not been specified */
#define ERR_STHDRNOMODE			118 /* mode for encryption has not been specified */
#define ERR_STHDRNOHASH			119 /* hash function has not been specified */
#define ERR_STHDRNOPASSPHRASE	120 /* passphrase has not been specified */
#define ERR_STHDRTWOALGOS		121	/* two names for encryption algorithms were found */
#define ERR_STHDRTWOMODES		122	/* two names for encryption modes were found */
#define ERR_STHDRTWOHASHES		123 /* two names for hash functions were found */
#define ERR_STHDRTWOPASSPHRASES	124 /* two passphrases were found */
#define ERR_STHDRLIBMCRYPTALGO	125	/* libmcrypt doesn't know algorithm */
#define ERR_STHDRLIBMCRYPTMODE	126	/* libmcrypt doesn't know mode */
#define ERR_ARGUNKNOWN			127	/* unknown argument has been given */
#define ERR_PASSPHRASETOOLONG	128	/* the passphrase is too long */
#define ERR_NOPASSPHRASE		129 /* no passphrase has been supplied */
#define ERR_PLNFILENAMELEN		130 /* plain file name is too long */
#define ERR_NUMTOOBIG			131	/* a number is too big */
#define ERR_NOPLNFILENAME		132	/* neither in stgfile nor on commandline a plnfilename */
#define ERR_NOMASK				133 /* -m argument without number afterwards */
#define ERR_NOPLNFILENAMECMDL	134 /* -pf without filename */
#define ERR_NOCVRFILENAME		135 /* -cf without filename */
#define ERR_NOSTGFILENAME		136 /* -sf without filename */
#define ERR_MASKZERO			137 /* mask is zero */
 
#define ERR_CVROPEN				201	/* can not open cover file */
#define ERR_STGOPEN				202	/* can not open stego file */
#define ERR_PLNOPEN				203	/* can not open plain file */
#define ERR_CVRREAD				204	/* can not read from cover file */
#define ERR_STGREAD				205	/* can not read from stego file */
#define ERR_PLNREAD				206	/* can not read from plain file */
#define ERR_STGWRITE			207	/* can not write to stego file */
#define ERR_PLNWRITE			208	/* can not write to plain file */
#define ERR_CVRCLOSE			209	/* can not close cover file */
#define ERR_STGCLOSE			210	/* can not close stego file */
#define ERR_PLNCLOSE			211	/* can not close plain file */

#define ERR_STHDRVER			301	/* stego header version is not known */
#define ERR_CVRUNKNOWNFFORMAT	303	/* cover file has unknown file format */
#define ERR_STGUNKNOWNFFORMAT	304	/* stego file has unknown file format */
#define ERR_CVRUNSUPBMPFILE		305	/* cover file is unsupported bmp file (not win3.x nor OS/2 1.x) */
#define ERR_STGUNSUPBMPFILE		306	/* stego file is unsupported bmp file */
#define ERR_CVRBMPWITHCOMPR		307	/* cover file is bmp file that uses compression */
#define ERR_STGBMPWITHCOMPR		308	/* stego file is bmp file that uses compression */
#define ERR_CVRUNSUPWAVFILE		309	/* cover file is unsupported wav file (not PCM) */
#define ERR_STGUNSUPWAVFILE		310	/* stego fileis unsupported wav file */
#define ERR_CVRBPSNOTMULTOF8	311	/* cover file is wav file with bits/sample rate not a multiple of eight */
#define ERR_STGBPSNOTMULTOF8	312	/* stego file is wav file with bits/sample rate not a multiple of eight */
#define ERR_STHDRDMTDUNKNOWN	313	/* the distribution method is not known */
#define ERR_COMPRNOTIMPL		320	/* compression is not implemented yet */
#define ERR_CKSUMNOTIMPL		321	/* checksum is not implemented yet */

#define ERR_MEMALLOC			401	/* can not allocate memory */
#define ERR_CVRTOOSHORT			402	/* cover file is too short to embed plain data there */
#define ERR_STGTOOSHORT			403	/* stego file is too short to extract plain data from there */

#define ERR_MHASH				501 /* error while calling a function from libmhash */
#define ERR_LIBMCRYPT			502 /* error while calling a function from libmcrypt */

#define ERR_OTHER				600 /* just for cvs version - error handling will be changed for 0.4.2 */

/* function prototypes */
void perr (int errn) ;

#endif	/* ndef SH_MSG_H */
