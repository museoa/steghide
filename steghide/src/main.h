/*
 * steghide 0.4.6b - a steganography program
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

#define PROGNAME "steghide"

#ifdef WIN32
#include <windows.h>

typedef struct struct_LCIDENTRY {
	LCID localeID ;
	char language[3] ;
} LCIDENTRY ;
#endif

#define INTERVAL_DEFAULT	10

#endif // ndef SH_MAIN_H
