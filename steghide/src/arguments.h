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

#ifndef SH_ARGUMENTS_H
#define SH_ARGUMENTS_H

#include <string>

#include "arg.h"

class Arguments {
	public:
	ArgCommand		Command ;
	ArgString		EmbFn ;
	ArgString		CvrFn ;
	ArgString		EncAlgo ;
	ArgString		EncMode ;
	ArgBool			Checksum ;
	ArgBool			EmbedEmbFn ;
	ArgString		ExtFn ;
	ArgString		Passphrase ;
	ArgString		StgFn ;
	ArgBool			Force ;
	ArgVerbosity	Verbosity ;

	Arguments (void) ;
	Arguments (int argc, char *argv[]) ;

	void parse (int argc, char *argv[]) ;

	bool stdin_isused (void) ;

	private:
	// FIXME
#if 0
	static const char		Default_EncAlgo = "rijndael-128" ;
	static const char		Default_EncMode = "cbc" ;
#endif
#define Default_EncAlgo "rijndael-128"
#define Default_EncMode "cbc"	
	static const bool		Default_Checksum = true ;
	static const bool		Default_EmbedEmbFn = true ;
	static const bool		Default_Force = false ;
	static const VERBOSITY	Default_Verbosity = NORMAL ;

	static const unsigned int	PassphraseMaxLen = 255 ;

	string getPassphrase (bool doublecheck = false) ;
	void setDefaults (void) ;
} ;

// gcc does not support the export keyword
#include "arg.cc"

#endif /* ndef SH_ARGUMENTS_H */
