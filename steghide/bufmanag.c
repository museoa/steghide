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
#include <assert.h>

#include "bufmanag.h"
#include "support.h"
#include "msg.h"

static BUFFER *appendnewbuf (BUFFER *buflhead) ;

int bufsize = 10240 ;

/* defines use or non-use of the noncvr buffer per file format
the array indices are the values of the FF_XXX constants */
int noncvrbufuse[] = {	0,	/* FF_UNKNOWN ; not used */
						0,	/* FF_BMP */
						1,	/* FF_WAV */
						0,	/* FF_AU */	} ;

/* creates a linked list of BUFFER structures */
BUFFER *createbuflist()
{
	return appendnewbuf (NULL) ;
}

/* append a new BUFFER structure to a list */
static BUFFER *appendnewbuf (BUFFER *buflhead)
{
	BUFFER *newbuf = NULL, *lastbuf = NULL, *endbuf = NULL ;

	lastbuf = NULL ;
	endbuf = buflhead ;
	while (endbuf != NULL) {
		lastbuf = endbuf ;
		endbuf = endbuf->next ;
	}

	newbuf = s_malloc (sizeof *newbuf) ;
	newbuf->ptr = s_malloc (bufsize) ;
	newbuf->nbytes = 0 ;
	newbuf->nbytesalloc = bufsize ;
	newbuf->next = NULL ;
	newbuf->prev = lastbuf ; /* lastbuf == NULL if newbuf is first buf */

	if (lastbuf != NULL)
		lastbuf->next = newbuf ;

	return newbuf ;
}

/* appends second to first and returns result */
BUFFER *bufcat (BUFFER *first, BUFFER *second)
{
	BUFFER *retval = NULL ;
	unsigned long readpos = 0 ;
	unsigned long writepos = 0 ;
	int c = ENDOFBUF ;

	retval = createbuflist () ;

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

	retval = createbuflist () ;
	readpos = from ;

	while ((readpos <= to) && ((c = bufgetbyte (buf, readpos)) != ENDOFBUF)) {
		bufsetbyte (retval, writepos, c) ;
		readpos++ ;
		writepos++ ;
	}

	return retval ;
}

/* returns 4 bytes in big endian byte encoding as ulong */
unsigned long bufget32_be (BUFFER *buflhead, unsigned long startpos)
{
	unsigned long retval = 0UL ;
	int byte = ENDOFBUF, i = 0 ;

	for (i = 0 ; i < 4 ; i++)
		if ((byte = bufgetbyte (buflhead, startpos + i)) == ENDOFBUF)
			retval = (retval << 8) | 0x00 ;
		else
			retval = (retval << 8) | byte ;

	return retval ;
}

/* sets 4 bytes in big endian byte encoding to a ulong */
void bufset32_be (BUFFER *buflhead, unsigned long startpos, unsigned long val)
{
	int i ;

	for (i = 0; i < 4 ; i++) {
		bufsetbyte (buflhead, startpos + i, (val & 0xFF000000) >> 24) ;
		val <<= 8 ;
	}

	return ;
}

/* returns a byte from the given buffer list */
int bufgetbyte (BUFFER *buflhead, unsigned long pos)
{
	int retval = ENDOFBUF ;

	if (pos >= buflength (buflhead)) {
		retval = ENDOFBUF ;
	}
	else {
		BUFFER *curbuf ;
		unsigned char *bufptr ;

		curbuf = buflhead ;
		while (pos >= curbuf->nbytes) {
			pos -= curbuf->nbytes ;
			curbuf = curbuf->next ;
		}
		bufptr = curbuf->ptr ;
		retval = (int) bufptr[pos] ;
	}

	return retval ;
}

/* sets the byte of the given buffer list to the given value */
void bufsetbyte (BUFFER *buflhead, unsigned long pos, int byteval)
{
	BUFFER *curbuf = NULL ;
	unsigned char *bufptr = NULL ;

	curbuf = buflhead ;
	while ((curbuf != NULL) && (pos >= curbuf->nbytesalloc)) {
		curbuf->nbytes = curbuf->nbytesalloc ;

		pos -= curbuf->nbytesalloc ;
		curbuf = curbuf->next ;
	}

	if (curbuf == NULL) {
		/* additional BUFFER(s) must be added */
		curbuf = appendnewbuf (buflhead) ;

		while (pos >= bufsize) {
			curbuf = appendnewbuf (buflhead) ;
			pos -= bufsize ;

			curbuf->prev->nbytes = curbuf->prev->nbytesalloc ;
		}
	}

	bufptr = curbuf->ptr ;
	bufptr[pos] = byteval ;
	if (pos + 1 > curbuf->nbytes) {
		assert (pos + 1 <= curbuf->nbytesalloc) ;
		curbuf->nbytes = pos + 1 ;
	}

	return ;
}

/* get a bit from the given buffer list */
int bufgetbit (BUFFER *buflhead, unsigned long bytepos, int bitpos)
{
	int retval = ENDOFBUF ;

	if (bytepos >= buflength (buflhead))
		retval = ENDOFBUF ;
	else {
		int byte = bufgetbyte (buflhead, bytepos) ;
		retval = (byte & (1 << bitpos)) >> bitpos ; 
	}

	return retval ;
}

/* sets a bit of the given buffer list to the given value */
void bufsetbit (BUFFER *buflhead, unsigned long bytepos, int bitpos, int bitval)
{
	int byte = ENDOFBUF ;
	int mask = 0 ;

	if ((byte = bufgetbyte (buflhead, bytepos)) == ENDOFBUF) {
		byte = 0 ;
	}

	mask = ~(1 << bitpos) ;
	byte = (byte & mask) | (bitval << bitpos) ;

	bufsetbyte (buflhead, bytepos, byte) ;

	return ;
}

/* returns the number of bytes that are saved in the buffer list */
unsigned long buflength (BUFFER *buflhead)
{
	BUFFER *curbuf = NULL ;
	unsigned long bytes = 0 ;

	curbuf = buflhead ;
	while (curbuf != NULL) {
		bytes += curbuf->nbytes ;
		curbuf = curbuf->next ;
	}

	return bytes ;
}

/* frees the memory that is occupied by a buffer list */
void buffree (BUFFER *buflhead)
{
	BUFFER *curbuf = buflhead, *nextbuf = NULL ;

	while (curbuf != NULL) {
		nextbuf = curbuf->next ;
		free (curbuf->ptr) ;
		free (curbuf) ;
		curbuf = nextbuf ;
	}

	return ;
}
