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
	// check for first argument -> command
	if (argc == 1) {
		command.setValue (SHOWHELP) ;
		return ;
	}
	else if (string (argv[1]) == "embed" || string (argv[1]) == "--embed") {
		command.setValue (EMBED) ;
		setDefaults () ;
	}
	else if (string (argv[1]) == "extract" || string (argv[1]) == "--extract") {
		command.setValue (EXTRACT) ;
		setDefaults () ;
	}
	else if (string (argv[1]) == "version" || string (argv[1]) == "--version") {
		command.setValue (SHOWVERSION) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"version\" command")) ;
		}
		return ;
	}
	else if (string (argv[1]) == "license" || string (argv[1]) == "--license") {
		command.setValue (SHOWLICENSE) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"license\" command")) ;
		}
		return ;
	}
	else if (string (argv[1]) == "help" || string (argv[1]) == "--help") {
		command.setValue (SHOWHELP) ;
		if (argc > 2) {
			pwarn (_("you cannot use arguments with the \"help\" command")) ;
		}
		return ;
	}
#ifdef DEBUG
	else if (string (argv[1]) == "test") {
		steghide_test_all () ;
		exit (EXIT_SUCCESS) ;
	}
#endif
	else {
		exit_err (_("unknown command \"%s\". type \"%s --help\" for help."), argv[1], PROGNAME) ;
	}

	// parse rest of arguments
	for (int i = 2; i < argc; i++) {
		if (string (argv[i]) == "-d" || string (argv[i]) == "--distribution") {
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

			if (string (argv[i]) == "cnsti") {
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
			else if (string (argv[i]) == "prndi") {
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

		else if (string (argv[i]) == "-e" || string (argv[i]) == "--encryption") {
			if (command.getValue() != EMBED) {
				exit_err (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (encryption.is_set()) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			encryption.setValue (true) ;
		}

		else if (string (argv[i]) == "-E" || string (argv[i]) == "--noencryption") {
			if (command.getValue () != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (encryption.is_set()) {
				exit_err (_("the encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			encryption.setValue (false) ;
		}

		else if (string (argv[i]) == "-h" || string (argv[i]) == "--sthdrencryption") {
			if (sthdrencryption.is_set()) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			sthdrencryption.setValue (true) ;
		}

		else if (string (argv[i]) == "-H" || string (argv[i]) == "--nosthdrencryption") {
			if (sthdrencryption.is_set()) {
				exit_err (_("the stego header encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			sthdrencryption.setValue (false) ;
		}

		else if (string (argv[i]) == "-k" || string (argv[i]) == "--checksum") {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (checksum.is_set()) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			checksum.setValue (true) ;
		}

		else if (string (argv[i]) == "-K" || string (argv[i]) == "--nochecksum") {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (checksum.is_set()) {
				exit_err (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			checksum.setValue (false) ;
		}

		else if (string (argv[i]) == "-n" || string (argv[i]) == "--embedplainname") {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (embedplnfn.is_set()) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			embedplnfn.setValue (true) ;
		}

		else if (string (argv[i]) == "-N" || string (argv[i]) == "--notembedplainname") {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (embedplnfn.is_set()) {
				exit_err (_("the plain file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			embedplnfn.setValue (false) ;
		}

		else if (string (argv[i]) == "-c" || string (argv[i]) == "--compatibility") {
			if (compatibility.is_set()) {
				exit_err (_("the compatibility argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			compatibility.setValue (true) ;
		}

		else if (string (argv[i]) == "-p" || string (argv[i]) == "--passphrase") {
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

			// overwrite passphrase in argv in order to avoid that it can be read with the ps command
			for (unsigned int j = 0 ; j < strlen (argv[i]) ; j++) {
				argv[i][j] = ' ' ;
			}
		}

		else if (string (argv[i]) == "-cf" || string (argv[i]) == "--coverfile") {
			if (command.getValue() != EMBED) {
				exit_err (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (cvrfn.is_set()) {
				exit_err (_("the cover file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the cover file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				cvrfn.setValue ("") ;
			}
			else {
				cvrfn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-sf" || string (argv[i]) == "--stegofile") {
			if (stgfn.is_set()) {
				exit_err (_("the stego file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the stego file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				stgfn.setValue ("") ;
			}
			else {
				stgfn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-pf" || string (argv[i]) == "--plainfile") {
			if (plnfn.is_set()) {
				exit_err (_("the plain file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				exit_err (_("the \"%s\" argument must be followed by the plain file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				plnfn.setValue ("") ;
			}
			else {
				plnfn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-f" || string (argv[i]) == "--force") {
			if (force.is_set()) {
				exit_err (_("the force argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			force.setValue (true);
		}

		else if (string (argv[i]) == "-q" || string (argv[i]) == "--quiet") {
			if (verbosity.is_set()) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			verbosity.setValue (QUIET) ;
		}

		else if (string (argv[i]) == "-v" || string (argv[i]) == "--verbose") {
			if (verbosity.is_set()) {
				exit_err (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			verbosity.setValue (VERBOSE) ;
		}

		else {
			exit_err (_("unknown argument \"%s\". type \"%s --help\" for help."), argv[i], PROGNAME) ;
		}
	}

	// argument post-processing 
	if (command.getValue() == EMBED) {
		if ((cvrfn.getValue() == "") && (plnfn.getValue() == "")) {
			exit_err (_("standard input cannot be used for cover AND plain data. type \"%s --help\" for help."), PROGNAME) ;
		}
	}

	if (!passphrase.is_set()) {
		// prompt for passphrase
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
