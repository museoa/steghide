/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@teleweb.at>
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

#ifndef SH_EMBEDDER_H
#define SH_EMBEDDER_H

#include <string>
#include <vector>

#include "bitstring.h"
#include "common.h"
#include "cvrstgfile.h"
#include "graph.h"

class Embedder {
	public:
	Embedder (void) ;
	Embedder (string cfn, string sfn, string efn) ;

	void embed (void) ;

	protected:
	/**
	 * calculate everything that is necessary to respond to getIndex and getSample
	 **/
	void calculate (CvrStgFile *csf, BitString e) ;

	/**
	 * get the sample index that has been selected for modification for embedding of the n-th ebit
	 * of those who need modification of the cvrstgfile
	 * \param n the index of an ebit (of those who need modification of the cvrstgfile)
	 * \return the index of the sample that needs to be modified
	 **/
	SamplePos getIndex (unsigned long n) ;

	/**
	 * get the new sample value that has been selected for embedding of the n-th ebit
	 * of those who need modification of the cvrstgfile
	 * \param n the index of an ebit (of those who need modification of the cvrstgfile)
	 * \return a pointer to a sample value
	 **/
	CvrStgSample *getSample (unsigned long n) ;

	private:
	string CoverFileName ;
	string StegoFileName ;
	string EmbedFileName ;
	/// NeedsChange[i] is true iff the cover file needs to be changed to embed the i-th ebit
	vector<bool> NeedsChange ;
	Graph MGraph ;
} ;

#endif // ndef SH_EMBEDDER_H
