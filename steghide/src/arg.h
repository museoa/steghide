#ifndef SH_ARG_H
#define SH_ARG_H

#include <string>

#include "stegano.h"

template<class T> class Arg {
	public:
	Arg (void) ;
	Arg (T v, bool setbyuser = true) ;

	T getValue (void) ;
	void setValue (T v, bool setbyuser = true) ;
	bool is_set (void) ;

	protected:
	T value ;
	bool set ;
} ;

enum Command { EMBED, EXTRACT, SHOWVERSION, SHOWLICENSE, SHOWHELP } ;
enum Verbosity { QUIET, NORMAL, VERBOSE } ;

typedef Arg<Command> ArgCommand ;
typedef Arg<unsigned int> ArgUInt ;
typedef Arg<Verbosity> ArgVerbosity ;
typedef Arg<bool> ArgBool ;
typedef Arg<string> ArgString ;
typedef Arg<DMTDINFO> ArgDMTDINFO ;

#endif /* ndef SH_ARG_H */
