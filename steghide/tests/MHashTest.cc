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

#include <iostream>

#include "MHashTest.h"

#include "BitString.h"

MHashTest::MHashTest (TestSuite* s)
	: UnitTest ("MHash", s)
{
	ADDTESTCATEGORY (MHashTest, testMD5) ;
	ADDTESTCATEGORY (MHashTest, testCRC32) ;
}

void MHashTest::testMD5 (void)
{
	BYTE shouldbe1[] = { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("")), shouldbe1)) ;

	BYTE shouldbe2[] = { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("a")), shouldbe2)) ;

	BYTE shouldbe3[] = { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("abc")), shouldbe3)) ;

	BYTE shouldbe4[] = { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("message digest")), shouldbe4)) ;

	BYTE shouldbe5[] = { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("abcdefghijklmnopqrstuvwxyz")), shouldbe5)) ;

	BYTE shouldbe6[] = { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")), shouldbe6)) ;

	BYTE shouldbe7[] = { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a } ;
	addTestResult (genericTestMHash (MHASH_MD5, BitString (std::string("12345678901234567890123456789012345678901234567890123456789012345678901234567890")), shouldbe7)) ;
}

void MHashTest::testCRC32 (void)
{
	// FIXME - these test vectors don't work
#if 0
	std::vector<unsigned char> data (40) ;

	for (unsigned int i = 0 ; i < 40 ; i++) {
		data[i] = 0x00 ;
	}
	BYTE shouldbe1[] = { 0x86, 0x4d, 0x7f, 0x99 } ;
	addTestResult (genericTestMHash (MHASH_CRC32, BitString(data), shouldbe1)) ;
#endif

	// the test vector used by libmhash
	BYTE shouldbe2[] = { 0x7F, 0xBE, 0xB0, 0x2E } ;
	addTestResult (genericTestMHash (MHASH_CRC32, BitString(std::string("checksum")), shouldbe2)) ;
}

bool MHashTest::genericTestMHash (hashid a, BitString data, BYTE* shouldbe)
{
	bool retval = true ;

	MHash hash (a) ;
	hash << data << MHash::endhash ;
	const std::vector<unsigned char>& result = hash.getHashBytes() ;
	for (unsigned short i = 0 ; i < result.size() ; i++) {
		if (result[i] != shouldbe[i]) {
			retval = false ;
		}
	}

	if (!retval) {
		std::cerr << std::endl << "---" << std::endl ;
		std::cerr << "genericTestMHash failed" << std::endl ;
		std::cerr << "shouldbe: " ;
		for (unsigned short i = 0 ; i < result.size() ; i++) {
			std::cerr << (unsigned short) shouldbe[i] ;
		}
		std::cerr << std::endl << "result:   " ;
		for (unsigned short i = 0 ; i < result.size() ; i++) {
			std::cerr << (unsigned short) result[i] ;
		}
		std::cerr << std::dec << std::endl ;
		std::cerr << "---" << std::endl << std::endl ;
	}

	return retval ;
}
