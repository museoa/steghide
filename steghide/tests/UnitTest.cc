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

#include "SteghideError.h"

#include "TestCategory.h"
#include "UnitTest.h"
#include "TestSuite.h"

UnitTest::~UnitTest()
{
	for (std::vector<TestCategory*>::const_iterator it = TestCategories.begin() ; it != TestCategories.end() ; it++) {
		delete *it ;
	}
}

void UnitTest::run()
{
	getSuite()->startUnit (getName()) ;
	for (std::vector<TestCategory*>::const_iterator it = TestCategories.begin() ; it != TestCategories.end() ; it++) {
		getSuite()->startCategory ((*it)->getName()) ;
		try {
			(*it)->run() ;
		}
		catch (SteghideError& e) {
			getSuite()->addTestResult (TestSuite::EXCEPTION) ;
			e.printMessage() ;
		}
		getSuite()->endCategory ((*it)->getName()) ;
	}
	getSuite()->endUnit (getName()) ;
}

void UnitTest::addTestCategory (TestCategory* tc)
{
	TestCategories.push_back (tc) ;
}

void UnitTest::addTestResult (bool r)
{
	getSuite()->addTestResult (r) ;
}
