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

#ifndef SH_FF_AU_H
#define SH_FF_AU_H

#include "cvrstgfile.h"

/*** au file format ***/
typedef struct struct_AU_HEADER {
	char			id[4] ;
	unsigned long	offset ;
	unsigned long	size ;
	unsigned long	encoding ;
	unsigned long	samplerate ;
	unsigned long	channels ;
} AU_HEADER ;

#define AU_SIZE_HEADER		24

typedef struct struct_AU_CONTENTS {
	AU_HEADER		*header ;
	unsigned long	len_infofield ;
	void			*infofield ;
	BUFFER			*data ;
} AU_CONTENTS ;

/* function prototypes */
void au_readfile (CVRSTGFILE *file) ;
void au_writefile (CVRSTGFILE *file) ;
unsigned long au_capacity (CVRSTGFILE *file) ;
void au_embedbit (CVRSTGFILE *file, unsigned long pos, int value) ;
int au_extractbit (CVRSTGFILE *file, unsigned long pos) ;
void au_cleanup (CVRSTGFILE *file) ;

#endif /* ndef SH_FF_AU_H */
