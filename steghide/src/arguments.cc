/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>
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

#include <iostream>

#include "arguments.h"
#include "common.h"
#include "error.h"
#include "mcryptpp.h"
#include "msg.h"
#include "terminal.h"

#ifdef DEBUG
#include "test.h"
#endif

// the global Arguments object
Arguments Args ;

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
	// if there are no arguments -> show help
	if (argc == 1) {
		Command.setValue (SHOWHELP) ;
		return ;
	}

	int i = 1 ;	// the i-th argument is evaluated

#ifdef DEBUG
	bool debugmode = false ;
	if (string (argv[i]) == "debug") {
		debugmode = true ;
		i++ ;
	}
#endif

	if (string (argv[i]) == "embed" || string (argv[i]) == "--embed") {
		Command.setValue (EMBED) ;
		setDefaults () ;
		i++ ;
	}
	else if (string (argv[i]) == "extract" || string (argv[i]) == "--extract") {
		Command.setValue (EXTRACT) ;
		setDefaults () ;
		i++ ;
	}
	else if (string (argv[i]) == "encinfo" || string (argv[i]) == "--encinfo") {
		Command.setValue (ENCINFO) ;
		if (argc > 2) {
			Warning w (_("you cannot use arguments with the \"encinfo\" command.")) ;
			w.printMessage() ;
		}
		return ;
	}
	else if (string (argv[i]) == "version" || string (argv[i]) == "--version") {
		Command.setValue (SHOWVERSION) ;
		if (argc > 2) {
			Warning w (_("you cannot use arguments with the \"version\" command.")) ;
			w.printMessage() ;
		}
		return ;
	}
	else if (string (argv[i]) == "license" || string (argv[i]) == "--license") {
		Command.setValue (SHOWLICENSE) ;
		if (argc > 2) {
			Warning w (_("you cannot use arguments with the \"license\" command.")) ;
			w.printMessage() ;
		}
		return ;
	}
	else if (string (argv[i]) == "help" || string (argv[i]) == "--help") {
		Command.setValue (SHOWHELP) ;
		if (argc > 2) {
			Warning w (_("you cannot use arguments with the \"help\" command.")) ;
			w.printMessage() ;
		}
		return ;
	}
#ifdef DEBUG
	else if (debugmode && (string (argv[i]) == "test")) {
		steghide_test_all () ;
		exit (EXIT_SUCCESS) ;
	}
