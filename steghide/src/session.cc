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

#include <cstdio>
#include <iostream>
#include <vector>

#include "common.h"
#include "embedder.h"
#include "extractor.h"
#include "mcryptpp.h"
#include "session.h"

void Session::run ()
{
	switch (Args.Command.getValue()) {
		case EMBED: {
			Embedder emb ;
			emb.embed() ;
		break ; }

		case EXTRACT: {
			Extractor ext ;
			ext.extract() ;
		break ; }

		case CAPACITY: {
			printCapacity() ;
		break ; }

		case ENCINFO: {
			printEncInfo() ;
		break ; }

		case SHOWVERSION: {
			printVersion() ;
		break ; }

		case SHOWLICENSE: {
			printLicense() ;
		break ; }

		case SHOWHELP: {
			printHelp() ;
		break ; }

		default: {
			myassert (0) ;
		break ; }
	}
}

void Session::printCapacity ()
{
	CvrStgFile* TheCvrStgFile = CvrStgFile::readFile (Args.CvrFn.getValue()) ;
	float capacity = (float) TheCvrStgFile->getCapacity() ;

	std::string unit = "Byte" ;
	if (capacity > 1024.0) {
		capacity /= 1024.0 ;
		unit = "KB" ;
	}
	if (capacity > 1024.0) {
		capacity /= 1024.0 ;
		unit = "MB" ;
	}
	if (capacity > 1024.0) {
		capacity /= 1024.0 ;
		unit = "GB" ;
	}

	printf (_("the capacity of \"%s\" is approximately %.1f %s.\n"), stripDir(TheCvrStgFile->getName()).c_str(), capacity, unit.c_str()) ;
}

std::string Session::stripDir (std::string s) const
{
	unsigned int start = 0 ;
	if ((start = s.find_last_of ("/\\")) == std::string::npos) {
		start = 0 ;
	}
	else {
		start += 1 ;
	}
	return s.substr (start, std::string::npos) ;
}

void Session::printEncInfo ()
{
#ifdef USE_LIBMCRYPT
	std::vector<std::string> algos = MCryptpp::getListAlgorithms() ;
	std::vector<std::string> modes = MCryptpp::getListModes() ;

	printf (_("encryption algorithms:\n"
		"<algorithm>: <supported modes>...\n")) ;

	for (std::vector<std::string>::iterator a = algos.begin() ; a != algos.end() ; a++) {
		if (EncryptionAlgorithm::isValidStringRep (*a)) { // invalid if supported by libmcrypt but not by steghide, e.g. blowfish-compat
			std::cout << *a << ":" ;
			for (std::vector<std::string>::iterator m = modes.begin() ; m != modes.end() ; m++) {
				if (EncryptionMode::isValidStringRep (*m)) {
					if (MCryptpp::AlgoSupportsMode (*a, *m)) {
						std::cout << " " << *m ;
					}
				}
			}
			std::cout << std::endl ;
		}
	}
#else
	printf (_("steghide has been compiled without support for encryption.\n"
		"If you want to encrypt your data before embedding it, use an external encryption\n"
		"program or install libmcrypt (http://mcrypt.hellug.gr/) and recompile steghide.\n")) ;
#endif
}

void Session::printVersion ()
{
	std::cout << "steghide version " << VERSION << std::endl ;
}

// TODO - test short und long arguments
// FIXME - every error message should end with a "." - see .pot file
void Session::printHelp ()
{
	printVersion() ;
	printf (_("\n"
		"the first argument must be one of the following:\n"
		" embed, --embed          embed data\n"
		" extract, --extract      extract data\n"
		" capacity, --capacity    calculate capacity of a cover file\n"
		" encinfo, --encinfo      display a list of supported encryption algorithms\n"
		" version, --version      display version information\n"
		" license, --license      display steghide's license\n"
		" help, --help            display this usage information\n"

		"\noptions for embedding only:\n"

		" -ef, --embedfile        select file to be embedded\n"
		"   -ef <filename>        embed the file <filename>\n"
		" -cf, --coverfile        select cover file\n"
		"   -cf <filename>        embed into the file <filename>\n"
		" -e, --encryption        select encryption parameters\n"
		"   -e <a>[<m>]|<m>[<a>]  specify an encryption algorithm and/or mode\n"
		"   -e none               do not encrypt data before embedding\n"
		" -r, --radius            specify the neighbourhood-radius\n"
		"   -r <r>                use the real number <r> as radius\n"
		" -k, --checksum          embed crc32 checksum of embedded data (default)\n"
		" -K, --nochecksum        do not embed crc32 checksum of embedded data\n"
		" -n, --embedname         embed the name of the original file (default)\n"
		" -N, --dontembedname     do not embed the name of the original file\n"

		"\noptions for extracting only:\n"
		" -xf, --extractfile      select file for extracted data\n"
		"   -xf <filename>        write the extracted data to <filename>\n"

		"\noptions for embedding and extracting:\n"

		" -p, --passphrase        specify passphrase\n"
		"   -p <passphrase>       use <passphrase> as passphrase\n"
		" -sf, --stegofile        select stego file\n"
		"   -sf <filename>        write result to <filename> (when embedding)\n"
		"   -sf <filename>        extract data from <filename> (when extracting)\n"
		" -f, --force             overwrite existing files\n"
		" -q, --quiet             suppress information messages\n"
		" -v, --verbose           display detailed information\n"

		"\nTo embed emb.txt in cvr.jpg: steghide embed -cf cvr.jpg -ef emb.txt\n"
		"To extract embedded data from stg.jpg: steghide extract -sf stg.jpg\n"
		"To calculate the capacity of cvr.jpg: steghide capacity -cf cvr.jpg\n")) ;
}

void Session::printLicense ()
{
 	printf (
		"Copyright (C) 2002 Stefan Hetzl <shetzl@chello.at>\n\n"

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
