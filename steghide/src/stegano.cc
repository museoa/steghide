/*
 * steghide 0.4.5 - a steganography program
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "stegano.h"
#include "bufmanag.h"
#include "support.h"
#include "crypto.h"
#include "msg.h"
#include "main.h"

STEGOHEADER sthdr = { 0, 0, { { 0 } }, '\0', 0, 0, 0 } ;

static int nstgbits (void) ;
#ifndef DEBUG
void dmtd_reset (unsigned int dmtd, DMTDINFO dmtdinfo, unsigned long resetpos) ;
unsigned long dmtd_nextpos (void) ;
#endif
static unsigned int findmaxilen_cnsti (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos) ;
static unsigned int findmaxilen_prndi (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos) ;
static int simprndi_ok (unsigned long cvrbytes, unsigned long plnbits, unsigned long firstplnpos, unsigned int imlen) ;

static unsigned int curdmtd_dmtd ;
static DMTDINFO curdmtd_dmtdinfo ;
static unsigned long curdmtd_curpos ;

/* embed plain data in cover data (resulting in stego data) */
void embeddata (CvrStgFile *cvrstgfile, unsigned long firstcvrpos, PLNFILE *plnfile)
{
	int plnbits = 0 ;
	int bit = 0 ;
	int i = 0 ;
	unsigned long plnpos_byte = 0, plnpos_bit = 0 ;
	unsigned long cvrpos_byte = firstcvrpos ;

	pverbose (_("embedding plain data.")) ;

	dmtd_reset (sthdr.dmtd, sthdr.dmtdinfo, cvrpos_byte) ;

	while (plnpos_byte < plnfile->plndata->length) {
		plnbits = 0 ;
		for (i = 0 ; i < nstgbits() ; i++) {
			if ((bit = bufgetbit (plnfile->plndata, plnpos_byte, plnpos_bit)) != ENDOFBUF) {
				plnbits |= (bit << i) ;
				if (plnpos_bit == 7) {
					plnpos_byte++ ;
					plnpos_bit = 0 ;
				}
				else {
					plnpos_bit++ ;
				}
			}
		}

		/* FIXME DELME
		cvrbyte = bufgetbyte (cvrdata, cvrpos_byte) ;
		bufsetbyte (cvrdata, cvrpos_byte, setbits (cvrbyte, plnbits)) ;
		*/
		cvrstgfile->embedBit (cvrpos_byte, plnbits) ;
		
		cvrpos_byte = dmtd_nextpos () ;

		if ((cvrpos_byte >= cvrstgfile->getCapacity()) && (plnpos_byte < plnfile->plndata->length)) {
			exit_err (_("the cover file is too short to embed the plain data. try a smaller interval length.")) ;
		}
	}

	return ;
}

/* extracts plain data (return value) from stego data */
BUFFER *extractdata (CvrStgFile *cvrstgfile, unsigned long firststgpos)
{
	BUFFER *plndata = NULL ;
	int plnbits = 0, i = 0 ;
	unsigned long plnpos_byte = 0, plnpos_bit = 0 ;
	unsigned long size = 0 ;
	unsigned long stgpos_byte = firststgpos ;

	pverbose (_("extracting plain data.")) ;

	dmtd_reset (sthdr.dmtd, sthdr.dmtdinfo, stgpos_byte) ;

	plndata = bufcreate (0) ; /* FIXME - length should be set more intelligently */

	if (sthdr.encryption == ENC_MCRYPT) {
		if (sthdr.nbytesplain % BLOCKSIZE_BLOWFISH == 0) {
			size = sthdr.nbytesplain + BLOCKSIZE_BLOWFISH /* IV */ ;
		}
		else {
			size = (((sthdr.nbytesplain / BLOCKSIZE_BLOWFISH) + 1) * BLOCKSIZE_BLOWFISH) + BLOCKSIZE_BLOWFISH /* IV */ ;
		}
	}
	else {
		size = sthdr.nbytesplain ;
	}

	while (plnpos_byte < size) {
		/* FIXME DELME
		plnbits = getbits (bufgetbyte (stgdata, stgpos_byte)) ;
		*/
		plnbits = cvrstgfile->extractBit (stgpos_byte) ;
		for (i = 0 ; i < nstgbits() ; i++) {
			if (plnpos_byte < size) {
				bufsetbit (plndata, plnpos_byte, plnpos_bit, ((plnbits & (1 << i)) >> i)) ;
				if (plnpos_bit == 7) {
					plnpos_byte++ ;
					plnpos_bit = 0 ;
				}
				else {
					plnpos_bit++ ;
				}
			}
		}

		stgpos_byte = dmtd_nextpos() ;

		if ((stgpos_byte >= cvrstgfile->getCapacity()) && (plnpos_byte < size)) {
			exit_err (_("the stego file is too short to contain the plain data (file corruption ?).")) ;
		}
	}

	return plndata ;
}

