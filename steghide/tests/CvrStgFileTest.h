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

#ifndef SH_CVRSTGFILETEST_H
#define SH_CVRSTGFILETEST_H

#include "BitString.h"
#include "CvrStgFile.h"

#include "UnitTest.h"
#include "TestSuite.h"

class CvrStgFileTest : public UnitTest {
	public:
	CvrStgFileTest (std::string n, TestSuite* s) : UnitTest(n,s) {} ;

	protected:
	bool genericTestReadWrite (const std::string& rfn) const ;
	bool genericTestReadEmbedExtract (const std::string& fn, const BitString& emb) const ;
	bool genericTestReadEmbedWriteReadExtract (const std::string& cvrfn, const BitString& emb) const ;

	private:
	bool areEqual (const std::string& fn1, const std::string& fn2) const ;
	void removeFile (const std::string& fn) const ;
} ;

#endif // ndef SH_CVRSTGFILETEST_H
