#include <iostream>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "main.h"
#include "msg.h"
#include "support.h"

#ifdef DEBUG
#include "test.h"
#endif

Arguments *args ;

Arguments::Arguments (void)
{
	// empty
}

Arguments::Arguments (int argc, char *argv[])
{
	Arguments () ;
	parse (argc, argv) ;
}

void Arguments::parse (int argc, char *argv[])
{
	int i ;

	/* FIXME statt strncmps hier überall string == verwenden - nicht auf Länge fehleranfällig */

	/* check for first argument -> command */
	if (argc == 1) {
		command.setValue (SHOWHELP) ;
		return ;
	}
	else if ((strncmp (argv[1], "embed\0", 6) == 0) || (strncmp (argv[1], "--embed\0", 8) == 0)) {
		command.setValue (EMBED) ;
		setDefaults () ;
	}
	else if ((strncmp (argv[1], "extract\0", 8) == 0) || (strncmp (argv[1], "--extract\0", 10) == 0)) {
		command.setValue (EXTRACT) ;
		setDefaults () ;
	}
	else if ((strncmp (argv[1], "version\0", 8) == 0) || (strncmp (argv[1], "--version\0", 10) == 0)) {
		command.setValue (SHOWVERSION) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"version\" command")) ;
		}
		return ;
	}
	else if ((strncmp (argv[1], "license\0", 8) == 0) || (strncmp (argv[1], "--license\0", 10) == 0)) {
		command.setValue (SHOWLICENSE) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"license\" command")) ;
		}
		return ;
	}
	else if ((strncmp (argv[1], "help\0", 5) == 0) || (strncmp (argv[1], "--help\0", 7) == 0)) {
		command.setValue (SHOWHELP) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"help\" command")) ;
		}
		return ;
	}
#ifdef DEBUG
	else if (strncmp (argv[1], "test\0", 5) == 0) {
		steghide_test_all () ;
		exit (EXIT_SUCCESS) ;
	}
