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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <termios.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "msg.h"
#include "support.h"
#include "main.h"

void pverbose (char *fmt, ...)
{
	if (args.verbosity.value == ARGS_VERBOSITY_VERBOSE) {
		va_list ap ;

		va_start (ap, fmt) ;
		vfprintf (stderr, fmt, ap) ;
		va_end (ap) ;

		putc ('\n', stderr) ;
	}

	return ;
}

void pmsg (char *fmt, ...)
{
	if (args.verbosity.value != ARGS_VERBOSITY_QUIET) {
		va_list ap ;

		va_start (ap, fmt) ;
		vfprintf (stderr, fmt, ap) ;
		va_end (ap) ;

		putc ('\n', stderr) ;
	}

	return ;
}

int pquestion (char *fmt, ...)
{
	va_list ap ;
	struct termios oldattr ;
	int retval = 0 ;
	char *yeschar = _("y"), *nochar = _("n") ;
	char input[2] ;
	
	va_start (ap, fmt) ;
	vfprintf (stderr, fmt, ap) ;
	va_end (ap) ;

	fprintf (stderr, " (%s/%s) ", yeschar, nochar) ;

	oldattr = termios_singlekey_on () ;
	input[0] = getchar () ;
	input[1] = '\0' ;
	if (strcmp (input, yeschar) == 0) {
		retval = 1 ;
	}
	termios_reset (oldattr) ;

	putc ('\n', stderr) ;

	return retval ;
}

void pwarn (char *fmt, ...)
{
	if (args.verbosity.value != ARGS_VERBOSITY_QUIET) {
		va_list ap ;

		fprintf (stderr, _("%s: warning: "), PROGNAME) ;

		va_start(ap, fmt) ;
		vfprintf (stderr, fmt, ap) ;
		va_end (ap) ;

		putc ('\n', stderr) ;
	}

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
