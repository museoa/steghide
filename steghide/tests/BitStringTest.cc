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

#include "common.h"

#include "BitStringTest.h"
#include "TestCategoryCaller.h"

BitStringTest::BitStringTest (TestSuite* s)
	: UnitTest ("BitString", s)
{
	ADDTESTCATEGORY (BitStringTest, testBitInputOutput) ;
	ADDTESTCATEGORY (BitStringTest, testLength) ;
	ADDTESTCATEGORY (BitStringTest, testDatatypeInput) ;
	ADDTESTCATEGORY (BitStringTest, testDatatypeOutput) ;
	ADDTESTCATEGORY (BitStringTest, testEquality) ;
	ADDTESTCATEGORY (BitStringTest, testTruncation) ;
	ADDTESTCATEGORY (BitStringTest, testCompression) ;
}

void BitStringTest::setup ()
{
	bs_0 = new BitString() ;
	bs_0->append(false) ;

	bs_1 = new BitString() ;
	bs_1->append(true) ;

	bs_100 = new BitString() ;
	bs_100->append(true) ;
	bs_100->append(false) ;
	bs_100->append(false) ;

	bs_10010 = new BitString() ;
	bs_10010->append(true) ;
	bs_10010->append(false) ;
	bs_10010->append(false) ;
	bs_10010->append(true) ;
	bs_10010->append(false) ;

	bs_10101110 = new BitString() ;
	bs_10101110->append(true) ;
	bs_10101110->append(false) ;
	bs_10101110->append(true) ;
	bs_10101110->append(false) ;
	bs_10101110->append(true) ;
	bs_10101110->append(true) ;
	bs_10101110->append(true) ;
	bs_10101110->append(false) ;

	bs_101011101 = new BitString() ;
	bs_101011101->append(true) ;
	bs_101011101->append(false) ;
	bs_101011101->append(true) ;
	bs_101011101->append(false) ;
	bs_101011101->append(true) ;
	bs_101011101->append(true) ;
	bs_101011101->append(true) ;
	bs_101011101->append(false) ;
	bs_101011101->append(true) ;
}

void BitStringTest::cleanup ()
{
	delete bs_0 ;
	delete bs_1 ;
	delete bs_100 ;
	delete bs_10010 ;
	delete bs_10101110 ;
	delete bs_101011101 ;
}

void BitStringTest::testBitInputOutput()
{
	addTestResult((*bs_0)[0] == false) ;
	addTestResult((*bs_1)[0] == true) ;
	addTestResult(	(*bs_10010)[0] == true &&
					(*bs_10010)[1] == false &&
					(*bs_10010)[2] == false &&
					(*bs_10010)[3] == true &&
					(*bs_10010)[4] == false) ;
	addTestResult( (*bs_10101110)[0] == true &&
					(*bs_10101110)[1] == false &&
					(*bs_10101110)[2] == true &&
					(*bs_10101110)[3] == false &&
					(*bs_10101110)[4] == true &&
					(*bs_10101110)[5] == true &&
					(*bs_10101110)[6] == true &&
					(*bs_10101110)[7] == false) ;
	addTestResult( (*bs_101011101)[0] == true &&
					(*bs_101011101)[1] == false &&
					(*bs_101011101)[2] == true &&
					(*bs_101011101)[3] == false &&
					(*bs_101011101)[4] == true &&
					(*bs_101011101)[5] == true &&
					(*bs_101011101)[6] == true &&
					(*bs_101011101)[7] == false &&
					(*bs_101011101)[8] == true) ;

}

void BitStringTest::testLength()
{
	addTestResult(bs_0->getLength() == 1) ;
	addTestResult(bs_1->getLength() == 1) ;
	addTestResult(bs_10010->getLength() == 5) ;
	addTestResult(bs_10101110->getLength() == 8) ;
	addTestResult(bs_101011101->getLength() == 9) ;
}

void BitStringTest::testDatatypeInput()
{
	{
		UWORD16 v = 0xFF81 ;
		BitString bs ;
		bs.append (v, 10) ;
		addTestResult (bs.getLength() == 10 &&
						bs.Data[0] == 0x81 &&
						bs.Data[1] == 0x03) ;
	}

	{
		std::string str ("test") ;
		BitString bs (str) ;
		addTestResult (bs.getLength() == 32 &&
						bs.Data[0] == 't' &&
						bs.Data[1] == 'e' &&
						bs.Data[2] == 's' &&
						bs.Data[3] == 't') ;
	}

	{
		std::vector<BYTE> vec ;
		vec.push_back (123) ;
		vec.push_back (45) ;
		vec.push_back (6) ;
		BitString bs (vec) ;
		addTestResult (bs.getLength() == 24 &&
						bs.Data[0] == 123 &&
						bs.Data[1] == 45 &&
						bs.Data[2] == 6) ;
	}

	{
		BitString bs1 ;
		bs1.append(true).append(false).append(true).append(false) ;
		BitString bs2 ;
		bs2.append(false).append(true).append(true).append(false).append(bs1) ;
		addTestResult (bs2.getLength() == 8 && bs2.Data[0] == 0x56) ;
	}
}

void BitStringTest::testDatatypeOutput()
{
	addTestResult (bs_10010->getValue(1, 2) == 0) ;

	{
		std::vector<BYTE> vec ;
		vec.push_back(0x88) ;
		vec.push_back(0x07) ;
		BitString bs (vec) ;
		addTestResult (bs.getValue(4, 8) == 0x78) ;
	}

	{
		BitString bs = bs_10101110->getBits(2, 4) ;
		addTestResult (bs.getLength() == 4 &&
						bs[0] == true &&
						bs[1] == false &&
						bs[2] == true &&
						bs[3] == true) ;
	}

	{
		std::vector<BYTE> vec = bs_10101110->getBytes() ;
		addTestResult (vec.size() == 1 && vec[0] == 0x75) ;
	}
}

void BitStringTest::testEquality()
{
	addTestResult (*bs_0 == *bs_0) ;
	addTestResult (!(*bs_0 == *bs_1)) ;

	{
		BitString bs (std::string("a test")) ;
		addTestResult (bs == bs) ;
	}

	{
		BitString bs1 ;
		bs1.append ((BYTE) 0xAB) ;
		BitString bs2 ;
		bs2.append(true).append(true).append(false).append(true) ;
		bs2.append(false).append(true).append(false).append(true) ;
		addTestResult (bs1 == bs2) ;
	}
}

void BitStringTest::testTruncation ()
{
	{
		BitString bs1 = *bs_10010 ;
		BitString bs2 = *bs_100 ;
		addTestResult (bs1.truncate(0, 3) == bs2) ;
	}

	{
		BitString bs1 = *bs_10101110 ;
		BitString bs2 ; 
		bs2.append(true).append(false).append(true).append(true) ;
		addTestResult (bs1.truncate(2, 6) == bs2) ;
	}
}

void BitStringTest::testCompression ()
{
#ifdef USE_ZLIB
	{
		BitString bs1 (std::string("a test of tests of some recurrence")) ;
		BitString bs2 = bs1 ;
		bs1.compress (9) ;
		bs1.uncompress (bs2.getLength()) ;
		addTestResult (bs1 == bs2) ;
	}

	{
		BitString bs1 (std::string("afafafafafafafaf")) ;
		BitString bs2 = bs1 ;
		bs1.compress(9) ;
		addTestResult (bs1 != bs2) ;
	}
#else
	addTestResult (TestSuite::NOTINSTALLED) ;
#endif // def USE_ZLIB
}
