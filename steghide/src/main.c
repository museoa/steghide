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

#include "main.h"
#include "io.h"
#include "crypto.h"
#include "hash.h"
#include "stegano.h"
#include "msg.h"
#include "support.h"

#ifdef DEBUG
#include "test.h"
#endif

/* arguments */
ARGS args ;

/* how to embed the stego header */
unsigned int sthdr_dmtd = 0 ;
DMTDINFO sthdr_dmtdinfo ;

static void parsearguments (int argc, char *argv[]) ;
static void args_setdefaults (void) ;
static void setsthdrdmtd (void) ;
static void version (void) ;
static void usage (void) ;
static void license (void) ;
static void embedfile (char *cvrfilename, char *stgfilename, char *plnfilename) ;
static void extractfile (char *stgfilename, char *plnfilename) ;
static void cleanup (void) ;

int main (int argc, char *argv[])
{
#ifndef DEBUG
	/* initialize gettext */
	setlocale (LC_ALL, "") ;
	bindtextdomain (PACKAGE, LOCALEDIR) ;
	textdomain (PACKAGE) ;
#endif

	/* 	the C "rand" generator is used if random numbers need not be reproduceable,
		the random number generator in support.c "rnd" is used if numbers must be reproduceable */
	srand ((unsigned int) time (NULL)) ;

	parsearguments (argc, argv) ;

	switch (args.action.value) {
		case ARGS_ACTION_EMBED:
		embedfile (args.cvrfn.value, args.stgfn.value, args.plnfn.value) ;
		break ;

		case ARGS_ACTION_EXTRACT:
		extractfile (args.stgfn.value, args.plnfn.value) ;
		break ;

		case ARGS_ACTION_VERSION:
		version () ;
		break ;

		case ARGS_ACTION_LICENSE:
		license () ;
		break ;

		case ARGS_ACTION_HELP:
		usage () ;
		break ;

		default:
		assert (0) ;
		break ;
	}

	cleanup () ;

	exit (EXIT_SUCCESS) ;
}

/* parses command line arguments */
static void parsearguments (int argc, char* argv[])
{
	int i ;

	/* check for first argument -> action */
	args.action.is_set = 1 ;
	if (argc == 1) {
		args.action.value = ARGS_ACTION_HELP ;
		return ;
	}
	else if ((strncmp (argv[1], "embed\0", 6) == 0) || (strncmp (argv[1], "--embed\0", 8) == 0)) {
		args.action.value = ARGS_ACTION_EMBED ;
		args_setdefaults () ;
	}
	else if ((strncmp (argv[1], "extract\0", 8) == 0) || (strncmp (argv[1], "--extract\0", 10) == 0)) {
		args.action.value = ARGS_ACTION_EXTRACT ;
		args_setdefaults () ;
	}
	else if ((strncmp (argv[1], "version\0", 8) == 0) || (strncmp (argv[1], "--version\0", 10) == 0)) {
		args.action.value = ARGS_ACTION_VERSION ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"version\" command")) ;
		}
		return ;
	}
	else if ((strncmp (argv[1], "license\0", 8) == 0) || (strncmp (argv[1], "--license\0", 10) == 0)) {
		args.action.value = ARGS_ACTION_LICENSE ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"license\" command")) ;
		}
		return ;
	}
	else if ((strncmp (argv[1], "help\0", 5) == 0) || (strncmp (argv[1], "--help\0", 7) == 0)) {
		args.action.value = ARGS_ACTION_HELP ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"help\" command")) ;
		}
		return ;
	}
#ifdef DEBUG
	else if (strncmp (argv[1], "test\0", 5) == 0) {
		test_all () ;
		exit (EXIT_SUCCESS) ;
	}
