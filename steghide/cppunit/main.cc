#include <cstdlib>

#include <cppunit/TestSuite.h>
#include <cppunit/TextTestResult.h>

#include "BitStringTest.h"

int main (void)
{
	CppUnit::TestSuite suite ;

	BitStringTest bst ("BitString Test") ;
	bst.registerTests (&suite) ;

	CppUnit::TextTestResult res ;
	suite.run (&res) ;
	std::cout << res << std::endl ;

	if (res.wasSuccessful()) {
		exit (EXIT_SUCCESS) ;
	}
	else {
		exit (EXIT_FAILURE) ;
	}
}
