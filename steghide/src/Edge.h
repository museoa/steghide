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

class SampleValue ;
class Vertex ;

class Edge {
	public:
	/**
	 * constructs an edge object (calculates the weight)
	 **/
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2) ;
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, UWORD32 w) ;

	Vertex *getVertex1 (void) const
		{ return Vertex1 ; } ;

	Vertex *getVertex2 (void) const
		{ return Vertex2 ; } ;

	unsigned short getIndex1 (void) const
		{ return Index1 ; } ;

	unsigned short getIndex2 (void) const
		{ return Index2 ; } ;

	UWORD32 getWeight (void) ;

	bool operator== (const Edge& e) const ;

	/**
	 * swap vertices 1 and 2 in this edge (weight is not altered)
	 **/
	void swap (void) ;

	/**
	 * \return true iff this edge contains the vertex v
	 **/
	bool contains (const Vertex *v) const ;

	/**
	 * get the vertex on this edge that is not equal to v
	 **/
	Vertex *getOtherVertex (const Vertex *v) const ;

	/**
	 * get the position of the sample that should be changed to embed the bit represented by the vertex v
	 **/
	SamplePos getSamplePos (Vertex* v) const ;

	/**
	 * get the old sample value that will be replaced to embed the bit represented by the vertex v
	 **/
	SampleValue *getOriginalSampleValue (Vertex *v) const ;

	/**
	 * get the sample value that should replace the previous sample value to embed the bit represented by the vertex v
	 **/
	SampleValue *getReplacingSampleValue (Vertex *v) const ;

#ifdef DEBUG
	void print (unsigned short spc = 0) ;
#endif
	
	private:
	Vertex *Vertex1 ;
	/// contains the index of the sample (of those in Vertex1) that will be changed (if this edge is used)
	unsigned short Index1 ;
	
	Vertex *Vertex2 ;
	/// contains the index of the sample (of those in Vertex2) that will be changed (if this edge is used)
	unsigned short Index2 ;

	/// contains the weight of this edge or UWORD32_MAX if it has not been calculated yet
	UWORD32 Weight ;
} ;

#endif // ndef SH_EDGE_H
