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

#ifndef SH_EDGEITERATOR_H
#define SH_EDGEITERATOR_H

#include "common.h"
#include "graph.h"
#include "graphaccess.h"
#include "sampleoccurence.h"
#include "vertex.h"

/**
 * \class EdgeIterator
 * \brief allows an iteration trough all edges of a vertex
 *
 * The Vertex that is the source for all edges is called "source vertex".
 * The order of the iteration through the edges is from the shortest to the longest edge.
 *
 * EdgeIterator uses an SampleOccurence::const_iterator to store information
 * about the current edge. Graph::(un)markDeletedSampleOccurence can invalidate
 * such iterators. It is therefore not a good idea to use EdgeIterators at the same
 * time as the Graph::(un)markDeletedSampleOccurence functionality.
 **/
class EdgeIterator : private GraphAccess {
	public:
	/**
	 * \param v the source vertex
	 **/
	EdgeIterator (Graph *g, Vertex *v) ;

	/**
	 * get the current edge
	 * \return the edge that is described by the current status of this EdgeIterator
	 *
	 * The returned Edge object is created in this function and
	 * should be deleted by the caller.
	 **/
	Edge* operator* (void) ;

	/**
	 * set this iterator to next edge
	 **/
	EdgeIterator& operator++ (void) ;

	/**
	 * reset this iterator to first (shortest) edge
	 **/
	void reset (void) ;

	private:
	/// the vertex that is common to all edges this edge iterator will iterate trough
	Vertex* SrcVertex ;

	/// the index of the sample value in the source vertex that is part of the current edge
	unsigned short SrcIndex ;

	/// contains (for every sample value) an index to the current opposite neighbour
	unsigned long* SVOppNeighsIndices ;

	/**
	 * contains the iterator pointing to the sample occurence that constitutes
	 * the edge together with SourceVertex/SourceSamleValueIndex
	 **/
	list<SampleOccurence>::const_iterator SampleOccurenceIt ;

	/// is true iff there are no more edges for this source vertex
	bool Finished ;

	/**
	 * choose i < SamplesPerVertex such that SVOppNeighsIndices[i] has at least one occurence in the graph
	 * and has the least distance from it's corresponding source sample value SrcVertex->getSampleValue(i).
	 * If no such i can be found, Finished is set to true.
	 **/
	unsigned short findShortestSampleValue (void) ;
} ;

#endif // ndef SH_EDGEITERATOR_H
