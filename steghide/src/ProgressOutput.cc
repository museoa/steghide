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

#include <cmath>

#include "ProgressOutput.h"
#include "common.h"

ProgressOutput::ProgressOutput (unsigned short uf)
	: UpdateFrequency(uf), Counter(uf - 1) // init Counter to uf - 1 to make first update call write
{
	if (Args.EmbFn.getValue() == "") {
		EmbString = _("standard input") ;
	}
	else {
		EmbString = "\"" + Args.EmbFn.getValue() + "\"" ;
	}

	if (Args.CvrFn.getValue() == "") {
		CvrString = _("standard input") ;
	}
	else {
		CvrString = "\"" + Args.CvrFn.getValue() + "\"" ;
	}
}
 
void ProgressOutput::update (float rate)
{
	if (++Counter >= UpdateFrequency) {
		Counter = 0 ;
		printf (_("\rembedding %s in %s...%d%%"), EmbString.c_str(), CvrString.c_str(), makePercent(rate)) ;
	}
}

void ProgressOutput::done ()
{
	printf (_("\rembedding %s in %s...done\n"), EmbString.c_str(), CvrString.c_str()) ;
}

unsigned short ProgressOutput::makePercent (float x)
{
	x *= 100 ;
	unsigned short retval = (unsigned short) x ;
	if (x > (floor(x) + 0.5)) {
		retval++ ;
	}
	return retval ;
}
