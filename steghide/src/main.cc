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

#include <cstdlib>

#include "common.h"
#include "error.h"
#include "session.h"

#ifdef WIN32 // locale support on Windows

#include <windows.h>

typedef struct struct_LCIDENTRY {
	LCID localeID ;
	char language[3] ;
} LCIDENTRY ;

LCIDENTRY LCIDTable[] = {
	// french
	{ 0x040c, "fr" },	// France
	{ 0x080c, "fr" },	// Belgium
	{ 0x0c0c, "fr" },	// Canada
	{ 0x100c, "fr" },	// Switzerland
	{ 0x140c, "fr" },	// Luxembourg
	{ 0x180c, "fr" },	// Monaco
	// german
	{ 0x0407, "de" },	// Germany
	{ 0x0807, "de" },	// Switzerland
	{ 0x0c07, "de" },	// Austria
	{ 0x1007, "de" },	// Luxembourg
	{ 0x1407, "de" },	// Liechtenstein
	// end of LCIDTable
	{ 0x0000, "__" }
} ;

#undef LOCALEDIR
#define LOCALEDIR	"./locale/"

#endif // WIN32

static void gettext_init (void) ;

int main (int argc, char *argv[])
{
	try {
		gettext_init () ;
        Args = Arguments (argc, argv) ;
		RndSrc = RandomSource() ;

        Session s ;
        s.run() ;
    }
    catch (SteghideError e) {
        e.printMessage() ;
        exit(EXIT_FAILURE) ;
    }

    exit(EXIT_SUCCESS) ;
}

static void gettext_init (void)
{
#ifndef DEBUG
	/* initialize gettext */
	setlocale (LC_ALL, "") ;
	bindtextdomain (PACKAGE, LOCALEDIR) ;
	textdomain (PACKAGE) ;

#ifdef WIN32
	/* using the Windows API to find out which language should be used
	   (as there is no environment variable indicating the language) */
	{
		LCID localeID = GetThreadLocale () ;	
		int i = 0 ;

		while (LCIDTable[i].localeID != 0x0000) {
			if (localeID == LCIDTable[i].localeID) {
				setenv ("LANG", LCIDTable[i].language, 1) ;
				/* Make Change known (see gettext manual) */
				{
					extern int _nl_msg_cat_cntr ;
					++_nl_msg_cat_cntr;
				}
				break ;
			}

			i++ ;
		}
	}
#endif // ndef WIN32
#endif // ndef DEBUG
	return ;
}
