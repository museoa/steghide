#include <stdio.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "binaryio.h"
#include "error.h"

BinaryIO::BinaryIO (void)
{
	setName ("") ;
	setStream (NULL) ;
	set_open (false) ;
}

BinaryIO::BinaryIO (string fn, MODE m)
{
	BinaryIO () ;
	open (fn, m) ;
}

BinaryIO::~BinaryIO (void)
{
	if (is_open()) {
		close () ;
	}
}

FILE *BinaryIO::getStream (void)
{
	return stream ;
}

void BinaryIO::setStream (FILE *s)
{
	stream = s ;
}

string BinaryIO::getName (void)
{
	return filename ;
}

void BinaryIO::setName (string fn)
{
	filename = fn ;
}

bool BinaryIO::is_open (void)
{
	return fileopen ;
}

void BinaryIO::set_open (bool fo)
{
	fileopen = fo ;
}

bool BinaryIO::is_std (void)
{
	return (getName() == "") ;
}

BinaryIO::MODE BinaryIO::getMode (void)
{
	return mode ;
}

void BinaryIO::setMode (BinaryIO::MODE m)
{
	mode = m ;
}
// FIXME nur eine dieser beiden Arten MODE anzugeben !!
void BinaryIO::open (string fn, MODE m)
{
	if (fn == "") {
		switch (m) {
			case READ:
			setStream (stdin) ;
			break ;

			case WRITE:
			setStream (stdout) ;
			break ;

			default:
			assert (0) ;
			break ;
		}
	}
	else {
		FILE *s = NULL ;
		char *cmode ;

		switch (m) {
			case READ:
			cmode = "rb" ;
			break ;

			case WRITE:
			// FIXME - overwrite test hier ?? (--force)
			cmode = "wb" ;
			break ;
		}

		if ((s = fopen (fn.c_str(), cmode)) == NULL) {
			throw SteghideError (_("could not open the file \"%s\"."), fn.c_str()) ;
		}

		setStream (s) ;
	}
	setName (fn) ;
	setMode (m) ;
	set_open (true) ;
}

bool BinaryIO::eof (void)
{
	return feof (getStream()) ;
}

void BinaryIO::close (void)
{
	if (getName() != "") {
		if (fclose (getStream()) == EOF) {
			throw SteghideError (_("could not close the file \"%s\"."), getName().c_str()) ;
		}
	}

	setName ("") ;
	setStream (NULL) ;
	set_open (false) ;
}

unsigned char BinaryIO::read8 (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int c = EOF ;
	if ((c = fgetc (getStream())) == EOF) {
		throw BinaryInputError (getName(), getStream()) ;
	}

	return (unsigned char) c ;
}

unsigned int BinaryIO::read16_le (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[2] ;
	for (int i = 0 ; i < 2 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[1] << 8) | bytes[0]) ;
}

unsigned int BinaryIO::read16_be (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[2] ;
	for (int i = 0 ; i < 2 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[0] << 8) | bytes[1]) ;
}

unsigned long BinaryIO::read32_le (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[4] ;
	for (int i = 0 ; i < 4 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0]) ;
}

unsigned long BinaryIO::read32_be (void)
{
	assert (getMode() == READ) ;
	assert (is_open()) ;

	int bytes[4] ;
	for (int i = 0 ; i < 4 ; i++) {
		if ((bytes[i] = fgetc (getStream())) == EOF) {
			throw BinaryInputError (getName(), getStream()) ;
		}
	}

	return ((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]) ;
}

void BinaryIO::write8 (unsigned char val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	if (fputc ((int) val, getStream()) == EOF) {
		throw BinaryOutputError (getName()) ;
	}
}

void BinaryIO::write16_le (unsigned int val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;
	
	for (int i = 0 ; i <= 1 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write16_be (unsigned int val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 1 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_le (unsigned long val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 0 ; i <= 3 ; i++) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}

void BinaryIO::write32_be (unsigned long val)
{
	assert (getMode() == WRITE) ;
	assert (is_open()) ;

	for (int i = 3 ; i >= 0 ; i--) {
		if (fputc ((val >> (8 * i)) & 0xFF, getStream()) == EOF) {
			throw BinaryOutputError (getName()) ;
		}
	}
}
