/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>
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

#ifndef SH_COMMON_H
#define SH_COMMON_H

// this file contains some definitions that are/can be used throughout the whole program

// the program name
#define PROGNAME "steghide"

// include config.h
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// gettext support
#include <libintl.h>
#define _(S) gettext (S)

// every class should (be able to) do assertions
#include <cassert>

// every class has access to the command line arguments
#include "arguments.h"
extern Arguments Args ;

// every class has access to random data
#include "randomsource.h"
extern RandomSource RndSrc ;

// typedefs
#ifdef HAVE_CONFIG_H

# if SIZEOF_UNSIGNED_LONG_INT == 4
typedef unsigned long WORD32;
# elif SIZEOF_UNSIGNED_INT == 4
typedef unsigned int WORD32;
# else
#  error "There is no int with a size of 32 bits on your system. Cannot compile."
# endif

# if SIZEOF_UNSIGNED_INT == 2
typedef unsigned int WORD16;
# elif SIZEOF_UNSIGNED_SHORT_INT == 2
typedef unsigned short WORD16;
# else
#  error "There is no int with a size of 32 bits on your system. Cannot compile."
# endif

# if SIZEOF_UNSIGNED_CHAR == 1
typedef unsigned char BYTE;
# else
#  error "There is no char with a size of 32 bits on your system. Cannot compile."
# endif

#else
typedef unsigned long	WORD32 ;
typedef unsigned int	WORD16 ;
typedef unsigned char	BYTE ;
#endif

typedef int Bit ;
typedef unsigned long SamplePos ;
typedef unsigned long VertexLabel ;
typedef unsigned long SampleLabel ;

#endif // ndef SH_COMMON_H
