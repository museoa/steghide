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

#ifndef SH_VERTEX_H
#define SH_VERTEX_H

#include <vector>

#include "common.h"
#include "cvrstgsample.h"

/**
 * \class Vertex
 * \brief a vertex in a graph
 **/
class Vertex {
	public:
	Vertex (void) ;
	Vertex (vector<SamplePos> sposs, vector<CvrStgSample*> ss) ;

	/**
	 * of how many samples does this vertex consist ?
	 * \return the number of samples this vertex consists of
	 **/
	unsigned int getNumSamples (void) const ;

	/**
	 * get the i-th sample position
	 * \param i an index of a sample in this vertex
	 * \return the position of the sample in the associated cvrstgfile
	 **/
	SamplePos getSamplePos (unsigned int i) const ;

	/**
	 * get the i-th samle value
	 * \param i an index of a sample in this vertex
	 * \return the value of the sample in the associated cvrstgfile
	 **/
	CvrStgSample *getSample (unsigned int i) const ;

	private:
	vector<SamplePos> SamplePositions ;
	vector<CvrStgSample*> Samples ;
} ;

#endif // ndef SH_VERTEX_H
