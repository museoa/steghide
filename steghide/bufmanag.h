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

#ifndef SH_BUFMANAG_H
#define SH_BUFMANAG_H

typedef struct struct_BUFFER {
	/* pointer to the data of this BUFFER structure */
	void	*ptr ;
	/* number of allocated bytes that are filled with data */
	long	nbytes ;
	/* number of bytes that have been allocated */
	long	nbytesalloc ;
	/* pointer to previous buffer in linked list ((x->prev == NULL) indicates first element) */
	struct struct_BUFFER	*prev ;
	/* pointer to next buffer in linked list ((x->next == NULL) indicates last element) */
	struct struct_BUFFER	*next ;
} BUFFER ;

#define ENDOFBUF		-1

/* is used for parameter freesubstructs of cleanupcvrfile */
#define FSS_NO	0
#define FSS_YES	1


extern int bufsize ;
extern int noncvrbufuse[] ;

/* function prototypes */
BUFFER *createbuflist(void) ;
BUFFER *bufcat (BUFFER *dest, BUFFER *src) ;
BUFFER *bufcut (BUFFER *buf, unsigned long from, unsigned long to) ;
unsigned long bufget32_be(BUFFER *buflhead, unsigned long startpos) ;
void bufset32_be(BUFFER *buflhead, unsigned long startpos, unsigned long val) ;
int bufgetbyte(BUFFER *buflhead, unsigned long pos) ;
void bufsetbyte(BUFFER *buflhead, unsigned long pos, int byteval) ;
int bufgetbit(BUFFER *buflhead, unsigned long bytepos, int bitpos) ;
void bufsetbit(BUFFER *buflhead, unsigned long bytepos, int bitpos, int bitval) ;
unsigned long buflength(BUFFER *buflhead) ;
void buffree(BUFFER *buflhead) ;

#endif /* ndef SH_BUFMANAG_H */