void embedsthdr (CvrStgFile *cvrstgfile, int dmtd, DMTDINFO dmtdinfo, int enc, char *passphrase, unsigned long *firstplnpos)
{
	unsigned int hdrbuflen = STHDR_NBYTES_BLOWFISH ;
	unsigned char *hdrbuf = NULL ;
	unsigned int bit = 0, sthdrbuflen = 0 ;
	unsigned int bitval = 0 ;
	unsigned long cvrbytepos = 0 ;

	pverbose (_("embedding stego header.")) ;

	hdrbuf = (unsigned char *) s_calloc (STHDR_NBYTES_BLOWFISH, 1) ;

	/* assemble bits that make up sthdr in a buffer */
	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) nbits (sthdr.nbytesplain), SIZE_NBITS_NBYTESPLAIN) ;
	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.nbytesplain, nbits (sthdr.nbytesplain)) ;
	
	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.dmtd, SIZE_DMTD) ;
	switch (sthdr.dmtd) {
		case DMTD_CNSTI:
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) nbits (sthdr.dmtdinfo.cnsti.interval_len), SIZE_DMTDINFO_CNSTI_NBITS_ILEN) ;
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.dmtdinfo.cnsti.interval_len, nbits (sthdr.dmtdinfo.cnsti.interval_len)) ;
		break ;

		case DMTD_PRNDI:
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.dmtdinfo.prndi.seed, SIZE_DMTDINFO_PRNDI_SEED) ;
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) nbits (sthdr.dmtdinfo.prndi.interval_maxlen), SIZE_DMTDINFO_PRNDI_NBITS_IMLEN) ;
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.dmtdinfo.prndi.interval_maxlen, nbits (sthdr.dmtdinfo.prndi.interval_maxlen)) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	if (sthdr.mask == DEFAULTMASK) {
		bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) 0, SIZE_MASKUSED) ;
	}
	else {
		bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) 1, SIZE_MASKUSED) ;
		bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.mask, SIZE_MASK) ;
	}

	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.encryption, SIZE_ENCRYPTION) ;

	/* compression is not yet implemented */
	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) COMPR_NONE, SIZE_COMPRESSION) ;

	bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) sthdr.checksum, SIZE_CHECKSUM) ;

	/* eventually encrypt the buffer */
	if (enc) {
		/* pad with random bits */
		while (bit < hdrbuflen * 8) {
			bit = cp_bits_to_buf_le (hdrbuf, bit, (unsigned long) (2.0 * rand() / (RAND_MAX + 1.0)), 1) ;
		}
		assert (bit == hdrbuflen * 8) ;

		encrypt_sthdr (hdrbuf, hdrbuflen, passphrase) ;
	}

	/* embed the buffer */
	sthdrbuflen = bit ;
	dmtd_reset (dmtd, dmtdinfo, 0) ;
	cvrbytepos = 0 ;
	for (bit = 0 ; bit < sthdrbuflen ; bit++) {
		bitval = (hdrbuf[bit / 8] & (1 << (bit % 8))) >> (bit % 8) ;
		/* FIXME DELME
		bufsetbit (cvrdata, cvrbytepos, 0, bitval) ;
		*/
		cvrstgfile->embedBit (cvrbytepos, bitval) ;
		if (((cvrbytepos = dmtd_nextpos()) >= cvrstgfile->getCapacity()) && (bit < sthdrbuflen)) {
			exit_err (_("the cover file is too short to embed the stego header. use another cover file.")) ;
		}
	}

	free (hdrbuf) ;
	
	*firstplnpos = cvrbytepos ;
}	

