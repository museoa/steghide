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

#ifndef SH_CONSTRHEUR_H
#define SH_CONSTRHEUR_H

#include "graph.h"
#include "matching.h"

class ConstructionHeuristic : private GraphAccess {
	public:
	ConstructionHeuristic (Graph *g) ;

	void run (void) ;

	Matching *getMatching (void)
		{ return TheMatching ; } ;

	private:
	class LongerShortestEdge : public binary_function<Vertex*,Vertex*,bool> {
		public:
		bool operator() (const Vertex *v1, const Vertex *v2) 
			{ return (v1->getShortestEdge()->getWeight() > v2->getShortestEdge()->getWeight()) ; } ;
	} ;

	static const unsigned int PriorityQueueRange = 1 ;

	Matching *TheMatching ;

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

	/// contains all vertices of degree 1
	priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> VerticesDeg1 ;
	/// contains all vertices with degree greater than 1
	priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> VerticesDegG ;
} ;

#endif // ndef SH_CONSTRHEUR_H
