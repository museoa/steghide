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
	printDebug (3, "searching augmenting path for vertex with label %lu", v0->getLabel()) ;

	TimeCounter++ ;
	bool addMatchedEdge = true ;
	std::vector<Edge*>* path = new std::vector<Edge*>() ;
	Edge *e = NULL ;

	while ((e = getNextEdge(v0)) != NULL) {
		pushOnPath (path, e) ;
		myassert (path->size() == 1) ;
		Vertex *w = e->getOtherVertex (v0) ;

		if (TheMatching->isExposed(w)) {
			return path ;
		}
		// add matched edge
		markVisited (w) ;
		e = TheMatching->getMatchingEdge (w) ; // w is matched (because not exposed)
		Vertex *w_next = e->getOtherVertex (w) ;
		pushOnPath (path, e) ;
		myassert (path->size() % 2 == 0) ;

		while (!path->empty()) {
			Edge* e_next = getNextEdge (w_next) ;
			if (e_next != NULL) { // found next edge
				pushOnPath (path, e_next) ;
				myassert (path->size() % 2 == 1) ;
				w = e_next->getOtherVertex (w_next) ;

				if (TheMatching->isExposed(w)) {
					return path ;
				}
				// add matched edge
				markVisited (w) ;
				e = TheMatching->getMatchingEdge (w) ; // w is matched (because not exposed)
				w_next = e->getOtherVertex (w) ;
				pushOnPath (path, e) ;
				myassert (path->size() % 2 == 0) ;
			}
			else { // could not find next edge
				printDebug (4, "could not find next edge from vertex with label %lu", w_next->getLabel()) ;

				VertexOnPath[e->getVertex1()->getLabel()] = false ;
				VertexOnPath[e->getVertex2()->getLabel()] = false ;
				
				// matched edge: pop from path
				myassert (path->back() == e) ;
				path->pop_back() ;

				// unmatched edge: pop from path and delete (has been created only for path)
				Edge *delme = path->back() ;
				if (TheMatching->includesEdge(delme)) {
					std::cerr << "I'm about to delete the following matched edge:" << std::endl ;
					delme->print(1) ;
				}
				myassert (!TheMatching->includesEdge(delme)) ;
				path->pop_back() ;
				delete delme ;

				// set w,e,w_next to complete backtracking step
				if (!path->empty()) {
					e = path->back() ;
					Edge* before_e = (*path)[path->size() - 2] ;
					if (before_e->contains (e->getVertex1())) {
						w = e->getVertex1() ;
						w_next = e->getVertex2() ;
					}
					else if (before_e->contains (e->getVertex2())) {
						w = e->getVertex2() ;
						w_next = e->getVertex1() ;
					}
					else {
						myassert(0) ;
					}
				}
			}
		}
	}

	return path ;
}

void AugmentingPathHeuristic::pushOnPath (std::vector<Edge*>* path, Edge* e)
{
	printDebug (4, "pushing edge on path: %lu - %lu", e->getVertex1()->getLabel(), e->getVertex2()->getLabel()) ;
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
		if (EdgeIterators[v->getLabel()].isFinished()) {
			// no more unexamined edges for this vertex
			found = true ;
		}
		else {
			VertexLabel pvlbl = EdgeIterators[v->getLabel()].getPartnerVertexLabel() ;
			if (!(isVisited (pvlbl) && VertexOnPath[pvlbl])) {
				// edge is admissible
				e = *EdgeIterators[v->getLabel()] ;
				found = true ;
			}
			++(EdgeIterators[v->getLabel()]) ;
		}
	} while (!found) ;
	return e ;
}
