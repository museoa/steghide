/*
 * steghide 0.5.1 - a steganography program
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

#ifndef SH_COMMON_H
#define SH_COMMON_H

// this file contains some definitions that are/can be used throughout the whole program

// the program name
#define PROGNAME "steghide"

// gettext support
#include <libintl.h>
#define _(S) gettext (S)

// include config.h
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// every class should (be able to) do assertions
#include <cassert>

// every class has access to the command line arguments
#include "arguments.h"
extern Arguments Args ;

// every class has access to random data
#include "randomsource.h"
extern RandomSource RndSrc ;

// typedefs
typedef int Bit ;
typedef unsigned long SamplePos ;

#endif // ndef SH_COMMON_H
