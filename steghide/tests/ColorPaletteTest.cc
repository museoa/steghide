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

#include "ColorPaletteTest.h"
#include "utcommon.h"

#include "ColorPalette.h"

ColorPaletteTest::ColorPaletteTest (TestSuite* s)
	: UnitTest ("ColorPalette", s)
{
	ADDTESTCATEGORY (ColorPaletteTest, testGetNearest) ;
}

void ColorPaletteTest::setup ()
{
	UnitTest::setup() ;

	pal1 = new ColorPalette() ;
	pal1->addEntry (100, 100, 100) ;
	pal1->addEntry (245, 23, 17) ;
	pal1->addEntry (101, 99, 101) ;
	pal1->addEntry (135, 0, 9) ;
	pal1->addEntry (15, 98, 128) ;
	pal1->addEntry (102, 100, 102) ;
}

void ColorPaletteTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete pal1 ;
}

void ColorPaletteTest::testGetNearest ()
{
	addTestResult (pal1->getNearest(0) == 2) ;
	addTestResult (pal1->getNearest(0, ColorPalette::ODDINDICES) == 5) ;
	addTestResult (pal1->getNearest(0, ColorPalette::EVENINDICES) == 2) ;
}
