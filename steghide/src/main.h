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

#ifndef SH_MAIN_H
#define SH_MAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stegano.h"

#define PROGNAME	"steghide"

#define ARGS_ACTION_EMBED		1
#define ARGS_ACTION_EXTRACT		2
#define ARGS_ACTION_VERSION		3
#define ARGS_ACTION_LICENSE		4
#define ARGS_ACTION_HELP		5

#define ARGS_VERBOSITY_QUIET	-1
#define ARGS_VERBOSITY_NORMAL	0
#define ARGS_VERBOSITY_VERBOSE	1

#define DEFAULT_DMTD			DMTD_PRNDI
#define DEFAULT_PRNDIMIVAL		10
#define DEFAULT_STHDRENCRYPTION	1
#define DEFAULT_ENCRYPTION		1
#define DEFAULT_CHECKSUM		1
#define DEFAULT_EMBEDPLNFN		1
#define DEFAULT_COMPATIBILITY	0
#define DEFAULT_VERBOSITY		ARGS_VERBOSITY_NORMAL
#define DEFAULT_FORCE			0

typedef struct struct_ARGSTRING {
	int is_set ;
	char *value ;
} ARGSTRING ;

typedef struct struct_ARGINT {
	int is_set ;
	int value ;
} ARGINT ;
typedef ARGINT ARGBOOL ;

typedef struct struct_ARGDMTD {
	int dmtd_is_set ;
	int maxilen_is_set ;
	unsigned int dmtd ;
	DMTDINFO dmtdinfo ;
} ARGDMTD ;

typedef struct struct_ARGS {
	ARGINT action ;
	ARGDMTD dmtd ;
	ARGBOOL sthdrencryption ;
	ARGBOOL encryption ;
	ARGBOOL checksum ;
	ARGBOOL embedplnfn ;
	ARGBOOL compatibility ;
	ARGINT verbosity ;
	ARGBOOL force ;
	ARGSTRING cvrfn ;
	ARGSTRING plnfn ;
	ARGSTRING stgfn ;
	ARGSTRING passphrase ;
} ARGS ;

extern ARGS args ;

#ifdef WIN32
#include <windows.h>

typedef struct struct_LCIDENTRY {
	LCID localeID ;
	char language[3] ;
} LCIDENTRY ;
#endif

#define INTERVAL_DEFAULT	10

#endif /* ndef SH_MAIN_H */
