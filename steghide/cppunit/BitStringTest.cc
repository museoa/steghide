#include <iostream>

#include <cppunit/TestCaller.h>

#include "BitStringTest.h"

void BitStringTest::setUp()
{
	bs_0 = new BitString() ;
	bs_0->append(false) ;

	bs_1 = new BitString() ;
	bs_1->append(true) ;

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

void BitStringTest::tearDown()
{
	delete bs_0 ;
	delete bs_1 ;
	delete bs_10010 ;
	delete bs_10101110 ;
	delete bs_101011101 ;
}

void BitStringTest::registerTests (CppUnit::TestSuite* suite)
{
	suite->addTest (new CppUnit::TestCaller<BitStringTest> ("Bit-by-Bit Input/Output", &BitStringTest::testBitInputOutput, *this)) ;
	suite->addTest (new CppUnit::TestCaller<BitStringTest> ("Length", &BitStringTest::testLength, *this)) ;
	suite->addTest (new CppUnit::TestCaller<BitStringTest> ("Datatype Input", &BitStringTest::testDatatypeInput, *this)) ;
	suite->addTest (new CppUnit::TestCaller<BitStringTest> ("Datatype Output", &BitStringTest::testDatatypeOutput, *this)) ;
	suite->addTest (new CppUnit::TestCaller<BitStringTest> ("Equality", &BitStringTest::testEquality, *this)) ;
}

void BitStringTest::testBitInputOutput()
{
	CPPUNIT_ASSERT((*bs_0)[0] == false) ;
	CPPUNIT_ASSERT((*bs_1)[0] == true) ;
	CPPUNIT_ASSERT(	(*bs_10010)[0] == true &&
					(*bs_10010)[1] == false &&
					(*bs_10010)[2] == false &&
					(*bs_10010)[3] == true &&
					(*bs_10010)[4] == false) ;
	CPPUNIT_ASSERT( (*bs_10101110)[0] == true &&
					(*bs_10101110)[1] == false &&
					(*bs_10101110)[2] == true &&
					(*bs_10101110)[3] == false &&
					(*bs_10101110)[4] == true &&
					(*bs_10101110)[5] == true &&
					(*bs_10101110)[6] == true &&
					(*bs_10101110)[7] == false) ;
	CPPUNIT_ASSERT( (*bs_101011101)[0] == true &&
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
	CPPUNIT_ASSERT(bs_0->getLength() == 1) ;
	CPPUNIT_ASSERT(bs_1->getLength() == 1) ;
	CPPUNIT_ASSERT(bs_10010->getLength() == 5) ;
	CPPUNIT_ASSERT(bs_10101110->getLength() == 8) ;
	CPPUNIT_ASSERT(bs_101011101->getLength() == 9) ;
}

void BitStringTest::testDatatypeInput()
{
	{
		UWORD16 v = 0xFF81 ;
		BitString bs ;
		bs.append (v, 10) ;
		CPPUNIT_ASSERT (bs.getLength() == 10) ;
		CPPUNIT_ASSERT (bs.Data[0] == 0x81 &&
						bs.Data[1] == 0x03) ;
	}

	{
		std::string str ("test") ;
		BitString bs (str) ;
		CPPUNIT_ASSERT (bs.getLength() == 32) ;
		CPPUNIT_ASSERT (bs.Data[0] == 't' &&
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
		CPPUNIT_ASSERT (bs.getLength() == 24) ;
		CPPUNIT_ASSERT (bs.Data[0] == 123 &&
						bs.Data[1] == 45 &&
						bs.Data[2] == 6) ;
	}

	{
		BitString bs1 ;
		bs1.append(true).append(false).append(true).append(false) ;
		BitString bs2 ;
		bs2.append(false).append(true).append(true).append(false).append(bs1) ;
		CPPUNIT_ASSERT (bs2.getLength() == 8) ;
		CPPUNIT_ASSERT (bs2.Data[0] = 0x6A) ;
	}
}

void BitStringTest::testDatatypeOutput()
{
	CPPUNIT_ASSERT (bs_10010->getValue(1, 2) == 0) ;

	{
		std::vector<BYTE> vec ;
		vec.push_back(0x88) ;
		vec.push_back(0x07) ;
		BitString bs (vec) ;
		CPPUNIT_ASSERT (bs.getValue(4, 8) == 0x78) ;
	}

	{
		BitString bs = bs_10101110->getBits(2, 4) ;
		CPPUNIT_ASSERT (bs.getLength() == 4) ;
		CPPUNIT_ASSERT (bs[0] == true &&
						bs[1] == false &&
						bs[2] == true &&
						bs[3] == true) ;
	}

	{
		std::vector<BYTE> vec = bs_10101110->getBytes() ;
		CPPUNIT_ASSERT (vec.size() == 1) ;
		CPPUNIT_ASSERT (vec[0] == 0x75) ;
	}
}

void BitStringTest::testEquality()
{
	CPPUNIT_ASSERT (*bs_0 == *bs_0) ;
	CPPUNIT_ASSERT (!(*bs_0 == *bs_1)) ;

	{
		BitString bs (std::string("a test")) ;
		CPPUNIT_ASSERT (bs == bs) ;
	}

	{
		BitString bs1 ;
		bs1.append ((BYTE) 0xAB) ;
		BitString bs2 ;
		bs2.append(true).append(true).append(false).append(true) ;
		bs2.append(false).append(true).append(false).append(true) ;
		CPPUNIT_ASSERT (bs1 == bs2) ;
	}
}
