#ifndef SH_ERROR_H
#define SH_ERROR_H

#include <cstdio>
#include <string>

#include "msg.h"

class SteghideError : MessageBase {
	public:
	SteghideError (void) : MessageBase() {} ;
	SteghideError (string msg) : MessageBase (msg) {} ;
	SteghideError (const char *msgfmt, ...) ;

	void printMessage (void) ;	
} ;

class BinaryInputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, FILE_EOF, STDIN_ERR, STDIN_EOF } ;

	BinaryInputError (string fn, FILE* s) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

class BinaryOutputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, STDOUT_ERR } ;

	BinaryOutputError (string fn) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

#endif