void extractsthdr (CvrStgFile *cvrstgfile, int dmtd, DMTDINFO dmtdinfo, int enc, char *passphrase, unsigned long *firstplnpos)
{
	unsigned int hdrbuflen = STHDR_NBYTES_BLOWFISH ;
	unsigned char hdrbuf[STHDR_NBYTES_BLOWFISH] ;
	unsigned long oldcvrbytepos[(8 * STHDR_NBYTES_BLOWFISH) + 1] ;
	unsigned long cvrbytepos = 0 ;
	int bitval = 0 ;
	unsigned long nbits = 0 ;
	unsigned long tmp = 0 ;
	unsigned int bit = 0 ;
	int i = 0 ;

	pverbose (_("extracting stego header.")) ;

	dmtd_reset (dmtd, dmtdinfo, 0) ;

	for (i = 0 ; i < BLOCKSIZE_BLOWFISH * STHDR_NBLOCKS_BLOWFISH ; i++) {
		hdrbuf[i] = 0 ;
	}

	/* this is a pretty dirty hack, but it prevents duplicating a lot of the code:
	   we read hdrbuflen bits even if the sthdr is not encrypted, thus
	   having read bits at the end of hdrbuf that have nothing to do with the sthdr.
	   After that we set the dmtd position generator back if sthdr is not encrypted */

	for (bit = 0 ; bit < hdrbuflen * 8 ; bit++) {
		oldcvrbytepos[bit] = cvrbytepos ;
		/* FIXME DELME
		bitval = bufgetbit (stgdata, cvrbytepos, 0) ;
		*/
		bitval = cvrstgfile->extractBit (cvrbytepos) ;
		hdrbuf[bit / 8] |= bitval << (bit % 8) ;
		if (((cvrbytepos = dmtd_nextpos()) >= cvrstgfile->getCapacity()) &&
		   bit < hdrbuflen * 8) {
			exit_err (_("the stego file is too short to contain the stego header (file corruption ?).")) ;
		}
	}
	oldcvrbytepos[bit] = cvrbytepos ;

	if (enc) {
		decrypt_sthdr (hdrbuf, hdrbuflen, passphrase) ;
	}

	/* copy the values from the buffer into the sthdr structure */
	bit = 0 ;
	bit = cp_bits_from_buf_le (hdrbuf, bit, &nbits, SIZE_NBITS_NBYTESPLAIN) ;
	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, nbits) ;
	sthdr.nbytesplain = (unsigned long) tmp ;

	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_DMTD) ;
	sthdr.dmtd = (unsigned int) tmp ;
	switch (sthdr.dmtd) {
		case DMTD_CNSTI:
			bit = cp_bits_from_buf_le (hdrbuf, bit, &nbits, SIZE_DMTDINFO_CNSTI_NBITS_ILEN) ;
			bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, nbits) ;
			sthdr.dmtdinfo.cnsti.interval_len = (unsigned int) tmp ;
		break ;

		case DMTD_PRNDI:
			bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_DMTDINFO_PRNDI_SEED) ;
			sthdr.dmtdinfo.prndi.seed = (unsigned long) tmp ;
			bit = cp_bits_from_buf_le (hdrbuf, bit, &nbits, SIZE_DMTDINFO_PRNDI_NBITS_IMLEN) ;
			bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, nbits) ;
			sthdr.dmtdinfo.prndi.interval_maxlen = (unsigned int) tmp ;
		break ;

		default:
			exit_err (_("the distribution method saved in the stego header is unknown (file corruption ?).")) ;
		break ;
	}

	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_MASKUSED) ;
	if (tmp) {
		bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_MASK) ;
		if (tmp == 0) {
			exit_err (_("the mask saved in the stego header is zero (file corruption ?).")) ;
		}
		sthdr.mask = (unsigned int) tmp ;
		exit_err (_("the mask saved in the stego header is not the default (1). This is no longer supported.")) ;
	}
	else {
		sthdr.mask = 1 ;
	}

	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_ENCRYPTION) ;
	sthdr.encryption = (unsigned int) tmp ;

	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_COMPRESSION) ;
	sthdr.compression = (unsigned int) tmp ;
	if (sthdr.compression != COMPR_NONE) {
		exit_err (_("the plain data is compressed. this is not implemented yet (file corruption ?).")) ;
	}

	bit = cp_bits_from_buf_le (hdrbuf, bit, &tmp, SIZE_CHECKSUM) ;
	sthdr.checksum = (unsigned int) tmp ;

	/* set *firstplnpos */
	if (enc) {
		*firstplnpos = oldcvrbytepos[hdrbuflen * 8] ;
	}
	else {
		*firstplnpos = oldcvrbytepos[bit] ;
	}

	return ;
}

