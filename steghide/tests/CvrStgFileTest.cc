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

#include <cstdio>
#include <cstdlib>

#include "CvrStgFileTest.h"
#include "SampleValue.h"
#include "utcommon.h"

// TODO: new test - readwrite with the same file name

// TODO: ReadCompare tests (compare on the getSampleBit() - level) against reference

// TODO: for a bug (already fixed): some ReadExtractCompare tests for every type of file
// ...a stego file in the tests/data is read, the embedded data is extracted and compared against reference

// TODO: EmbedWriteCompare - inverted ReadExtractCompare tests - data is embedded, stego file written and compared against reference stego file

bool CvrStgFileTest::genericTestReadWrite (const std::string& rfn) const
{
	std::string wfn ("test_rw.out") ;

	CvrStgFile *file = CvrStgFile::readFile (rfn) ;
	file->transform (wfn) ;
	file->write() ;
	delete file ;

	bool retval = areEqual (rfn, wfn) ;
	removeFile (wfn) ;

	return retval ;
}

bool CvrStgFileTest::genericTestReadEmbedExtract (const std::string& fn, const BitString& emb) const
{
	CvrStgFile *file = CvrStgFile::readFile (fn) ;

	// embed data (at the beginning of the file)
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		SampleValue *oldsample = file->getSampleValue (i) ;
		if (oldsample->getBit() != emb[i]) {
			SampleValue *newsample = oldsample->getNearestOppositeSampleValue() ;
			file->replaceSample (i, newsample) ;
			delete newsample ;
		}
		delete oldsample ;
	}

	// extract and compare
	bool retval = true ;
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		if (file->getSampleBit(i) != emb[i]) {
			retval = false ;
		}
	}

	delete file ;
	return retval ;
}

bool CvrStgFileTest::genericTestReadEmbedWriteReadExtract (const std::string& cvrfn, const BitString& emb) const
{
	std::string stgfn ("test_rewrx.out") ;

	// read
	CvrStgFile *file = CvrStgFile::readFile (cvrfn) ;

	// embed
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		SampleValue *oldsample = file->getSampleValue (i) ;
		if (oldsample->getBit() != emb[i]) {
			SampleValue *newsample = oldsample->getNearestOppositeSampleValue() ;
			file->replaceSample (i, newsample) ;
			delete newsample ;
		}
		delete oldsample ;
	}

	// write
	file->transform (stgfn) ;
	file->write() ;
	delete file ;

	// read
	file = CvrStgFile::readFile (stgfn) ;

	// extract and compare
	bool retval = true ;
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		if (file->getSampleBit(i) != emb[i]) {
			retval = false ;
		}
	}

	delete file ;
	removeFile (stgfn) ;
	return retval ;
}

bool CvrStgFileTest::genericTestPosition (const CvrStgFile* f, const SamplePos pos, SampleValue* sv_r) const
{
	SampleValue* sv_o = f->getSampleValue (pos) ;
	bool retval = (*sv_o == *sv_r) ;
	if (ArgVerbose && !retval) {
		std::cerr << std::endl << "---- FAILED: genericTestPosition ----" << std::endl ;
		std::cerr << "file->getSampleValue (" << pos << ") has key: " << std::hex << sv_o->getKey() << std::dec << std::endl ;
		std::cerr << "should be: " << std::hex << sv_r->getKey() << std::dec << std::endl ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	delete sv_o ;
	delete sv_r ;
	return retval ;
}

bool CvrStgFileTest::areEqual (const std::string& fn1, const std::string& fn2) const
{
	char command[256] ;
	sprintf (command, "%s %s %s", COMPARE, fn1.c_str(), fn2.c_str()) ;
	return (system(command) ? false : true) ;
}

void CvrStgFileTest::removeFile (const std::string& fn) const
{
	char command[256] ;
	sprintf (command, "%s %s", REMOVE, fn.c_str()) ;
	system (command) ;
}
