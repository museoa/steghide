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

#ifndef SH_BUFMANAG_H
#define SH_BUFMANAG_H

typedef struct struct_BUFFER {
	/* number of bytes stored in this buffer */
	unsigned long length ;
	/* length of one subbuffer (in bytes) */
	unsigned long subbuflength ;
	/* array of pointers to subbuffers */
	void **subbufs ;
} BUFFER ;

#define ENDOFBUF -1

extern int subbuflength ;

/* function prototypes */
BUFFER *bufcreate (unsigned long length) ;
int bufgetbyte (BUFFER *buf, unsigned long pos) ;
void bufsetbyte (BUFFER *buf, unsigned long pos, int byteval) ;
int bufgetbit (BUFFER *buf, unsigned long bytepos, int bitpos) ;
void bufsetbit (BUFFER *buf, unsigned long bytepos, int bitpos, int bitval) ;
BUFFER *bufappend (BUFFER *first, BUFFER *second) ;
BUFFER *bufcut (BUFFER *buf, unsigned long from, unsigned long to) ;
void buffree (BUFFER *buf) ;

#endif /* ndef SH_BUFMANAG_H */
