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