/* returns the number of set bits in sthdr.mask */
static int nstgbits (void)
{
	int i = 0, n = 0 ;

	for (i = 0; i < 8; i++)
		if (sthdr.mask & (1 << i))
			n++ ;

	/* FIXME muss jetzt 1 sein - keine Möglichkeit mehr andere mask zu behandeln - ? Funktion raushauen */
	assert (n == 1) ;

	return n ;
}

void dmtd_reset (unsigned int dmtd, DMTDINFO dmtdinfo, unsigned long resetpos)
{
	switch (dmtd) {
		case DMTD_CNSTI:
		/* no initialization code necessary */
		break ;

		case DMTD_PRNDI:
		srnd (dmtdinfo.prndi.seed) ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	curdmtd_dmtd = dmtd ;
	curdmtd_dmtdinfo = dmtdinfo ;
	curdmtd_curpos = resetpos ;

	return ;
}

unsigned long dmtd_nextpos (void)
{
	switch (curdmtd_dmtd) {
		case DMTD_CNSTI:
		curdmtd_curpos += curdmtd_dmtdinfo.cnsti.interval_len + 1 ;
		break ;

		case DMTD_PRNDI:
		curdmtd_curpos += rnd (curdmtd_dmtdinfo.prndi.interval_maxlen) + 1 ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	return curdmtd_curpos ;
}

void setmaxilen (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos)
{
	unsigned int maxilen = 0 ;

	switch (args->dmtd.getValue()) {
		case DMTD_CNSTI:
		maxilen = findmaxilen_cnsti (cvrbytes, plnbytes, firstplnpos) ;
		if (maxilen > DMTD_CNSTI_MAX_ILEN) {
			maxilen = DMTD_CNSTI_MAX_ILEN ;
		}
		pverbose (_("setting interval length to %d."), maxilen) ;
		sthdr.dmtdinfo.cnsti.interval_len = maxilen ;
		break ;

		case DMTD_PRNDI:
		maxilen = findmaxilen_prndi (cvrbytes, plnbytes, firstplnpos) ;
		if (maxilen > DMTD_PRNDI_MAX_IMLEN) {
			maxilen = DMTD_PRNDI_MAX_IMLEN ;
		}
		pverbose (_("setting maximum interval length to %d."), maxilen) ;
		sthdr.dmtdinfo.prndi.interval_maxlen = maxilen ;
		break;

		default:
		assert (0) ;
		break ;
	}

	return ;
}

static unsigned int findmaxilen_cnsti (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos)
{
	unsigned long plnbits = 0, restcvrbytes = 0 ;

	plnbits = 8 * plnbytes ;
	restcvrbytes = cvrbytes - firstplnpos ;

	return (restcvrbytes - plnbits) / (plnbits - 1) ;
}

static unsigned int findmaxilen_prndi (unsigned long cvrbytes, unsigned long plnbytes, unsigned long firstplnpos)
{
	unsigned long plnbits = 0, restcvrbytes = 0 ;
	unsigned int est_imlen = 0 ;
	int est_ok[2] = { 0, 0 } ;

	plnbits = 8 * plnbytes ;
	restcvrbytes = cvrbytes - firstplnpos ;

	est_imlen = 2 * ((cvrbytes - plnbits) / (plnbits - 1)) ;

	est_ok[0] = simprndi_ok (cvrbytes, plnbits, firstplnpos, est_imlen) ;
	est_ok[1] = simprndi_ok (cvrbytes, plnbits, firstplnpos, est_imlen + 1) ;

	while (!(est_ok[0] && !est_ok[1])) {
		if (est_ok[0] && est_ok[1]) {
			est_imlen++ ;
			est_ok[0] = est_ok[1] ;
			est_ok[1] = simprndi_ok (cvrbytes, plnbits, firstplnpos, est_imlen + 1) ;
		}

		else if (!est_ok[0] && !est_ok[1]) {
			est_imlen-- ;
			est_ok[1] = est_ok[0] ;
			est_ok[0] = simprndi_ok (cvrbytes, plnbits, firstplnpos, est_imlen) ;
		}

		else {
			assert (0) ;
		}
	}

	return est_imlen ;
}

static int simprndi_ok (unsigned long cvrbytes, unsigned long plnbits, unsigned long firstplnpos, unsigned int imlen)
{	
	unsigned long plnpos_bit = 0 ;
	unsigned long cvrpos_byte = firstplnpos ;
	int retval = 1 ;
	DMTDINFO simdmtdinfo ;

	simdmtdinfo.prndi.seed = sthdr.dmtdinfo.prndi.seed ;
	simdmtdinfo.prndi.interval_maxlen = imlen ;

	dmtd_reset (DMTD_PRNDI, simdmtdinfo, cvrpos_byte) ;

	while (plnpos_bit < plnbits) {
		plnpos_bit++ ;
		cvrpos_byte = dmtd_nextpos () ;

		if ((cvrpos_byte >= cvrbytes) && (plnpos_bit < plnbits)) {
			retval = 0 ;
			break ;
		}
	}

	return retval ;
}

/* calculates an upper bound for the first postion of a plain data bit in the cover file
   by simulating the sthdr embedding (with maximal values for the interval length) */
unsigned long calc_ubfirstplnpos (int dmtd, DMTDINFO dmtdinfo, int enc, unsigned long nbytesplain)
{
	unsigned int bit = 0, sthdrbuflen = 0 ;
	unsigned long cvrbytepos = 0 ;

	if (enc) {
		bit = STHDR_NBYTES_BLOWFISH * 8 ;
	}
	else {
		bit += SIZE_NBITS_NBYTESPLAIN ;
		bit += nbits (nbytesplain) ;
	
		bit += SIZE_DMTD ;
		switch (args->dmtd.getValue()) {
			case DMTD_CNSTI:
				bit += SIZE_DMTDINFO_CNSTI_NBITS_ILEN ;
				bit += MAXSIZE_DMTDINFO_CNSTI_ILEN ;
			break ;

			case DMTD_PRNDI:
				bit += SIZE_DMTDINFO_PRNDI_SEED ;
				bit += SIZE_DMTDINFO_PRNDI_NBITS_IMLEN ;
				bit += MAXSIZE_DMTDINFO_PRNDI_IMLEN ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		bit += SIZE_MASKUSED ;	/* backwards compatibility - mask is always default */
		bit += SIZE_ENCRYPTION ;	
		bit += SIZE_COMPRESSION ;
		bit += SIZE_CHECKSUM ;
	}

	/* simulate embedding of the buffer */
	sthdrbuflen = bit ;
	dmtd_reset (dmtd, dmtdinfo, 0) ;
	for (bit = 0 ; bit < sthdrbuflen ; bit++) {
		cvrbytepos = dmtd_nextpos () ;
	}

	return cvrbytepos ;
}
