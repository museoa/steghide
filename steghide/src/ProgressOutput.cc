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

ProgressOutput::ProgressOutput ()
{
	if (Args.EmbFn.getValue() == "") {
		EmbString = _("standard input") ;
	}
	else {
		EmbString = Args.EmbFn.getValue() ;
	}

	if (Args.CvrFn.getValue() == "") {
		CvrString = _("standard input") ;
	}
	else {
		CvrString = Args.CvrFn.getValue() ;
	}

	LastUpdate = time(NULL) - 1 ; // -1 to ensure that message is written first time
}
 
void ProgressOutput::update (float rate, bool done)
{
	if (done) {
		printf (_("\rembedding %s in %s...%.1f%% done\n"), EmbString.c_str(), CvrString.c_str(), rate * 100.0) ;
		fflush (stdout) ;
	}
	else {
		time_t now = time(NULL) ;
		if (LastUpdate < now) {
			LastUpdate = now ;
			printf (_("\rembedding %s in %s...%.1f%%"), EmbString.c_str(), CvrString.c_str(), rate * 100.0) ;
			fflush (stdout) ;
		}
	}
}
