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

#ifndef SH_ARGUMENTS_H
#define SH_ARGUMENTS_H

#include <string>
#include <vector>

#include "arg.h"
class EncryptionAlgorithm ;
class EncryptionMode ;

/**
 * \class Arguments
 * \brief parsing and data representation of command-line arguments
 **/
class Arguments {
	public:
	Arguments (void) {} ;

	/**
	 * initialize this Arguments object with argc and argv
	 **/
	Arguments (int argc, char *argv[]) ;

	/**
	 * parse Argc and Argv filling the Arg* member variable for later access
	 **/
	void parse (void) ;

	/**
	 * is standard input used ? - according to the given arguments
	 **/
	bool stdin_isused (void) const ;

	ArgCommand		Command ;
	ArgString		EmbFn ;
	ArgString		ExtFn ;
	ArgString		CvrFn ;
	ArgString		StgFn ;
	ArgString		Passphrase ;
	ArgBool			Checksum ;
	ArgBool			EmbedEmbFn ;
	ArgEncAlgo		EncAlgo ;
	ArgEncMode		EncMode ;
	ArgFloat		Radius ;
	ArgBool			Force ;
	ArgVerbosity	Verbosity ;
#ifdef DEBUG
	ArgDebugCommand	DebugCommand ;
	ArgUInt			DebugLevel ;
	ArgUInt			PriorityQueueRange ;
	ArgUInt			NConstrHeur ;
#endif

	private:
	typedef std::vector<std::string>::const_iterator ArgIt ;

	static const EncryptionAlgorithm Default_EncAlgo ;
	static const EncryptionMode Default_EncMode ;
	static const bool		Default_Checksum = true ;
	static const bool		Default_EmbedEmbFn = true ;
	static const bool		Default_Force = false ;
	static const VERBOSITY	Default_Verbosity = NORMAL ;
	static const float		Default_Radius = 0.0 ; // there is no default radius for all file formats
#ifdef DEBUG
	static const DEBUGCOMMAND	Default_DebugCommand = NONE ;
	static const unsigned int	Default_DebugLevel = 0 ;
	static const unsigned int	Default_PriorityQueueRange = 0 ; // is never used
	static const unsigned int	Default_NConstrHeur = 0 ; // is never used
#endif

	/**
	 * parse the command
	 *
	 * Note: parse_Command is the only parse_* function that requires curarg to be a command.
	 * (because the command is the only argument with a fixed position).
	 **/
	void parse_Command (ArgIt& curarg) ;

	/**
	 * test if curarg points to an emb filename argument and if yes: parse it
	 * \return true iff one or more arguments have been parsed
	 **/
	bool parse_EmbFn (ArgIt& curarg) ;

	bool parse_ExtFn (ArgIt& curarg) ;
	bool parse_CvrFn (ArgIt& curarg) ;
	bool parse_StgFn (ArgIt& curarg) ;
	bool parse_Passphrase (ArgIt& curarg) ;
	bool parse_Checksum (ArgIt& curarg) ;
	bool parse_EmbedEmbFn (ArgIt& curarg) ;
	bool parse_Encryption (ArgIt& curarg) ;
	bool parse_Radius (ArgIt& curarg) ;
	bool parse_Force (ArgIt& curarg) ;
	bool parse_Verbosity (ArgIt& curarg) ;
#ifdef DEBUG
	bool parse_Debug (ArgIt& curarg) ;
#endif

	std::string getPassphrase (bool doublecheck = false) ;
	void setDefaults (void) ;

	std::vector<std::string> TheArguments ;
	/// true iff the first argument was "debug"
	bool DebugMode ;
} ;

// gcc does not support the export keyword
#include "arg.cc"

#endif /* ndef SH_ARGUMENTS_H */
