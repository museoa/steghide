/*
 * steghide 0.4.6 - a steganography program
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "error.h"
#include "main.h"
#include "cvrstgfile.h"
#include "plnfile.h"
#include "crypto.h"
#include "hash.h"
#include "stegano.h"
#include "msg.h"
#include "support.h"

#ifdef WIN32
/* locale support on Windows */
#include <windows.h>

LCIDENTRY LCIDTable[] = {
	/* french */
	{ 0x040c, "fr" },	/* France */
	{ 0x080c, "fr" },	/* Belgium */
	{ 0x0c0c, "fr" },	/* Canada */
	{ 0x100c, "fr" },	/* Switzerland */
	{ 0x140c, "fr" },	/* Luxembourg */
	{ 0x180c, "fr" },	/* Monaco */
	/* german */
	{ 0x0407, "de" },	/* Germany */
	{ 0x0807, "de" },	/* Switzerland */
	{ 0x0c07, "de" },	/* Austria */
	{ 0x1007, "de" },	/* Luxembourg */
	{ 0x1407, "de" },	/* Liechtenstein */
	/* end of LCIDTable */
	{ 0x0000, "__" }
} ;

#undef LOCALEDIR
#define LOCALEDIR	"./locale/"
#endif /* WIN32 */
 
#ifdef DEBUG
#include "test.h"
#endif

/* how to embed the stego header */
unsigned int sthdr_dmtd = 0 ;
DMTDINFO sthdr_dmtdinfo ;

#ifndef DEBUG
static void gettext_init (void) ;
#endif /* DEBUG */
static void setsthdrdmtd (void) ;
static void version (void) ;
static void usage (void) ;
static void license (void) ;
static void embedfile (string cvrfilename, string stgfilename, string plnfilename) ;
static void extractfile (string stgfilename, string plnfilename) ;
static void cleanup (void) ;

int main (int argc, char *argv[])
{
	try {
#ifndef DEBUG
		gettext_init () ;
#endif

		/* 	the C "rand" generator is used if random numbers need not be reproduceable,
			the random number generator in support.c "rnd" is used if numbers must be reproduceable */
		srand ((unsigned int) time (NULL)) ;

		args = new Arguments (argc, argv) ;

		switch (args->command.getValue()) {
			case EMBED:
			embedfile (args->cvrfn.getValue(), args->stgfn.getValue(), args->plnfn.getValue()) ;
			break ;

			case EXTRACT:
			extractfile (args->stgfn.getValue(), args->plnfn.getValue()) ;
			break ;

			case SHOWVERSION:
			version () ;
			break ;

			case SHOWLICENSE:
			license () ;
			break ;

			case SHOWHELP:
			usage () ;
			break ;

			default:
			assert (0) ;
			break ;
		}

		cleanup () ;
	}
	catch (SteghideError e) {
		e.printMessage () ;
		exit (EXIT_FAILURE) ;
	}

	exit (EXIT_SUCCESS) ;
}

#ifndef DEBUG
static void gettext_init (void)
{
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
#endif /* WIN32 */

	return ;
}
#endif /* DEBUG */

static void setsthdrdmtd (void)
{
	/* set embedding of stego header */
	sthdr_dmtd = DMTD_PRNDI ;
	sthdr_dmtdinfo.prndi.seed = getseed ((char *) args->passphrase.getValue().c_str()) ;
	sthdr_dmtdinfo.prndi.interval_maxlen = 2 * INTERVAL_DEFAULT ;

	return ;
}

static void fillsthdr (unsigned long nbytescvrbuf, unsigned long nbytesplain, unsigned long nbytesenc)
{
	/* fill stego header with values */
	sthdr.nbytesplain = nbytesplain ;

	sthdr.dmtd = args->dmtd.getValue() ;

	switch (sthdr.dmtd) {
		case DMTD_CNSTI:
			if (args->dmtdinfo.is_set()) {
				DMTDINFO di = args->dmtdinfo.getValue() ;
				sthdr.dmtdinfo.cnsti.interval_len = di.cnsti.interval_len ;
			}
			else {
				unsigned long ubfirstplnpos = calc_ubfirstplnpos(sthdr_dmtd, sthdr_dmtdinfo, args->sthdrencryption.getValue(), nbytesplain) ;
				setmaxilen (nbytescvrbuf, nbytesenc, ubfirstplnpos) ;
			}
		break ;

		case DMTD_PRNDI:
			DMTDINFO di = args->dmtdinfo.getValue() ;
			sthdr.dmtdinfo.prndi.seed = di.prndi.seed ;
			if (args->dmtdinfo.is_set()) {
				sthdr.dmtdinfo.prndi.interval_maxlen = di.prndi.interval_maxlen ;
			}
			else {
				unsigned long ubfirstplnpos = calc_ubfirstplnpos(sthdr_dmtd, sthdr_dmtdinfo, args->sthdrencryption.getValue(), nbytesplain) ;
				setmaxilen (nbytescvrbuf, nbytesenc, ubfirstplnpos) ;
			}
		break ;

		default:
			assert (0) ;
		break ;
	}

	sthdr.mask = DEFAULTMASK ; /* only for backwards compatibility to 0.4.x versions */

	if (args->encryption.getValue()) {
		sthdr.encryption = ENC_MCRYPT ;
	}
	else {
		sthdr.encryption = ENC_NONE ;
	}

	/* compression is not yet implemented but included
	   to enable 0.4.6 to read not compressed post 0.4.6 files */
	sthdr.compression = COMPR_NONE ;

	if (args->checksum.getValue()) {
		sthdr.checksum = CHECKSUM_CRC32 ;
	}
	else {
		sthdr.checksum = CHECKSUM_NONE ;
	}
}

