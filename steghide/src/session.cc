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

#include <cstdio>
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
			assert (0) ;
		break ; }
	}
}

void Session::printEncInfo ()
{
	vector<string> algos = MCryptpp::getListAlgorithms() ;
	vector<string> modes = MCryptpp::getListModes() ;

	printf (_("encryption algorithms:\n"
		"<algorithm>: <supported modes>...\n")) ;

	for (vector<string>::iterator a = algos.begin() ; a != algos.end() ; a++) {
		cout << *a << ":" ;
		for (vector<string>::iterator m = modes.begin() ; m != modes.end() ; m++) {
			if (MCryptpp::AlgoSupportsMode (*a, *m)) {
				cout << " " << *m ;
			}
		}
		cout << endl ;
	}
}

void Session::printVersion ()
{
	cout << "steghide version " << VERSION << endl ;
}

// TODO - Lang- u. Kurzformen von Argumenten testen
// FIXME - in übersetzungen nachsehen - alle fehlermeldungen mit punkt beenden
void Session::printHelp ()
{
	printVersion() ;
	printf (_("\n"
		"the first argument must be one of the following:\n"
		" embed, --embed          embed data\n"
		" extract, --extract      extract data\n"
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

		"\nIf a <filename> is \"-\", stdin or stdout is used.\n\n"

		"The arguments default to reasonable values.\n\n"

		"For embedding the following line is enough:\n"
		"steghide embed -cf cvr.jpg -sf stg.jpg -ef emb.txt\n\n"

		"For extracting it is sufficient to use:\n"
		"steghide extract -sf stg.jpg\n\n")) ;
}

void Session::printLicense ()
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
