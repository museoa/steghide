#include <iostream>
#include <stdio.h>
#include <stdarg.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "main.h"
#include "error.h"

SteghideError::SteghideError (void)
{
	setMessage (_("no error message defined")) ;
}

SteghideError::SteghideError (string msg)
{
	SteghideError() ;
	setMessage (msg) ;
}

SteghideError::SteghideError (const char *msgfmt, ...)
{
	va_list ap ;
	
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

string SteghideError::getMessage (void)
{
	return message ;
}

void SteghideError::setMessage (string msg)
{
	message = msg ;
}

void SteghideError::printMessage (void)
{
	cerr << PROGNAME << ": " << getMessage() << endl ;
}

string SteghideError::compose (const char *msgfmt, ...)
{
    va_list ap ;
	string retval ;

    va_start (ap, msgfmt) ;
	retval = vcompose (msgfmt, ap) ;
    va_end (ap) ;

    return retval ;
}

string SteghideError::vcompose (const char *msgfmt, va_list ap)
{
	char *str = (char *) malloc (errmsg_maxsize) ;

	vsnprintf (str, errmsg_maxsize, msgfmt, ap) ;

	return string (str) ;
}


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
