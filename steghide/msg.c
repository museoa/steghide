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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <termios.h>

#include "msg.h"
#include "support.h"
#include "main.h"

void pmsg (char *fmt, ...)
{
	va_list ap ;

	va_start(ap, fmt) ;
	vfprintf (stderr, fmt, ap) ;
	va_end (ap) ;

	putc ('\n', stderr) ;

	return ;
}

int pquestion (char *fmt, ...)
{
	va_list ap ;
	struct termios oldattr ;
	int retval = 0 ;

	va_start (ap, fmt) ;
	vfprintf (stderr, fmt, ap) ;
	va_end (ap) ;

	fprintf (stderr, " (y/n) ") ;

	oldattr = termios_singlekey_on () ;
	if (getchar () == 'y') {
		retval = 1 ;
	}
	termios_reset (oldattr) ;

	putc ('\n', stderr) ;

	return retval ;
}

void pwarn (char *fmt, ...)
{
	va_list ap ;

	fprintf (stderr, "%s: warning: ", PROGNAME) ;

	va_start(ap, fmt) ;
	vfprintf (stderr, fmt, ap) ;
	va_end (ap) ;

	putc ('\n', stderr) ;

	return ;
}

void exit_err (char *fmt, ...)
{
	va_list ap ;

	fprintf (stderr, "%s: ", PROGNAME) ;

	va_start(ap, fmt) ;
	vfprintf (stderr, fmt, ap) ;
	va_end (ap) ;

	putc ('\n', stderr) ;

	exit (EXIT_FAILURE) ;
}