#endif
	else {
		exit_err (_("unknown command \"%s\". type \"%s --help\" for help."), argv[1], PROGNAME) ;
	}

	/* check rest of arguments */
	for (i = 2; i < argc; i++) {
		if ((strncmp (argv[i], "-d\0", 3) == 0) || (strncmp (argv[i], "--distribution\0", 15) == 0)) {
			unsigned int tmp = 0 ;

			if (command.getValue() != EMBED) {
				exit_err (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (dmtd.is_set()) {
				exit_err (_("the distribution argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the argument \"%s\" is incomplete. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (strncmp (argv[i], "cnsti\0", 6) == 0) {
				dmtd.setValue (DMTD_CNSTI) ;
				
				if ((i + 1 < argc) && (argv[i + 1][0] != '-')) {
					DMTDINFO di = dmtdinfo.getValue() ;

					i++ ;
					if ((tmp = readnum (argv[i])) > DMTD_CNSTI_MAX_ILEN) {
						exit_err (_("the interval length for the cnsti method must be smaller than %d."), DMTD_CNSTI_MAX_ILEN + 1) ;
					}

					di.cnsti.interval_len = tmp ;
					dmtdinfo.setValue (di) ;	
				}
			}
			else if ((strncmp (argv[i], "prndi\0", 6) == 0)) {
				dmtd.setValue (DMTD_PRNDI) ;
				
				if ((i + 1 < argc) && (argv[i + 1][0] != '-')) {
					DMTDINFO di = dmtdinfo.getValue() ;

					i++ ;
					if ((tmp = readnum (argv[i])) > DMTD_PRNDI_MAX_IMLEN) {
						exit_err (_("the maximum interval length for the prndi method must be smaller than %d."), DMTD_PRNDI_MAX_IMLEN + 1) ;
					}

					di.prndi.interval_maxlen = tmp ;
					dmtdinfo.setValue (di) ;
				}
			}
			else {
				exit_err (_("unknown distribution method \"%s\". type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}
		}

		else if ((strncmp (argv[i], "-e\0", 3) == 0) || (strncmp (argv[i], "--encryption\0", 13) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (encryption.is_set()) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			encryption.setValue (true) ;
		}

		else if ((strncmp (argv[i], "-E\0", 3) == 0) || (strncmp (argv[i], "--noencryption\0", 15) == 0)) {
			if (command.getValue () != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (encryption.is_set()) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			encryption.setValue (false) ;
		}

		else if ((strncmp (argv[i], "-h\0", 3) == 0) || (strncmp (argv[i], "--sthdrencryption\0", 18) == 0)) {
			if (sthdrencryption.is_set()) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			sthdrencryption.setValue (true) ;
		}

		else if ((strncmp (argv[i], "-H\0", 3) == 0) || (strncmp (argv[i], "--nosthdrencryption\0", 20) == 0)) {
			if (sthdrencryption.is_set()) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			sthdrencryption.setValue (false) ;
		}

		else if ((strncmp (argv[i], "-k\0", 3) == 0) || (strncmp (argv[i], "--checksum\0", 11) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (checksum.is_set()) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			checksum.setValue (true) ;
		}

		else if ((strncmp (argv[i], "-K\0", 3) == 0) || (strncmp (argv[i], "--nochecksum\0", 13) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (checksum.is_set()) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			checksum.setValue (false) ;
		}

		else if ((strncmp (argv[i], "-n\0", 3) == 0) || (strncmp (argv[i], "--embedplainname\0", 17) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (embedplnfn.is_set()) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			embedplnfn.setValue (true) ;
		}

		else if ((strncmp (argv[i], "-N\0", 3) == 0) || (strncmp (argv[i], "--notembedplainname\0", 20) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (embedplnfn.is_set()) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			embedplnfn.setValue (false) ;
		}

		else if ((strncmp (argv[i], "-c\0", 3) == 0) || (strncmp (argv[i], "--compatibility\0", 16) == 0)) {
			if (compatibility.is_set()) {
				exit_err (_("the compatibility argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			compatibility.setValue (true) ;
		}

		else if ((strncmp (argv[i], "-p\0", 3) == 0) || (strncmp (argv[i], "--passphrase\0", 13) == 0)) {
			if (passphrase.is_set()) {
				exit_err (_("the passphrase argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the passphrase. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (strlen (argv[i]) > PASSPHRASE_MAXLEN) {
				exit_err (_("the maximum length of the passphrase is %d characters."), PASSPHRASE_MAXLEN) ;
			}
			passphrase.setValue (argv[i]) ;

			/* overwrite passphrase in argv in order to avoid that it can be read with the ps command  */
			for (unsigned int j = 0 ; j < strlen (argv[i]) ; j++) {
				argv[i][j] = ' ' ;
			}
		}

		else if ((strncmp (argv[i], "-cf\0", 4) == 0) || (strncmp (argv[i], "--coverfile\0", 16) == 0)) {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (cvrfn.is_set()) {
				exit_err (_("the cover file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the cover file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (strncmp (argv[i], "-\0", 2) == 0) {
				cvrfn.setValue ("") ;
			}
			else {
				cvrfn.setValue (argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-sf\0", 4) == 0) || (strncmp (argv[i], "--stegofile\0", 12) == 0)) {
			if (stgfn.is_set()) {
				exit_err (_("the stego file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the stego file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (strncmp (argv[i], "-\0", 2) == 0) {
				stgfn.setValue ("") ;
			}
			else {
				stgfn.setValue (argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-pf\0", 4) == 0) || (strncmp (argv[i], "--plainfile\0", 12) == 0)) {
			if (plnfn.is_set()) {
				exit_err (_("the plain file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the plain file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (strncmp (argv[i], "-\0", 2) == 0) {
				plnfn.setValue ("") ;
			}
			else {
				plnfn.setValue (argv[i]) ;
			}
		}

		else if ((strncmp (argv[i], "-f\0", 3) == 0) || (strncmp (argv[i], "--force\0", 8) == 0)) {
			if (force.is_set()) {
				exit_err (_("the force argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			force.setValue (true);
		}

		else if ((strncmp (argv[i], "-q\0", 3) == 0) || (strncmp (argv[i], "--quiet\0", 8) == 0)) {
			if (verbosity.is_set()) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			verbosity.setValue (QUIET) ;
		}

		else if ((strncmp (argv[i], "-v\0", 3) == 0) || (strncmp (argv[i], "--verbose\0", 10) == 0)) {
			if (verbosity.is_set()) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			verbosity.setValue (VERBOSE) ;
		}

		else {
			exit_err (_("unknown argument \"%s\". type \"%s --help\" for help."), argv[i], PROGNAME) ;
		}
	}

	/* argument post-processing */
	if (command.getValue() == EMBED) {
		if ((cvrfn.getValue() == "") && (plnfn.getValue() == "")) {
			exit_err (_("standard input cannot be used for cover AND plain data. type \"%s --help\" for help."), PROGNAME) ;
		}
	}

	if (!passphrase.is_set()) {
		/* prompt for passphrase */
		if (command.getValue() == EMBED) {
			if ((cvrfn.getValue() == "") || (plnfn.getValue() == "")) {
				exit_err (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			passphrase.setValue (get_passphrase (true)) ;
		}
		else if (command.getValue() == EXTRACT) {
			if (stgfn.getValue() == "") {
				exit_err (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			passphrase.setValue (get_passphrase (false)) ;
		}
	}

	return ;
}

void Arguments::setDefaults (void)
{
	assert (command.is_set()) ;

	dmtd.setValue (Default_dmtd, false) ;

	unsigned char tmp[4] ;
	DMTDINFO di ;
	tmp[0] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[1] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[2] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	tmp[3] = (unsigned char) (256.0 * rand() / (RAND_MAX + 1.0)) ;
	cp32uc2ul_be (&di.prndi.seed, tmp) ; 
	dmtdinfo.setValue (di, false) ;

	sthdrencryption.setValue (Default_sthdrencryption, false) ;
	encryption.setValue (Default_encryption, false) ;
	checksum.setValue (Default_checksum, false) ;
	embedplnfn.setValue (Default_embedplnfn, false) ;
	compatibility.setValue (Default_compatibility, false) ;
	verbosity.setValue (Default_verbosity, false) ;
	force.setValue (Default_force, false) ;
	cvrfn.setValue ("", false) ;
	plnfn.setValue ("", false) ;
	stgfn.setValue ("", false) ;
	passphrase.setValue ("", false) ;
}
