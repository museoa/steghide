/*
 * steghide 0.4.3 - a steganography program
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
#include <string.h>
#include <assert.h>

#include "bufmanag.h"
#include "support.h"
#include "msg.h"

#define BUFACCESS_RW_UC(BUF,POS)	(((unsigned char **) BUF->subbufs)[(POS) / BUF->subbuflength][(POS) % BUF->subbuflength])
#define BUFACCESS_RO_INT(BUF,POS)	((int) BUFACCESS_RW_UC(BUF,POS))

#define CALCNSUBBUFS(LEN,SUBBUFLEN)	(((LEN) % (SUBBUFLEN) == 0) ? ((LEN) / (SUBBUFLEN)) : (((LEN) / (SUBBUFLEN)) + 1))

int subbuflength = 32768 ; /* default sub-buffer length is 32 Kilobytes */

/* creates a linked list of BUFFER structures */
BUFFER *bufcreate (unsigned long length)
{
	BUFFER *newbuf = NULL ;
	unsigned long nsubbufs = CALCNSUBBUFS (length, subbuflength) ;
	unsigned long i = 0 ;

	newbuf = s_malloc (sizeof *newbuf) ;
	newbuf->length = length ;
	newbuf->subbuflength = subbuflength ;

	if (nsubbufs > 0) {
		newbuf->subbufs = s_malloc (nsubbufs * sizeof (void *)) ;
		for (i = 0 ; i < nsubbufs ; i++) {
			newbuf->subbufs[i] = s_calloc (subbuflength, 1) ;
		}
	}
	else {
		newbuf->subbufs = NULL ;
	}

	return newbuf ;
}

/* returns a byte from the given buffer */
int bufgetbyte (BUFFER *buf, unsigned long pos)
{
	int retval = ENDOFBUF ;

	if (pos < buf->length) {
		retval = BUFACCESS_RO_INT (buf, pos) ;
	}

	return retval ;
}

/* sets the byte of the given buffer to the given value */
void bufsetbyte (BUFFER *buf, unsigned long pos, int byteval)
{
	if (pos >= buf->length) {
		unsigned long nsubbufs = CALCNSUBBUFS (buf->length, buf->subbuflength) ;

		if (pos >= nsubbufs * buf->subbuflength) {
			/* new sub-buffer(s) must be added */
			unsigned long i = 0 ;
			unsigned long nsubbufs_toadd = CALCNSUBBUFS (pos + 1, buf->subbuflength) - nsubbufs ;

			buf->subbufs = s_realloc (buf->subbufs, (nsubbufs + nsubbufs_toadd) * sizeof (void *)) ;
			for (i = nsubbufs ; i < (nsubbufs + nsubbufs_toadd) ; i++) {
				buf->subbufs[i] = s_calloc (buf->subbuflength, 1) ;
			}
		}

		buf->length = pos + 1 ;
	}

	BUFACCESS_RW_UC (buf, pos) = (unsigned char) byteval ;

	return ;
}

/* get a bit from the given buffer */
int bufgetbit (BUFFER *buf, unsigned long bytepos, int bitpos)
{
	int retval = ENDOFBUF ;

	assert (0 <= bitpos && bitpos <= 7) ;

	if (bytepos < buf->length) {
		int byte = bufgetbyte (buf, bytepos) ;
		retval = (byte & (1 << bitpos)) >> bitpos ; 
	}

	return retval ;
}

/* sets a bit of the given buffer to the given value */
void bufsetbit (BUFFER *buf, unsigned long bytepos, int bitpos, int bitval)
{
	int byte = ENDOFBUF ;
	int mask = 0 ;

	assert (0 <= bitpos && bitpos <= 7) ;
	assert (bitval == 0 || bitval == 1) ;

	if (bytepos < buf->length) {
		byte = bufgetbyte (buf, bytepos) ;
		mask = ~(1 << bitpos) ;
		byte = (byte & mask) | (bitval << bitpos) ;
	}
	else {
		byte = bitval << bitpos ;
	}

	bufsetbyte (buf, bytepos, byte) ;

	return ;
}

/* appends second to first and returns result */
BUFFER *bufappend (BUFFER *first, BUFFER *second)
{
	BUFFER *retval = NULL ;
	unsigned long readpos = 0 ;
	unsigned long writepos = 0 ;
	int c = ENDOFBUF ;

	retval = bufcreate (first->length + second->length) ;

	while ((c = bufgetbyte (first, readpos)) != ENDOFBUF) {
		bufsetbyte (retval, writepos, c) ;
		readpos++ ;
		writepos++ ;
	}

	readpos = 0 ;
	while ((c = bufgetbyte (second, readpos)) != ENDOFBUF) {
		bufsetbyte (retval, writepos, c) ;
		readpos++ ;
		writepos++ ;
	}

	return retval ;
}

/* returns buffer that contains bytes from-to of buf */
BUFFER *bufcut (BUFFER *buf, unsigned long from, unsigned long to)
{
	BUFFER *retval = NULL ;
	unsigned long readpos = 0 ;
	unsigned long writepos = 0 ;
	int c = ENDOFBUF ;

	retval = bufcreate (to - from + 1) ;
	readpos = from ;

	while ((readpos <= to) && ((c = bufgetbyte (buf, readpos)) != ENDOFBUF)) {
		bufsetbyte (retval, writepos, c) ;
		readpos++ ;
		writepos++ ;
	}

	return retval ;
}

/* frees the memory that is occupied by a buffer */
void buffree (BUFFER *buf)
{
	unsigned long i = 0 ;
	unsigned long nsubbufs = 0 ;
	
	if (buf->length % buf->subbuflength == 0) {
		nsubbufs = buf->length / buf->subbuflength ;
	}
	else {
		nsubbufs = (buf->length / buf->subbuflength) + 1 ;
	}

	for (i = 0 ; i < nsubbufs ; i++) {
		free (buf->subbufs[i]) ;
	}

	free (buf->subbufs) ;
	free (buf) ;

	return ;
}
