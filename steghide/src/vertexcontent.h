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

#ifndef SH_DEGREE_H
#define SH_DEGREE_H

#include <functional>
#include <hash_set>
#include <list>
#include <vector>

#include "common.h"
#include "graphaccess.h"
#include "samplevalue.h"
// declared here to prevent circulating includes
class Graph ;
class Vertex ;

class VertexContent : private GraphAccess {
	public:
	/**
	 * construct a vertex content object using the given sample values
	 * \param g the graph this vertex content will be part of
	 * \param svs the sample values that this vertex content will contain
	 * \param sposs the positions corresponding to the sample values
	 *
	 * Both, the svs and the sposs arrays will be sorted according to the
	 * keys of the sample values. Only the sample values will be kept, but the
	 * sample positions will be sorted too (by corresponding sample value keys).
	 **/
	VertexContent (Graph* g, SampleValue**& svs, SamplePos*& sposs) ;

	bool operator== (VertexContent vc) const ;

	bool operator!= (VertexContent vc) const
		{ return !(operator==(vc)) ; } ;

	// FIXME - ev. stattdessen operator [] - ev. const return value
	SampleValue *getSampleValue (unsigned short i) const
		{ return SampleValues[i] ; } ; 

	// FIXME - konstante Rückgabe
	SampleValue **getSampleValues (void) const
		{ return SampleValues ; } ;

	size_t getHash (void) const ;

	unsigned long getDegree (void) const ;

#if 0
	/**
	 * add a vertex to the Occurences list of this vertex content
	 * \param v the vertex to add
	 * \return the iterator pointing to the added vertex in the Occurences list (should be used as argument to deleteFromOccurences)
	 **/
	list<Vertex*>::iterator addOccurence (Vertex *v)
		{ return Occurences.insert (Occurences.end(), v) ; } ;

	/**
	 * delete a vertex from the Occurences list
	 * \param it the iterator pointing to the vertex to be deleted
	 **/
	list<Vertex*>::iterator markDeletedFromOccurences (list<Vertex*>::iterator it) ;

	list<Vertex*>::iterator unmarkDeletedFromOccurences (list<Vertex*>::iterator it) ;
#endif
	
	private:
	/// the sample values of this vertex content
	SampleValue** SampleValues ;
	/// the number of loop edges vertices with this content _would_ have
	unsigned long SelfDegree ;
#if 0
	/// the vertices whose content is this vertex content
	list<Vertex*> Occurences ;
	/// the vertices that have been delted from Occurences
	list<Vertex*> DeletedOccurences ;
#endif
} ;

struct VertexContentsEqual : binary_function<VertexContent*, VertexContent*, bool> {
	bool operator() (const VertexContent *vc1, const VertexContent* vc2) const ;
} ;

struct hash<VertexContent*> {
	size_t operator() (const VertexContent *vc) const ;
} ;

#endif // ndef SH_DEGREE_H
