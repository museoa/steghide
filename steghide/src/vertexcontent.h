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
#include <list>

#include "common.h"
#include "graphaccess.h"
#include "samplevalue.h"
#include "wrapper_hash_set.h"
// declared here to prevent circulating includes
class Graph ;
class Vertex ;

/**
 * \class VertexContent
 * \brief the content of a vertex
 *
 * A vertex content is essentially a k-tuple of sample values, where
 * k is the number of samples per vertex. In a VertexContent object
 * the sample values are sorted by their getKey()s to make it easier
 * to compare two vertex contents.
 **/
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

	~VertexContent (void) ;

	bool operator== (const VertexContent& vc) const ;

	bool operator!= (const VertexContent& vc) const
		{ return !(operator==(vc)) ; } ;

	SampleValue *getSampleValue (unsigned short i) const
		{ return SampleValues[i] ; } ; 

	SampleValue **getSampleValues (void) const
		{ return SampleValues ; } ;

	size_t getHash (void) const ;

	unsigned long getDegree (void) const ;

	/**
	 * add a vertex to the Occurences std::list of this vertex content
	 * \param v the vertex to add
	 * \return the iterator pointing to the added vertex in the Occurences std::list (should be used as argument to deleteFromOccurences)
	 **/
	std::list<Vertex*>::iterator addOccurence (Vertex *v)
		{ return Occurences.insert (Occurences.end(), v) ; } ;

	/**
	 * delete a vertex from the Occurences std::list
	 * \param it the iterator pointing to the vertex to be deleted
	 **/
	std::list<Vertex*>::iterator markDeletedFromOccurences (std::list<Vertex*>::iterator it) ;

	std::list<Vertex*>::iterator unmarkDeletedFromOccurences (std::list<Vertex*>::iterator it) ;
	
	const std::list<Vertex*>& getOccurences (void) const
		{ return Occurences ; } ;

	bool hasOccurences (void) const
		{ return !Occurences.empty() ; } ;

#ifdef DEBUG
	void print (unsigned short spc = 0) const ;
#endif

	private:
	/// the sample values of this vertex content
	SampleValue** SampleValues ;
	/// the number of loop edges vertices with this content _would_ have
	unsigned long SelfDegree ;
	/// the vertices whose content is this vertex content
	std::list<Vertex*> Occurences ;
	/// the vertices that have been delted from Occurences
	std::list<Vertex*> DeletedOccurences ;
} ;

struct VertexContentsEqual : std::binary_function<VertexContent*, VertexContent*, bool> {
	bool operator() (const VertexContent *vc1, const VertexContent* vc2) const ;
} ;

namespace NAMESPACE_SGI
{

struct sgi::hash<VertexContent*> {
	size_t operator() (const VertexContent* vc) const ;
} ;

}

#endif // ndef SH_DEGREE_H
