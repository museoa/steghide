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

#ifndef SH_PLNFILE_H
#define SH_PLNFILE_H

#include "bufmanag.h"

/* PLNFILE describes a plaindata file */
typedef struct struct_PLNFILE {	
	/* file as data stream */
	FILE	*stream ;
	/* name of file */
	char	*filename ;
	/* first element of linked list of buffers that contain the data of the file */
	BUFFER	*plndata ;
} PLNFILE ;

/* function prototypes */
PLNFILE *pln_readfile (char *filename) ;
void pln_writefile (PLNFILE *plnfile) ;
void assemble_plndata (PLNFILE *plnfile) ;
void deassemble_plndata (PLNFILE *plnfile) ;
PLNFILE *pln_createfile (void) ;
void pln_cleanup (PLNFILE *plnfile) ;

#endif /* SH_PLNFILE_H */
