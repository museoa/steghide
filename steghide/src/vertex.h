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

#ifndef SH_VERTEX_H
#define SH_VERTEX_H

#include <map>
#include <vector>

#include "common.h"
#include "edge.h"
#include "graph.h"
#include "graphaccess.h"
#include "sampleoccurence.h"
#include "samplevalue.h"
#include "vertexcontent.h"

/**
 * \class Vertex
 * \brief a vertex in a graph
 *
 * A vertex represents a bit that will cause a change to the cover-stego-file to be embedded.
 * A vertex consists of k samples (that is k sample values at k (different) positions in the
 * cover-stego-file), where k is TheCvrStgFile->getNumSamplesPerEBit(). One of these k samples
 * must be changed to an opposite sample to embed the bit that corresponds to this vertex.
 **/
class Vertex : private GraphAccess {
	public:
	/**
	 * construct a new vertex object
	 * \param g the graph this vertex will be part of
	 * \param l the vertex label for this vertex
	 * \param sposs the array (with length g->getSamplesPerEBit()) of the positions of the samples
	 * \param vc the corresponding vertex content
	 **/
	Vertex (Graph* g, VertexLabel l, SamplePos* sposs, VertexContent* vc) ;

	~Vertex (void) ;

	/**
	 * get the i-th sample position
	 * \param i an index of a sample in this vertex (must be < TheCvrStgFile->getNumSamplesPerEBit())
	 * \return the position of the sample in the associated cvrstgfile
	 **/
	SamplePos getSamplePos (unsigned short i) const
		{ return SamplePositions[i] ; } ;

	/**
	 * get the i-th sample value
	 * \param i an index of a sample in this vertex (must be < TheCvrStgFile->getNumSamplesPerEBit())
	 * \return the value of the sample in the associated cvrstgfile
	 **/
	SampleValue *getSampleValue (unsigned short i) const
		{ return Content->getSampleValue(i) ; } ;

	/**
	 * get the degree of this vertex (via it's vertex content)
	 **/
	unsigned long getDegree (void) const
		{ return Content->getDegree() ; } ;

	/**
	 * get the shortest edge of this vertex
	 **/
	Edge *getShortestEdge (void) const
		{ return ShortestEdge ; } ;

	/**
	 * find shortest edge of this vertex and save result to ShortestEdge
	 **/
	void updateShortestEdge (void) ;

	/**
	 * if this vertex is valid, mark it as deleted
	 **/
	void markDeleted (void) ;

	/**
	 * if this vertex is marked as deleted, undo this
	 **/
	void unmarkDeleted (void) ;

	bool operator== (const Vertex& v) const
		{ return getLabel() == v.getLabel() ; } ;

	VertexLabel getLabel (void) const
		{ return Label ; } ;

	void setLabel (VertexLabel l)
		{ Label = l ; } ;

	void setSampleOccurenceIt (unsigned short i, list<SampleOccurence>::iterator it)
		{ SampleOccurenceIts[i] = it ; }

#ifdef DEBUG
	void print (void) const ;
#endif

	private:
	/// the vertex label of this vertex
	VertexLabel Label ;

	/// the sample positions of the samples described by this vertex in the CvrStgFile
	SamplePos* SamplePositions ;

	/// the content of this vertex
	VertexContent* Content ;

#if 0
	/// points to an entry in the list of vertex occurences in the vertex content
	list<Vertex*>::iterator VertexOccurenceIt ;
#endif

	/// point to entries in lists of sample occurences in the graph
	list<SampleOccurence>::iterator* SampleOccurenceIts ;

	/// the shortest edge of this vertex (as calculated by updateShortestEdge)
	Edge *ShortestEdge ;

	/// true iff this vertex is not deleted
	bool valid ;
} ;

#endif // ndef SH_VERTEX_H
