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

#ifndef SH_EDGE_H
#define SH_EDGE_H

#include "common.h"
// declared here to prevent circulating #includes
class SampleValue ;
class Vertex ;

class Edge {
	public:
	/**
	 * constructs an edge object (calculates the weight)
	 **/
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2) ;
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, float w) ;

	Vertex *getVertex1 (void) const
		{ return Vertex1 ; } ;

	Vertex *getVertex2 (void) const
		{ return Vertex2 ; } ;

	float getWeight (void) const
		{ return Weight ; } ;

	/**
	 * get the vertex on this edge that is not equal to v
	 **/
	Vertex *getOtherVertex (Vertex *v) const ;

	/**
	 * get the position of the sample that should be changed to embed the bit represented by the vertex v
	 **/
	SamplePos getSamplePos (Vertex *v) const ;

	/**
	 * get the old sample value that will be replaced to embed the bit represented by the vertex v
	 * warning: this function must be used before the sample has been replaced in the cvrstgfile
	 **/
	SampleValue *getOriginalSampleValue (Vertex *v) const ;

	/**
	 * get the sample value that should replace the previous sample value to embed the bit represented by the vertex v
	 * warning: this function must be used before the sample has been replaced in the cvrstgfile
	 **/
	SampleValue *getReplacingSampleValue (Vertex *v) const ;
	
	private:
	Vertex *Vertex1 ;
	/// contains the index of the sample (of those in Vertex1) that will be changed (if this edge is used)
	unsigned short Index1 ;
	
	Vertex *Vertex2 ;
	/// contains the index of the sample (of those in Vertex2) that will be changed (if this edge is used)
	unsigned short Index2 ;

	/// contains the weight of this edge
	float Weight ;
} ;

#endif // ndef SH_EDGE_H