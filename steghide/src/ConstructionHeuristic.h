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

#ifndef SH_CONSTRUCTIONHEURISTIC_H
#define SH_CONSTRUCTIONHEURISTIC_H

#include <functional>
#include <queue>
#include <vector>

class Edge ;
class Graph ;
class Matching ;
#include "MatchingAlgorithm.h"
class ProgressOutput ;
class Vertex ;

/**
 * \class ConstructionHeuristic
 * \brief a heuristic algorithm for constructing a matching
 *
 * This class implements a construction heuristic for the maximum matching
 * problem. The idea for the algorithm is taken from Michael Sipser, Richard M. Karp:
 * "Maximum matchings in sparse random graphs", in 22nd Annual Symposium on Foundations
 * of Computer Science. The modification consists of the priority queues, resp. of
 * the fact that the vertices in the priority queues are ordered by the length of their
 * shortest edge. This biases the heuristic to choosing shorter edges on average.
 **/
class ConstructionHeuristic : public MatchingAlgorithm {
	public:
	ConstructionHeuristic (Graph* g, Matching* m, float goal) ;

	~ConstructionHeuristic (void) ;

	void run (void) ;

	private:
	/**
	 * \class LongerShortestEdge
	 * \brief a comparison operator
	 *
	 * lse(v1, v2) is true if the length of the shortest edge of v1
	 * is greater than the length of the shortest edge of v2. If a vertex
	 * has Degree 0, the length of its shortest edge is defined to be +Infinity.
	 * If both v1 and v2 have degree 0, then the vertex with the greater label
	 * is defined to have the "longer shortest edge".
	 **/
	class LongerShortestEdge : public std::binary_function<Vertex*,Vertex*,bool> {
		public:
		bool operator() (const Vertex *v1, const Vertex *v2) ;
	} ;

	static const unsigned int PriorityQueueRange = 1 ;

	/**
	 * get the Vertex from VerticesDeg1 that is k-nearest to top (with updated degrees and shortest edges)
	 **/
	Vertex *findVertexDeg1 (unsigned int k) ;

	/**
	 * get the Vertex from VerticesDegG that is k-nearest to top (with updated degrees and shortest edges)
	 **/
	Vertex *findVertexDegG (unsigned int k) ;

	/**
	 * insert the edge e into the matching m and invalidate the two vertices for the matching algorithm
	 **/
	void insertInMatching (Edge *e) ;

	/**
	 * copy all Neighbours of v that have degree 1 to VerticesDeg1
	 **/
	void checkNeighboursDeg1 (Vertex *v) ;

	/// contains all vertices of degree 1 - every vertex in this queue has a correct shortest edge if it still has degree 1
	std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> VerticesDeg1 ;
	/// contains all vertices with degree greater than 1
	std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> VerticesDegG ;
} ;

#endif // ndef SH_CONSTRUCTIONHEURISTIC_H
