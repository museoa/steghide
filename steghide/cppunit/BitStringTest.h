#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#define private public
#define protected public
#include "bitstring.h"
#undef private
#undef protected

class BitStringTest : public CppUnit::TestCase {
	public:
	BitStringTest (std::string name) : CppUnit::TestCase (name) {} ;

	void setUp (void) ;
	void tearDown (void) ;
	void registerTests (CppUnit::TestSuite* suite) ;

	void testBitInputOutput (void) ;
	void testLength (void) ;
	void testDatatypeInput (void) ;
	void testDatatypeOutput (void) ;
	void testEquality (void) ;

	private:
	BitString *bs_0, *bs_1, *bs_10010, *bs_10101110, *bs_101011101 ;

} ;
