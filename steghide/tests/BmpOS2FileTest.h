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

#ifndef SH_BMPOS2FILETEST_H
#define SH_BMPOS2FILETEST_H

#include "BmpFile.h"

#include "CvrStgFileTest.h"

class BmpOS2FileTest : public CvrStgFileTest {
	public:
	BmpOS2FileTest (TestSuite* s) ;
	~BmpOS2FileTest (void) ;

	void testReadWrite (void) ;
	void testReadEmbedExtract (void) ;
	void testReadEmbedWriteReadExtract (void) ;
	void testPosition (void) ;

	private:
	std::string *datadir ;
	BitString *bs1, *bs2, *bs3, *bs4 ;
	CvrStgFile *f1, *f2, *f3, *f4 ;
} ;

#endif // ndef SH_BMPOS2FILETEST_H