static void version (void)
{
	printf ("steghide version %s\n", VERSION) ;

	return ;
}

static void usage (void)
{
	version () ;
	printf (_("\n"
		"the first argument must be one of the following:\n"
		" embed, --embed          embed plain data in cover data\n"
		" extract, --extract      extract plain data from stego data\n"
		" version, --version      display version information\n"
		" license, --license      display steghide's license\n"
		" help, --help            display this usage information\n"

		"\noptions for data embedding only:\n"

		" -d, --distribution      distribution method for secret bits in cover bits\n"
		"   -d cnsti <n>          constant intervals, length: <n>\n"
		"   -d prndi <n>          pseudo-random intervals, maximum interval length: <n>\n"
		" -cf, --coverfile        select cover file\n"
		"   -cf <filename>        use <filename> as cover file\n"
		" -e, --encryption        encrypt plain data before embedding (default)\n"
		" -E, --noencryption      do not encrypt plain data before embedding\n"
		" -k, --checksum          embed crc32 checksum of plain data (default)\n"
		" -K, --nochecksum        do not embed crc32 checksum of plain data\n"
		" -n, --embedplainname    embed the name of the plain file (default)\n"
		" -N, --notembedplainname do not embed the name of the plain file\n"

		"\noptions for embedding and extracting:\n"

		" -p, --passphrase        specify passphrase (mandatory)\n"
		"   -p <passphrase>       use <passphrase> as passphrase\n"
		" -sf, --stegofile        select stego file\n"
		"   -sf <filename>        use <filename> as stego file\n"
		" -pf, --plainfile        select plain file\n"
		"   -pf <filename>        use <filename> as plain file\n"
		" -h, --sthdrencryption   encrypt stego header before embedding (default)\n"
		" -H, --nosthdrencryption do not encrypt stego header before embedding\n"
		" -c, --compatibility     compatibility with libmcrypt version < 2.4.9\n"
		" -f, --force             overwrite existing files\n"
		" -q, --quiet             suppress information messages\n"
		" -v, --verbose           display detailed information\n"

		"\nIf a <filename> is \"-\", stdin or stdout is used.\n\n"

		"The arguments default to reasonable values.\n"
		"For embedding the following line is enough:\n"
		"steghide embed -cf cvr.bmp -sf stg.bmp -pf pln.txt\n\n"

		"For extracting it is sufficient to use:\n"
		"steghide extract -sf stg.bmp\n\n")) ;
}

static void license ()
{
 	printf (
		"Copyright (C) 2002 Stefan Hetzl <shetzl@teleweb.at>\n\n"

 		"This program is free software; you can redistribute it and/or\n"
 		"modify it under the terms of the GNU General Public License\n"
 		"as published by the Free Software Foundation; either version 2\n"
 		"of the License, or (at your option) any later version.\n\n"

 		"This program is distributed in the hope that it will be useful,\n"
 		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
 		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
 		"GNU General Public License for more details.\n\n"

 		"You should have received a copy of the GNU General Public License\n"
 		"along with this program; if not, write to the Free Software\n"
 		"Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n") ;
}

/* calls functions to embed plain data in cover data and save as stego data */
static void embedfile (string cvrfilename, string stgfilename, string plnfilename)
{
	CvrStgFile *cvrstgfile = NULL ;
	PLNFILE *plnfile = NULL ;
	unsigned long nbytesplain = 0 ;
	unsigned long firstplnpos = 0 ;

	cvrstgfile = CvrStgFile::readfile (cvrfilename) ;

	plnfile = pln_readfile (plnfilename == "" ? NULL : (char *) plnfilename.c_str()) ;

	assemble_plndata (plnfile) ;

	nbytesplain = plnfile->plndata->length ;
	if (args->encryption.getValue()) {
		encrypt_plnfile (plnfile, (char *) args->passphrase.getValue().c_str()) ;
	}

	setsthdrdmtd () ;

	fillsthdr (cvrstgfile->getCapacity(), nbytesplain, plnfile->plndata->length) ;

	embedsthdr (cvrstgfile, sthdr_dmtd, sthdr_dmtdinfo, args->sthdrencryption.getValue(), (char *) args->passphrase.getValue().c_str(), &firstplnpos) ;

	embeddata (cvrstgfile, firstplnpos, plnfile) ;

	cvrstgfile->transform (stgfilename) ;

	cvrstgfile->write() ;

	delete cvrstgfile ;
	pln_cleanup (plnfile) ;

	VerboseMessage v (_("done.")) ;
	v.printMessage() ;
}

/* calls functions to extract (and save) plain data from stego data */
static void extractfile (string stgfilename, string plnfilename)
{
	PLNFILE *plnfile = NULL ;
	unsigned long firstplnpos = 0 ;

	CvrStgFile *stgfile = CvrStgFile::readfile (stgfilename) ;

	setsthdrdmtd () ;

	extractsthdr (stgfile, sthdr_dmtd, sthdr_dmtdinfo, args->sthdrencryption.getValue(), (char *) args->passphrase.getValue().c_str(), &firstplnpos) ;

	plnfile = pln_createfile () ;
	plnfile->plndata = extractdata (stgfile, firstplnpos) ;

	if (sthdr.encryption) {
		decrypt_plnfile (plnfile, (char *) args->passphrase.getValue().c_str()) ;
	}

	deassemble_plndata (plnfile) ;

	pln_writefile (plnfile) ;

	delete stgfile ;
	pln_cleanup (plnfile) ;

	VerboseMessage v (_("done.")) ;
	v.printMessage() ;
}

static void cleanup (void)
{
	delete args ;
}
