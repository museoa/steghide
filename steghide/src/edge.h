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

#ifndef SH_EDGE_H
#define SH_EDGE_H

#include "common.h"
// declared here to prevent circulating #includes
class CvrStgSample ;
class Vertex ;

class Edge {
	public:
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, unsigned long w)
		: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2), Weight(w) {}

	Vertex *getVertex1 (void) const ;
	Vertex *getVertex2 (void) const ;
	unsigned long getWeight (void) const ;

	/**
	 * get the position of the sample that should be changed to embed the bit represented by the vertex v
	 **/
	SamplePos getSamplePos (Vertex *v) const ;

	/**
	 * get the sample value that should replace the previous sample value to embed the bit represented by the vertex v
	 **/
	CvrStgSample *getSample (Vertex *v) const ;
	
	private:
	Vertex *Vertex1 ;
	/// contains the index of the sample (of those in Vertex1) that will be changed (if this edge is used)
	unsigned short Index1 ;
	
	Vertex *Vertex2 ;
	/// contains the index of the sample (of those in Vertex2) that will be changed (if this edge is used)
	unsigned short Index2 ;

	/// contains the weight of this edge
	unsigned long Weight ;
} ;

#endif // ndef SH_EDGE_H
