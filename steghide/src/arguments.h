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

#ifndef SH_ARGUMENTS_H
#define SH_ARGUMENTS_H

#include <string>

#include "arg.h"
#include "stegano.h"

class Arguments {
	public:
	ArgCommand		command ;
	ArgUInt			dmtd ;
	ArgDMTDINFO		dmtdinfo ;
	ArgBool			sthdrencryption ;
	ArgBool			encryption ;
	ArgBool			checksum ;
	ArgBool			embedplnfn ;
	ArgBool			compatibility ;
	ArgVerbosity	verbosity ;
	ArgBool			force ;
	ArgString		cvrfn ;
	ArgString		plnfn ;
	ArgString		stgfn ;
	ArgString		passphrase ;

	Arguments (void) ;
	Arguments (int argc, char *argv[]) ;

	void parse (int argc, char *argv[]) ;

	private:
	static const unsigned int	Default_dmtd = DMTD_PRNDI ;
	static const bool			Default_sthdrencryption = true ;
	static const bool			Default_encryption = true ;
	static const bool			Default_checksum = true ;
	static const bool			Default_embedplnfn = true ;
	static const bool			Default_compatibility = false ;
	static const Verbosity		Default_verbosity = NORMAL ;
	static const bool			Default_force = false ;

	void setDefaults (void) ;
} ;

extern Arguments *args ;

#endif /* ndef SH_ARGUMENTS_H */

// gcc does not support the export keyword
#include "arg.cc"
