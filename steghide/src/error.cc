#include <iostream>
#include <stdio.h>
#include <stdarg.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "error.h"
#include "main.h"

//
// class SteghideError
//
SteghideError::SteghideError (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void SteghideError::printMessage (void)
{
	cerr << PROGNAME << ": " << getMessage() << endl ;
}

//
// class BinaryInputError
//
BinaryInputError::BinaryInputError (string fn, FILE* s)
{
	if (feof (s)) {
		if (fn == "") {
			SteghideError (_("premature end of data from standard input.")) ;
			setType (STDIN_EOF) ;
		}
		else {
			SteghideError (compose (_("premature end of file \"%s\"."), fn.c_str())) ;
			setType (FILE_EOF) ;
		}
	}
	else {
		if (fn == "") {
			SteghideError (_("an error occured while reading data from standard input.")) ;
			setType (STDIN_ERR) ;
		}
		else {
			SteghideError (compose (_("an error occured while reading data from the file \"%s\"."), fn.c_str())) ;
			setType (FILE_ERR) ;
		}
	}
}

BinaryInputError::TYPE BinaryInputError::getType (void)
{
	return type ;
}

void BinaryInputError::setType (BinaryInputError::TYPE t)
{
	type = t ;
}

//
// class BinaryOutputError
//
BinaryOutputError::BinaryOutputError (string fn)
{
	if (fn == "") {
		SteghideError (_("an error occured while writing data to standard output.")) ;
		setType (STDOUT_ERR) ;
	}
	else {
		SteghideError (compose (_("an error occured while writing data to the file \"%s\"."), fn.c_str())) ;
		setType (FILE_ERR) ;
	}
}

BinaryOutputError::TYPE BinaryOutputError::getType (void)
{
	return type ;
}

void BinaryOutputError::setType (BinaryOutputError::TYPE t)
{
	type = t ;
}