#endif
	else {
		exit_err (_("unknown command \"%s\". type \"%s --help\" for help."), argv[1], argv[0]) ;
	}

	/* check rest of arguments */
	for (i = 2; i < argc; i++) {
		if ((strncmp (argv[i], "-d\0", 3) == 0) || (strncmp (argv[i], "--distribution\0", 15) == 0)) {
			unsigned int tmp = 0 ;

			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.dmtd.dmtd_is_set) {
				exit_err (_("the distribution argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.dmtd.dmtd_is_set = 1 ;
			}

			if (++i == argc) {
				exit_err (_("the argument \"%s\" is incomplete. type \"%s --help\" for help."), argv[i - 1], argv[0]) ;
			}

			if (strncmp (argv[i], "cnsti\0", 6) == 0) {
				args.dmtd.dmtd = DMTD_CNSTI ;
				
				if ((i + 1 == argc) || (argv[i + 1][0] == '-')) {
					args.dmtd.maxilen_is_set = 0 ;
				}
				else {
					i++ ;
					if ((tmp = readnum (argv[i])) > DMTD_CNSTI_MAX_ILEN) {
						exit_err (_("the interval length for the cnsti method must be smaller than %d."), DMTD_CNSTI_MAX_ILEN + 1) ;
					}

					args.dmtd.dmtdinfo.cnsti.interval_len = tmp ;

					args.dmtd.maxilen_is_set = 1 ;
				}
			}
			else if ((strncmp (argv[i], "prndi\0", 6) == 0)) {
				args.dmtd.dmtd = DMTD_PRNDI ;
				
				if ((i + 1 == argc) || (argv[i + 1][0] == '-')) {
					args.dmtd.maxilen_is_set = 0 ;
				}
				else {
					i++ ;
					if ((tmp = readnum (argv[i])) > DMTD_PRNDI_MAX_IMLEN) {
						exit_err (_("the maximum interval length for the prndi method must be smaller than %d."), DMTD_PRNDI_MAX_IMLEN + 1) ;
					}

					args.dmtd.dmtdinfo.prndi.interval_maxlen = tmp ;
	
					args.dmtd.maxilen_is_set = 1 ;
				}
			}
			else {
				exit_err (_("unknown distribution method \"%s\". type \"%s --help\" for help."), argv[i], argv[0]) ;
			}
		}

		else if ((strncmp (argv[i], "-e\0", 3) == 0) || (strncmp (argv[i], "--encryption\0", 13) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.encryption.is_set) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.encryption.is_set = 1 ;
			}

			args.encryption.value = 1 ;
		}

		else if ((strncmp (argv[i], "-E\0", 3) == 0) || (strncmp (argv[i], "--noencryption\0", 15) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.encryption.is_set) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.encryption.is_set = 1 ;
			}

			args.encryption.value = 0 ;
		}

		else if ((strncmp (argv[i], "-h\0", 3) == 0) || (strncmp (argv[i], "--sthdrencryption\0", 18) == 0)) {
			if (args.sthdrencryption.is_set) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.sthdrencryption.is_set = 1 ;
			}

			args.sthdrencryption.value = 1 ;
		}

		else if ((strncmp (argv[i], "-H\0", 3) == 0) || (strncmp (argv[i], "--nosthdrencryption\0", 20) == 0)) {
			if (args.sthdrencryption.is_set) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.sthdrencryption.is_set = 1 ;
			}

			args.sthdrencryption.value = 0 ;
		}

		else if ((strncmp (argv[i], "-k\0", 3) == 0) || (strncmp (argv[i], "--checksum\0", 11) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.checksum.is_set) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.checksum.is_set = 1 ;
			}

			args.checksum.value = 1 ;
		}

		else if ((strncmp (argv[i], "-K\0", 3) == 0) || (strncmp (argv[i], "--nochecksum\0", 13) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.checksum.is_set) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.checksum.is_set = 1 ;
			}

			args.checksum.value = 0 ;
		}

		else if ((strncmp (argv[i], "-n\0", 3) == 0) || (strncmp (argv[i], "--embedplainname\0", 17) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.embedplnfn.is_set) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.embedplnfn.is_set = 1 ;
			}

			args.embedplnfn.value = 1 ;
		}

		else if ((strncmp (argv[i], "-N\0", 3) == 0) || (strncmp (argv[i], "--notembedplainname\0", 20) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (args.embedplnfn.is_set) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.embedplnfn.is_set = 1 ;
			}

			args.embedplnfn.value = 0 ;
		}

		else if ((strncmp (argv[i], "-p\0", 3) == 0) || (strncmp (argv[i], "--passphrase\0", 13) == 0)) {
			int j = 0 ;

			if (args.passphrase.is_set) {
				exit_err (_("the passphrase argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.passphrase.is_set = 1 ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the passphrase. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (strlen (argv[i]) > PASSPHRASE_MAXLEN) {
				exit_err (_("the maximum length of the passphrase is %d characters."), PASSPHRASE_MAXLEN) ;
			}
			args.passphrase.value = s_malloc (strlen (argv[i]) + 1) ;
			strcpy (args.passphrase.value, argv[i]) ;

			/* overwrite passphrase in argv in order to avoid that it can be read with the ps command  */
			for (j = 0 ; j < strlen (argv[i]) ; j++) {
				argv[i][j] = '*' ;
			}
		}

		else if ((strncmp (argv[i], "-cf\0", 4) == 0) || (strncmp (argv[i], "--coverfile\0", 16) == 0)) {
			if (args.action.value != ARGS_ACTION_EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], argv[0]) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the cover file name. type \"%s --help\" for help."), argv[i - 1], argv[0]) ;
			}

			if (args.cvrfn.is_set) {
				exit_err (_("the cover file name argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			else {
				args.cvrfn.is_set = 1 ;
			}

			if (strncmp (argv[i], "-\0", 2) == 0) {
				args.cvrfn.value = NULL ;
			}
			else {
				args.cvrfn.value = s_malloc (strlen (argv[i]) + 1) ;
				strcpy (args.cvrfn.value, argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-sf\0", 4) == 0) || (strncmp (argv[i], "--stegofile\0", 12) == 0)) {
			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the stego file name. type \"%s --help\" for help."), argv[i - 1], argv[0]) ;
			}

			if (args.stgfn.is_set) {
				exit_err (_("the stego file name argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			args.stgfn.is_set = 1 ;

			if (strncmp (argv[i], "-\0", 2) == 0) {
				args.stgfn.value = NULL ;
			}
			else {
				args.stgfn.value = s_malloc (strlen (argv[i]) + 1) ;
				strcpy (args.stgfn.value, argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-pf\0", 4) == 0) || (strncmp (argv[i], "--plainfile\0", 12) == 0)) {
			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the plain file name. type \"%s --help\" for help."), argv[i - 1], argv[0]) ;
			}

			if (args.plnfn.is_set) {
				exit_err (_("the plain file name argument can be used only once. type \"%s --help\" for help."), argv[0]) ;
			}
			args.plnfn.is_set = 1 ;

			if (strncmp (argv[i], "-\0", 2) == 0) {
				args.plnfn.value = NULL ;
			}
			else {
				args.plnfn.value = s_malloc (strlen (argv[i]) + 1) ;
				strcpy (args.plnfn.value, argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-f\0", 3) == 0) || (strncmp (argv[i], "--force\0", 8) == 0)) {
			args.force.value = 1 ;
		}

		else if ((strncmp (argv[i], "-q\0", 3) == 0) || (strncmp (argv[i], "--quiet\0", 8) == 0)) {
			if (args.verbosity.is_set) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}
			else {
				args.verbosity.is_set = 1 ;
				args.verbosity.value = ARGS_VERBOSITY_QUIET ;
			}
		}

		else if ((strncmp (argv[i], "-v\0", 3) == 0) || (strncmp (argv[i], "--verbose\0", 10) == 0)) {
			if (args.verbosity.is_set) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}
			else {
				args.verbosity.is_set = 1 ;
				args.verbosity.value = ARGS_VERBOSITY_VERBOSE ;
			}
		}

		else {
			exit_err (_("unknown argument \"%s\". type \"%s --help\" for help."), argv[i], argv[0]) ;
		}
	}

	/* argument post-processing */
	if (args.action.value == ARGS_ACTION_EMBED) {
		if ((args.cvrfn.value == NULL) && (args.plnfn.value == NULL)) {
			exit_err (_("standard input cannot be used for cover AND plain data. type \"%s --help\" for help."), argv[0]) ;
		}
	}

	if (!args.passphrase.is_set) {
		/* prompt for passphrase */
		if (args.action.value == ARGS_ACTION_EMBED) {
			if ((args.cvrfn.value == NULL) || (args.plnfn.value == NULL)) {
				exit_err (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			args.passphrase.value = get_passphrase (PP_DOUBLECHECK) ;
		}
		else if (args.action.value == ARGS_ACTION_EXTRACT) {
			if (args.stgfn.value == NULL) {
				exit_err (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			args.passphrase.value = get_passphrase (PP_NODOUBLECHECK) ;
		}
	}

	return ;
}

static void args_setdefaults (void)
{
	unsigned char tmp[4] ;

	assert (args.action.is_set) ;

	args.dmtd.dmtd_is_set = 0 ;
	args.dmtd.dmtd = DMTD_PRNDI ;

	tmp[0] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[1] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[2] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[3] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	cp32uc2ul_be (&args.dmtd.dmtdinfo.prndi.seed, tmp) ; 
	args.dmtd.maxilen_is_set = 0 ;

	args.sthdrencryption.is_set = 0 ;
	args.sthdrencryption.value = DEFAULT_STHDRENCRYPTION ;

	args.encryption.is_set = 0 ;
	args.encryption.value = DEFAULT_ENCRYPTION ;

	args.checksum.is_set = 0 ;
	args.checksum.value = DEFAULT_CHECKSUM ;

	args.embedplnfn.is_set = 0 ;
	args.embedplnfn.value = DEFAULT_EMBEDPLNFN ;

	args.verbosity.is_set = 0 ;
	args.verbosity.value = DEFAULT_VERBOSITY ;

	args.force.is_set = 0 ;
	args.force.value = DEFAULT_FORCE ;

	args.cvrfn.is_set = 0 ;
	args.cvrfn.value = NULL ;

	args.plnfn.is_set = 0 ;
	args.plnfn.value = NULL ;

	args.stgfn.is_set = 0 ;
	args.stgfn.value = NULL ;

	args.passphrase.is_set = 0 ;
	args.passphrase.value = NULL ;

	return ;
}

static void setsthdrdmtd (void)
{
	/* set embedding of stego header */
	sthdr_dmtd = DMTD_PRNDI ;
	sthdr_dmtdinfo.prndi.seed = getseed (args.passphrase.value) ;
	sthdr_dmtdinfo.prndi.interval_maxlen = 2 * INTERVAL_DEFAULT ;

	return ;
}

static void fillsthdr (unsigned long nbytescvrbuf, unsigned long nbytesplain, unsigned long nbytesenc)
{
	/* fill stego header with values */
	sthdr.nbytesplain = nbytesplain ;

	sthdr.dmtd = args.dmtd.dmtd ;

	switch (sthdr.dmtd) {
		case DMTD_CNSTI:
			if (args.dmtd.maxilen_is_set) {
				sthdr.dmtdinfo.cnsti.interval_len = args.dmtd.dmtdinfo.cnsti.interval_len ;
			}
			else {
				unsigned long ubfirstplnpos = calc_ubfirstplnpos(sthdr_dmtd, sthdr_dmtdinfo, args.sthdrencryption.value, nbytesplain) ;
				setmaxilen (nbytescvrbuf, nbytesenc, ubfirstplnpos) ;
			}
		break ;

		case DMTD_PRNDI:
			sthdr.dmtdinfo.prndi.seed = args.dmtd.dmtdinfo.prndi.seed ;
			if (args.dmtd.maxilen_is_set) {
				sthdr.dmtdinfo.prndi.interval_maxlen = args.dmtd.dmtdinfo.prndi.interval_maxlen ;
			}
			else {
				unsigned long ubfirstplnpos = calc_ubfirstplnpos(sthdr_dmtd, sthdr_dmtdinfo, args.sthdrencryption.value, nbytesplain) ;
				setmaxilen (nbytescvrbuf, nbytesenc, ubfirstplnpos) ;
			}
		break ;

		default:
		assert (0) ;
		break ;
	}

	sthdr.mask = 1 ; /* only for backwards compatibility to 0.4.x versions */

	if (args.encryption.value) {
		sthdr.encryption = ENC_MCRYPT ;
	}
	else {
		sthdr.encryption = ENC_NONE ;
	}

	/* compression is not yet implemented but included
	   to enable 0.4.5 to read not compressed post 0.4.5 files */
	sthdr.compression = COMPR_NONE ;

	if (args.checksum.value) {
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
static void embedfile (char *cvrfilename, char *stgfilename, char *plnfilename)
{
	CVRFILE *cvrfile = NULL, *stgfile = NULL ;
	PLNFILE *plnfile = NULL ;
	unsigned long nbytesplain = 0 ;
	unsigned long firstplnpos = 0 ;

	cvrfile = readcvrfile (cvrfilename) ;

	plnfile = readplnfile (plnfilename) ;

	assemble_plndata (plnfile) ;

	nbytesplain = plnfile->plndata->length ;
	if (args.encryption.value) {
		encrypt_plnfile (plnfile, args.passphrase.value) ;
	}

	setsthdrdmtd () ;

	fillsthdr (cvrfile->cvrdata->length, nbytesplain, plnfile->plndata->length) ;

	embedsthdr (cvrfile->cvrdata, sthdr_dmtd, sthdr_dmtdinfo, args.sthdrencryption.value, args.passphrase.value, &firstplnpos) ;

	embeddata (cvrfile->cvrdata, firstplnpos, plnfile->plndata) ;

	stgfile = createstgfile (cvrfile, stgfilename) ;

	writestgfile (stgfile) ;

	cleanupcvrfile (cvrfile, FSS_NO) ;
	cleanupcvrfile (stgfile, FSS_YES) ;
	cleanupplnfile (plnfile) ;

	pverbose (_("done.")) ;

	return ;
}

/* calls functions to extract (and save) plain data from stego data */
static void extractfile (char *stgfilename, char *plnfilename)
{
	CVRFILE *stgfile = NULL ;
	PLNFILE *plnfile = NULL ;
	unsigned long firstplnpos = 0 ;

	stgfile = readcvrfile (stgfilename) ;

	setsthdrdmtd () ;

	extractsthdr (stgfile->cvrdata, sthdr_dmtd, sthdr_dmtdinfo, args.sthdrencryption.value, args.passphrase.value, &firstplnpos) ;

	plnfile = createplnfile () ;
	plnfile->plndata = extractdata (stgfile->cvrdata, firstplnpos) ;

	if (sthdr.encryption) {
		decrypt_plnfile (plnfile, args.passphrase.value) ;
	}

	deassemble_plndata (plnfile) ;

	writeplnfile (plnfile) ;

	cleanupcvrfile (stgfile, FSS_YES) ;
	cleanupplnfile (plnfile) ;

	pverbose (_("done.")) ;

	return ;
}

static void cleanup (void)
{
	if (args.plnfn.value != NULL) {
		free (args.plnfn.value) ;
	}
	if (args.passphrase.value != NULL) {
		free (args.passphrase.value) ;
	}
}
