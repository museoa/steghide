#ifndef SH_ERROR_H
#define SH_ERROR_H

#include <string>

#if 0
class MessageBase {
	public:
	MessageBase (void) ;
	MessageBase (string msg) ;
	MessageBase (const char *msgfmt, ...) ;
	MessageBase (const char *msgfmt, va_list ap) ;

	string getMessage (void) ;
	void setMessage (string msg) ;
	void setMessage (const char *msgfmt, ...) ;
	void setMessage (const char *msgfmt, va_list ap) ;
	virtual void printMessage (void) = 0 ;

	protected:
	static const unsigned int MsgMaxSize = 512 ;

	string compose (const char *msgfmt, ...) ;
	string vcompose (const char *msgfmt, va_list ap) ;

	private:
	string message ;
} ;
/* Hierarchie:
MessageBase
	SteghideError
		BinaryInputError
		BinaryOutputError
		...
	Message (print für NORMAL, VERBOSE)
	VerboseMessage (print für VERBOSE)
	Warning (bool critical - print für NORMAL, VERBOSE (if critical auch QUIET))
*/
#endif

class SteghideError {
	public:
	SteghideError (void) ;
	SteghideError (string msg) ;
	SteghideError (const char *msgfmt, ...) ;

	string getMessage (void) ;
	void setMessage (string msg) ;
	void printMessage (void) ;	

	protected:
	static const unsigned int errmsg_maxsize = 512 ;

	string compose (const char *msgfmt, ...) ;
	string vcompose (const char *msgfmt, va_list ap) ;

	private:
	string message ;
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
