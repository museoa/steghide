#ifndef SH_BINFILE_H
#define SH_BINFILE_H

#include <stdio.h>
#include <string>

class BinaryIO {
	public:
	enum MODE { READ, WRITE } ;

	BinaryIO (void) ;
	BinaryIO (string fn, MODE m) ;
	~BinaryIO (void) ;

	string getName (void) ;
	bool is_open (void) ;
	bool is_std (void) ;
	bool eof (void) ;

	void open (string fn, MODE m) ;	
	void close (void) ;

	unsigned char read8 (void) ;
	unsigned int read16_le (void) ;
	unsigned int read16_be (void) ;
	unsigned long read32_le (void) ;
	unsigned long read32_be (void) ;

	void write8 (unsigned char val) ;
	void write16_le (unsigned int val) ;
	void write16_be (unsigned int val) ;
	void write32_le (unsigned long val) ;
	void write32_be (unsigned long val) ;

// FIXME	
#ifndef DEBUG	
	protected:
#endif
	FILE *getStream (void) ;
	void setStream (FILE *s) ;
	MODE getMode (void) ;
	void setMode (MODE m) ;
	void setName (string fn) ;
	void set_open (bool fo) ;

	private:
	string filename ;
	FILE *stream ;
	bool fileopen ;
	MODE mode ;
} ;

#endif