#endif
	else {
		throw SteghideError (_("unknown command \"%s\". type \"%s --help\" for help."), argv[1], PROGNAME) ;
	}

	// parse rest of arguments
	for ( ; i < argc; i++) {
		if (string (argv[i]) == "-e" || string (argv[i]) == "--encryption") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("the argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (EncAlgo.is_set() || EncMode.is_set()) {
				throw SteghideError (_("the encryption argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by encryption information. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			string s1, s2 ;
			if (argv[i][0] == '-') {
				throw SteghideError (_("the \"%s\" argument must be followed by encryption information. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}
			else {
				s1 = string (argv[i]) ;
				if (i + 1 == argc) {
					s2 = "" ;
				}
				else {
					if (argv[i + 1][0] != '-') {
						s2 = string (argv[i + 1]) ;
						i++ ;
					}
				}
			}

			if (s1 == "none") {
				EncAlgo.setValue (s1) ;
			}
			else {
				bool s1_isalgo = false, s1_ismode = false ;
				bool s2_isalgo = false, s2_ismode = false ;

				if (s1 != "") {
					s1_isalgo = MCryptpp::isValidAlgorithm (s1) ;
					s1_ismode = MCryptpp::isValidMode (s1) ;

					assert (!(s1_isalgo && s1_ismode)) ;
					if (!(s1_isalgo || s1_ismode)) {
						throw SteghideError (_("\"%s\" is neither an algorithm nor a mode supported by libmcrypt."), s1.c_str()) ;
					}
				}
				if (s2 != "") {
					s2_isalgo = MCryptpp::isValidAlgorithm (s2) ;
					s2_ismode = MCryptpp::isValidMode (s2) ;

					assert (!(s2_isalgo && s2_ismode)) ;
					if (!(s2_isalgo || s2_ismode)) {
						throw SteghideError (_("\"%s\" is neither an algorithm nor a mode supported by libmcrypt."), s2.c_str()) ;
					}
				}

				if (s1_isalgo && s2_isalgo) {
					throw SteghideError (_("\"%s\" and \"%s\" are both libmcrypt algorithms. please specify only one."), s1.c_str(), s2.c_str()) ;
				}
				if (s1_ismode && s2_ismode) {
					throw SteghideError (_("\"%s\" and \"%s\" are both libmcrypt modes. please specify only one."), s1.c_str(), s2.c_str()) ;
				}

				if (s1_isalgo) {
					EncAlgo.setValue (s1) ;
				}
				if (s1_ismode) {
					EncMode.setValue (s1) ;
				}
				if (s2_isalgo) {
					EncAlgo.setValue (s2) ;
				}
				if (s2_ismode) {
					EncMode.setValue (s2) ;
				}

				if (!MCryptpp::AlgoSupportsMode (EncAlgo.getValue(), EncMode.getValue())) {
					throw SteghideError (_("the encryption algorithm \"%s\" can not be used with the mode \"%s\"."), EncAlgo.getValue().c_str(), EncAlgo.getValue().c_str()) ;
				}
			}
		}

		else if (string (argv[i]) == "-k" || string (argv[i]) == "--checksum") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (Checksum.is_set()) {
				throw SteghideError (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			Checksum.setValue (true) ;
		}

		else if (string (argv[i]) == "-K" || string (argv[i]) == "--nochecksum") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (Checksum.is_set()) {
				throw SteghideError (_("the checksum argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			Checksum.setValue (false) ;
		}

		else if (string (argv[i]) == "-n" || string (argv[i]) == "--embedname") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (EmbedEmbFn.is_set()) {
				throw SteghideError (_("the file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			EmbedEmbFn.setValue (true) ;
		}

		else if (string (argv[i]) == "-N" || string (argv[i]) == "--dontembedname") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (EmbedEmbFn.is_set()) {
				throw SteghideError (_("the file name embedding argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			EmbedEmbFn.setValue (false) ;
		}

		else if (string (argv[i]) == "-p" || string (argv[i]) == "--passphrase") {
			if (Passphrase.is_set()) {
				throw SteghideError (_("the passphrase argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by the passphrase. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (strlen (argv[i]) > PassphraseMaxLen) {
				throw SteghideError (_("the maximum length of the passphrase is %d characters."), PassphraseMaxLen) ;
			}
			Passphrase.setValue (argv[i]) ;

			// overwrite passphrase in argv in order to avoid that it can be read with the ps command
			for (unsigned int j = 0 ; j < strlen (argv[i]) ; j++) {
				argv[i][j] = ' ' ;
			}
		}

		else if (string (argv[i]) == "-cf" || string (argv[i]) == "--coverfile") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (CvrFn.is_set()) {
				throw SteghideError (_("the cover file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by the cover file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				CvrFn.setValue ("") ;
			}
			else {
				CvrFn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-sf" || string (argv[i]) == "--stegofile") {
			if (StgFn.is_set()) {
				throw SteghideError (_("the stego file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by the stego file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				StgFn.setValue ("") ;
			}
			else {
				StgFn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-ef" || string (argv[i]) == "--embedfile") {
			if (Command.getValue() != EMBED) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (EmbFn.is_set()) {
				throw SteghideError (_("the embed file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by the embed file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				EmbFn.setValue ("") ;
			}
			else {
				EmbFn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-xf" || string (argv[i]) == "--extractfile") {
			if (Command.getValue() != EXTRACT) {
				throw SteghideError (_("argument \"%s\" can only be used with the \"extract\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
			}

			if (ExtFn.is_set()) {
				throw SteghideError (_("the plain file name argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			if (++i == argc) {
				throw SteghideError (_("the \"%s\" argument must be followed by the plain file name. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
			}

			if (string (argv[i]) == "-") {
				ExtFn.setValue ("") ;
			}
			else {
				ExtFn.setValue (argv[i]) ;
			}
		}

		else if (string (argv[i]) == "-f" || string (argv[i]) == "--force") {
			if (Force.is_set()) {
				throw SteghideError (_("the force argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
			}

			Force.setValue (true);
		}

		else if (string (argv[i]) == "-q" || string (argv[i]) == "--quiet") {
			if (Verbosity.is_set()) {
				throw SteghideError (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			Verbosity.setValue (QUIET) ;
		}

		else if (string (argv[i]) == "-v" || string (argv[i]) == "--verbose") {
			if (Verbosity.is_set()) {
				throw SteghideError (_("the \"%s\" argument cannot be used here because the verbosity has already been set."), argv[i]) ;
			}

			Verbosity.setValue (VERBOSE) ;
		}
		
#ifdef DEBUG
		else if (debugmode) {
			if (string (argv[i]) == "--printgraph") {
				if (DebugCommand.is_set()) {
					throw SteghideError (_("you cannot use more than one debug command at a time.")) ;
				}

				DebugCommand.setValue (PRINTGRAPH) ;
			}

			else if (string (argv[i]) == "-r" || string (argv[i]) == "--radius") {
				if (Command.getValue() != EMBED) {
					throw SteghideError (_("argument \"%s\" can only be used with the \"embed\" command. type \"%s --help\" for help."), argv[i], PROGNAME) ;
				}

				if (Radius.is_set()) {
					throw SteghideError (_("the radius argument can be used only once. type \"%s --help\" for help."), PROGNAME) ;
				}

				if (++i == argc) {
					throw SteghideError (_("the \"%s\" argument must be followed by the neighbourhood radius. type \"%s --help\" for help."), argv[i - 1], PROGNAME) ;
				}

				float tmp = 0.0 ;
				sscanf (argv[i], "%f", &tmp) ;
				Radius.setValue (tmp) ;
			}

			/*
			else if (string (argv[i]) == "--printstats")  {
				// TODO
			}
			*/
		}
#endif
		
		else {
			throw SteghideError (_("unknown argument \"%s\". type \"%s --help\" for help."), argv[i], PROGNAME) ;
		}
	}

	// argument post-processing 
	if (Command.getValue() == EMBED) {
		if ((CvrFn.getValue() == "") && (EmbFn.getValue() == "")) {
			throw SteghideError (_("standard input cannot be used for cover data AND data to be embedded. type \"%s --help\" for help."), PROGNAME) ;
		}
		if (!StgFn.is_set() && CvrFn.is_set()) {
			StgFn.setValue (CvrFn.getValue()) ;
			Force.setValue (true) ;
		}
	}

	if (!Passphrase.is_set()) {
		// prompt for passphrase
		if (Command.getValue() == EMBED) {
			if ((CvrFn.getValue() == "") || (EmbFn.getValue() == "")) {
				throw SteghideError (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			Passphrase.setValue (getPassphrase (true)) ;
		}
		else if (Command.getValue() == EXTRACT) {
			if (StgFn.getValue() == "") {
				throw SteghideError (_("if standard input is used, the passphrase must be specified on the command line.")) ;
			}
			Passphrase.setValue (getPassphrase()) ;
		}
	}
}

string Arguments::getPassphrase (bool doublecheck)
{
    int c = EOF ;

	cerr << _("Enter passphrase: ") ;
	Terminal term ;
	term.EchoOff() ;
	
    string s1 = "" ;
    while ((c = cin.get()) != '\n') {
        s1 += c ;
    }

	term.reset() ;
	cerr << endl ;

	if (doublecheck) {
		cerr << _("Re-Enter passphrase: ") ;
		term.EchoOff() ;

		string s2 = "" ;
		while ((c = cin.get()) != '\n') {
			s2 += c ;
		}

		term.reset() ;
		cerr << endl ;

		if (s1 != s2) {
			throw SteghideError (_("the passphrases do not match.")) ;
		}
	}

	return s1 ;
}

bool Arguments::stdin_isused ()
{
	bool retval = false ;
	if (Command.getValue() == EMBED &&
		(EmbFn.getValue() == "" ||
		 CvrFn.getValue() == "")) {
		retval = true ;
	}
	if (Command.getValue() == EXTRACT &&
		StgFn.getValue() == "") {
		retval = true ;
	}
	return retval ;
}

void Arguments::setDefaults (void)
{
	assert (Command.is_set()) ;

	EmbFn.setValue ("", false) ;
	CvrFn.setValue ("", false) ;
	EncAlgo.setValue (Default_EncAlgo, false) ;
	EncMode.setValue (Default_EncMode, false) ;
	Checksum.setValue (Default_Checksum, false) ;
	EmbedEmbFn.setValue (Default_EmbedEmbFn, false) ;
	ExtFn.setValue ("", false) ;
	Passphrase.setValue ("", false) ;
	StgFn.setValue ("", false) ;
	Force.setValue (Default_Force, false) ;
	Verbosity.setValue (Default_Verbosity, false) ;
#ifdef DEBUG
	DebugCommand.setValue (Default_DebugCommand, false) ;
#endif
}

const char* Arguments::Default_EncAlgo = "rijndael-128" ;
const char* Arguments::Default_EncMode = "cbc" ;
