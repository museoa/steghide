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

#include "augheur.h"
#include "common.h"

AugmentingPathHeuristic::AugmentingPathHeuristic (Graph *g, Matching *m)
	: GraphAccess(g)
{
	TheMatching = m ;

	unsigned long numvertices = g->getNumVertices() ;
	VertexOnPath = vector<bool> (numvertices, false) ;

	TimeCounter = 0 ;
	TimeCounters = vector<UWORD32> (numvertices, 0) ;

	// FIXME - eigentlich hier kein init n�tig, wird in getNextEdge dann ohnehin gemacht
	EdgeIterators.reserve (numvertices) ;
	for (VertexLabel l = 0 ; l < numvertices ; l++) {
		EdgeIterators.push_back (EdgeIterator (g, g->getVertex(l))) ;
	}
}

void AugmentingPathHeuristic::run ()
{
	const list<Vertex*> ExposedVertices = TheMatching->getExposedVertices() ;
	for (list<Vertex*>::const_iterator expv = ExposedVertices.begin() ; expv != ExposedVertices.end() ; expv++) {
		if (TheMatching->isExposed (*expv)) {
			vector<Edge*>* path = searchAugmentingPath (*expv) ;
			if (!path->empty()) {
				TheMatching->augment (*path) ;
				delete path ;
			}
		}
	}
}

vector<Edge*>* AugmentingPathHeuristic::searchAugmentingPath (Vertex *v0)
{
	TimeCounter++ ;

	vector<Edge*>* path = new vector<Edge*>() ;
	Edge *e = NULL ;

	markVisited (v0) ;
	VertexOnPath[v0->getLabel()] = true ;

	while ((e = getNextEdge(v0)) != NULL) {
		path->push_back (e) ;

		Vertex *w = e->getOtherVertex (v0) ;
		markVisited (w) ;
		VertexOnPath[w->getLabel()] = true ;

		while (!path->empty()) {
			if (TheMatching->isExposed(w)) {
				return path ;
			}

			// w is matched
			e = TheMatching->getMatchingEdge (w) ;
			path->push_back (e) ;
			Vertex *w_next = e->getOtherVertex (w) ;
			markVisited (w_next) ;
			VertexOnPath[w_next->getLabel()] = true ;

			Edge* e_next = getNextEdge (w_next) ;
			if (e_next != NULL) {
				path->push_back (e_next) ;
				w = e_next->getOtherVertex (w_next) ;
				markVisited (w) ;
				VertexOnPath[w->getLabel()] ;
			}
			else {
				path->pop_back() ;
				path->pop_back() ;
				VertexOnPath[e->getVertex1()->getLabel()] = false ;
				VertexOnPath[e->getVertex2()->getLabel()] = false ;
			}
		}
	}

	return path ;
}

Edge *AugmentingPathHeuristic::getNextEdge (Vertex *v)
{
	if (!isVisited(v)) {
		EdgeIterators[v->getLabel()].reset() ;
	}

	Edge *e = NULL ;
	bool found = false ;
	do {
		e = *EdgeIterators[v->getLabel()] ;
		++EdgeIterators[v->getLabel()] ;

		if (e == NULL) {
			// no more unexamined edges for this vertex
			found = true ;
			e = NULL ;
		}
		else if (isVisited (e->getOtherVertex (v)) && VertexOnPath[e->getOtherVertex(v)->getLabel()]) {
			delete e ;
		}
		else {
			// edge is admissible
			found = true ;
		}
	} while (!found) ;
	return e ;
}
