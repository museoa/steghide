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

#include "AugmentingPathHeuristic.h"
#include "Edge.h"
#include "EdgeIterator.h"
#include "Graph.h"
#include "Matching.h"
#include "common.h"

AugmentingPathHeuristic::AugmentingPathHeuristic (Graph* g, Matching* m)
	: GraphAccess(g)
{
	TheMatching = m ;

	unsigned long numvertices = g->getNumVertices() ;
	VertexOnPath = std::vector<bool> (numvertices, false) ;

	TimeCounter = 0 ;
	TimeCounters = std::vector<UWORD32> (numvertices, 0) ;

	EdgeIterators.reserve (numvertices) ;
	for (VertexLabel l = 0 ; l < numvertices ; l++) {
		EdgeIterators.push_back (EdgeIterator (g, g->getVertex(l))) ;
	}
}

void AugmentingPathHeuristic::run ()
{
	// FIXME - ? as reference
	const std::list<Vertex*> ExposedVertices = TheMatching->getExposedVertices() ;
	for (std::list<Vertex*>::const_iterator expv = ExposedVertices.begin() ; expv != ExposedVertices.end() ; expv++) {
		if (TheMatching->isExposed (*expv)) {
			std::vector<Edge*>* path = searchAugmentingPath (*expv) ;
			if (!path->empty()) {
				TheMatching->augment (*path) ;
			}
			delete path ;
		}
	}
}

std::vector<Edge*>* AugmentingPathHeuristic::searchAugmentingPath (Vertex *v0)
{
	TimeCounter++ ;

	std::vector<Edge*>* path = new std::vector<Edge*>() ;
	Edge *e = NULL ;

	while ((e = getNextEdge(v0)) != NULL) {
		pushOnPath (path, e) ;
		Vertex *w = e->getOtherVertex (v0) ;

		while (!path->empty()) {
			if (TheMatching->isExposed(w)) {
				return path ;
			}

			// w is matched
			markVisited (w) ;
			e = TheMatching->getMatchingEdge (w) ;
			Vertex *w_next = e->getOtherVertex (w) ;
			pushOnPath (path, e) ;

			Edge* e_next = getNextEdge (w_next) ;
			if (e_next != NULL) {
				pushOnPath (path, e_next) ;
				w = e_next->getOtherVertex (w_next) ;
			}
			else {
				VertexOnPath[e->getVertex1()->getLabel()] = false ;
				VertexOnPath[e->getVertex2()->getLabel()] = false ;
				
				// matched edge: pop from path
				myassert (path->back() == e) ;
				path->pop_back() ;

				// unmatched edge: pop from path and delete (has been created only for path)
				Edge *delme = path->back() ;
				path->pop_back() ;
				// FIXME do this - delete delme ;
			}
		}
	}

	return path ;
}

void AugmentingPathHeuristic::pushOnPath (std::vector<Edge*>* path, Edge* e)
{
	path->push_back (e) ;
	VertexOnPath[e->getVertex1()->getLabel()] = true ;
	VertexOnPath[e->getVertex2()->getLabel()] = true ;
}

Edge *AugmentingPathHeuristic::getNextEdge (Vertex *v)
{
	if (!isVisited(v)) {
		EdgeIterators[v->getLabel()].reset() ;
		markVisited(v) ;
	}

	Edge *e = NULL ;
	bool found = false ;
	do {
		e = *EdgeIterators[v->getLabel()] ;
		if (e == NULL) {
			// no more unexamined edges for this vertex
			found = true ;
		}
		else {
			if (isVisited (e->getOtherVertex (v)) && VertexOnPath[e->getOtherVertex(v)->getLabel()]) {
				delete e ;
			}
			else {
				// edge is admissible
				found = true ;
			}
			++(EdgeIterators[v->getLabel()]) ;
		}
	} while (!found) ;
	return e ;
}
