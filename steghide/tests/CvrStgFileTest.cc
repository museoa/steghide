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

// TODO - do tests with new_wfn == false in *FileTests !
bool CvrStgFileTest::genericTestReadWrite (const std::string& rfn, bool new_wfn) const
{
	bool retval = false ;
	if (new_wfn) {
		CvrStgFile *file = CvrStgFile::readFile (rfn) ;
		std::string wfn ("test_rw.out") ;
		file->transform (wfn) ;
		file->write() ;
		delete file ;

		retval = areEqual (rfn, wfn) ;
		removeFile (wfn) ;
	}
	else {
		CvrStgFile *file = CvrStgFile::readFile (rfn) ;
		copyFile (rfn, "test_rw.bak") ;
		file->write() ;
		delete file ;

		retval = areEqual (rfn, "test_rw.bak") ;

		myassert(false) ;
		// FIXME - finish this
	}

	return retval ;
}

bool CvrStgFileTest::genericTestReadExtractCompare (const std::string& fn, const BitString& emb) const
{
	CvrStgFile *file = CvrStgFile::readFile (fn) ;
	bool ok = true ;
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		ok = (emb.getNAry(file->getEmbValueModulus(), i) == file->getEmbeddedValue(i)) && ok ;
	}
	delete file ;

	return ok ;
}

bool CvrStgFileTest::genericTestReadEmbedExtract (const std::string& fn, const BitString& emb) const
{
	CvrStgFile *file = CvrStgFile::readFile (fn) ;

	// embed data (at the beginning of the file)
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		SampleValue *oldsample = file->getSampleValue (i) ;
		EmbValue target = emb.getNAry(file->getEmbValueModulus(), i) ;
		if (oldsample->getEmbeddedValue() != target) {
			SampleValue* newsample = oldsample->getNearestTargetSampleValue (target) ;
			file->replaceSample (i, newsample) ;
			delete newsample ;
		}
		delete oldsample ;
	}

	// extract and compare
	bool retval = true ;
	for (unsigned long i = 0 ; i < emb.getLength() ; i++) {
		if (file->getEmbeddedValue(i) != emb.getNAry(file->getEmbValueModulus(), i)) {
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
		EmbValue target = emb.getNAry(file->getEmbValueModulus(), i) ;
		if (oldsample->getEmbeddedValue() != target) {
			SampleValue* newsample = oldsample->getNearestTargetSampleValue (target) ;
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
		if (file->getEmbeddedValue(i) != emb.getNAry(file->getEmbValueModulus(), i)) {
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

void CvrStgFileTest::copyFile (const std::string& src, const std::string& dest) const
{
	char command[256] ;
	sprintf (command, "%s %s %s", COPY, src.c_str(), dest.c_str()) ;
	system (command) ;
}
