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

#ifndef SH_AUGMENTINGPATHHEURISTIC
#define SH_AUGMENTINGPATHHEURISTIC

#include "EdgeIterator.h"
#include "MatchingAlgorithm.h"
#include "Vertex.h"

class Edge ;
#include "Graph.h"
class Matching ;

/**
 * \class AugmentingPathHeuristic
 * \brief a matching algorithm implementing a heuristic search for augmenting paths
 *
 * This class implements the heuristic augmenting path search presented by
 * Rolf H. Moehring and Matthias Mueller-Hannemann in their paper: "Cardinality
 * Matching: Heuristic Search for Augmenting Paths".
 **/
class AugmentingPathHeuristic : public MatchingAlgorithm {
	public:
	/**
	 * construct an AugmentingPathHeuristic object
	 * \param g the graph on which this heuristic should run
	 * \param m the matching to start with
	 * \param goal the percentage of matched vertices that should be reached
	 * \param mne the maximum number of edges that should be considered for every vertex
	 * \param mo the mode for edge iteration
	 **/
	AugmentingPathHeuristic (Graph* g, Matching* m, float goal, UWORD32 mne = UWORD32_MAX, EdgeIterator::ITERATIONMODE mo = EdgeIterator::SAMPLEOCCURENCE) ;
	virtual ~AugmentingPathHeuristic (void) ;

	/**
	 * reset the state of this AugmentingPathHeuristic, esp. the EdgeIterators
	 * \param mne the maximum number of edges that should be considered for every vertex for now on
	 **/
	void reset (UWORD32 mne = UWORD32_MAX, EdgeIterator::ITERATIONMODE mo = EdgeIterator::SAMPLEOCCURENCE) ;

	void run (void) ;

	private:
	/**
	 * \param v0 an exposed vertex
	 * \param path an array of Edge pointers where the path will be put
	 * \return the length of the path (the number of valid edges in path)
	 **/
	unsigned long searchAugmentingPath (Vertex* v0, const Edge** path) ;

	const Edge* getNextEdge (Vertex *v) ;

	void markVisited (Vertex *v)
		{ TimeCounters[v->getLabel()] = TimeCounter ; } ;

	/**
	 * returns true iff v has already been visited in this
	 * iteration, i.e. in the current call of searchAugmentingPath
	 **/
	bool isVisited (Vertex *v) const
		{ return isVisited(v->getLabel()) ; } ;

	bool isVisited (VertexLabel vlbl) const
		{ /* myassert (vlbl < TheGraph->getNumVertices) ;  FIXME DEBUG */ return (TimeCounters[vlbl] == TimeCounter) ; } ;

	UWORD32 TimeCounter ;
	UWORD32* TimeCounters ;
	bool* VertexOnPath ;
	EdgeIterator* EdgeIterators ;
} ;

#endif // ndef SH_AUGMENTINGPATHHEURISTIC
