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

AugmentingPathHeuristic::AugmentingPathHeuristic (Graph* g, Matching* m, float goal, UWORD32 mne, EdgeIterator::ITERATIONMODE mo)
	: MatchingAlgorithm (g, m, goal)
{
	unsigned long numvertices = g->getNumVertices() ;
	VertexOnPath = new bool[numvertices] ;

	TimeCounter = 0 ;
	TimeCounters = new UWORD32[numvertices] ;

	EdgeIterators = new EdgeIterator[numvertices] ;
	for (VertexLabel l = 0 ; l < numvertices ; l++) {
		EdgeIterators[l] = EdgeIterator (g->getVertex(l), mo) ;
	}
	EdgeIterator::setMaxNumEdges (mne) ;
}

AugmentingPathHeuristic::~AugmentingPathHeuristic ()
{
	delete[] EdgeIterators ;
	delete[] TimeCounters ;
	delete[] VertexOnPath ;
}

void AugmentingPathHeuristic::reset (UWORD32 mne, EdgeIterator::ITERATIONMODE mo)
{
	EdgeIterator::setMaxNumEdges (mne) ;
	unsigned long numvertices = TheGraph->getNumVertices() ;
	TimeCounter = 0 ;
	for (VertexLabel l = 0 ; l < numvertices ; l++) {
		VertexOnPath[l] = false ;
		TimeCounters[l] = 0 ;
		EdgeIterators[l].reset(mo) ;
	}
}

void AugmentingPathHeuristic::run ()
{
	const Edge** path = new const Edge*[TheGraph->getNumVertices()] ;

	// FIXME - ? as reference
	const std::list<Vertex*> ExposedVertices = TheMatching->getExposedVertices() ;
	for (std::list<Vertex*>::const_iterator expv = ExposedVertices.begin() ;
		(expv != ExposedVertices.end()) && (TheMatching->getCardinality() < CardinalityGoal) ; expv++) {
		if (TheMatching->isExposed (*expv)) {
			unsigned long pathlength = searchAugmentingPath (*expv, path) ;

#ifdef DEBUG
			if (pathlength == 0) {
				printDebug (5, "AugmentingPathHeuristic: could not find augmenting path for vertex %lu", (*expv)->getLabel()) ;
			}
			else {
				if (RUNDEBUGLEVEL(5)) {
					std::cerr << "AugmentingPathHeuristic: found augmenting path for vertex " << (*expv)->getLabel() << ": " ;
					for (unsigned long i = 0 ; i < pathlength ; i++) {
						std::cerr << path[i]->getVertex1()->getLabel() << "-" << path[i]->getVertex2()->getLabel() ;
						if (i != pathlength - 1) {
							std::cerr << ", " ;
						}
					}
					std::cerr << std::endl ;
				}
			}
#endif
			if (pathlength > 0) {
				TheMatching->augment ((const Edge**) path, pathlength) ;
			}
		}
	}

	delete path ;
}

#ifdef DEBUG
#define pushOnPath(EDGE) \
printDebug (6, "AugmentingPathHeuristic: pushing edge on path: %lu - %lu", EDGE->getVertex1()->getLabel(), EDGE->getVertex2()->getLabel()) ; \
path[pathlen] = EDGE ; \
pathlen++ ; \
VertexOnPath[EDGE->getVertex1()->getLabel()] = true ; \
VertexOnPath[EDGE->getVertex2()->getLabel()] = true ;
#else
#define pushOnPath(EDGE) \
path[pathlen] = EDGE ; \
pathlen++ ; \
VertexOnPath[EDGE->getVertex1()->getLabel()] = true ; \
VertexOnPath[EDGE->getVertex2()->getLabel()] = true ;
#endif

unsigned long AugmentingPathHeuristic::searchAugmentingPath (Vertex *v0, const Edge** path)
{
#ifdef DEBUG
	printDebug (5, "AugmentingPathHeuristic: searching augmenting path for vertex with label %lu", v0->getLabel()) ;
#endif

	TimeCounter++ ;
	unsigned long pathlen = 0 ;
	const Edge* e = NULL ;

	while ((e = getNextEdge(v0)) != NULL) {
		pushOnPath(e) ;
		Vertex *w = e->getOtherVertex (v0) ;

		if (TheMatching->isExposed(w)) {
			return pathlen ;
		}
		// add matched edge
		markVisited (w) ;
		e = TheMatching->getMatchingEdge (w) ; // w is matched (because not exposed)
		Vertex *w_next = e->getOtherVertex (w) ;
		pushOnPath(e) ;

		while (pathlen > 0) {
			const Edge* e_next = getNextEdge (w_next) ;
			if (e_next != NULL) { // found next edge
				pushOnPath(e_next) ;
				w = e_next->getOtherVertex (w_next) ;

				if (TheMatching->isExposed(w)) {
					return pathlen ;
				}
				// add matched edge
				markVisited (w) ;
				e = TheMatching->getMatchingEdge (w) ; // w is matched (because not exposed)
				w_next = e->getOtherVertex (w) ;
				pushOnPath(e) ;
			}
			else { // could not find next edge
#ifdef DEBUG
				printDebug (6, "AugmentingPathHeuristic: could not find next edge from vertex with label %lu", w_next->getLabel()) ;
				printDebug (6, "AugmentingPathHeuristic: popping edge %lu - %lu from path", path[pathlen - 1]->getVertex1()->getLabel(), path[pathlen - 1]->getVertex2()->getLabel()) ;
				printDebug (6, "AugmentingPathHeuristic: popping edge %lu - %lu from path", path[pathlen - 2]->getVertex1()->getLabel(), path[pathlen - 2]->getVertex2()->getLabel()) ;
#endif

				VertexOnPath[e->getVertex1()->getLabel()] = false ;
				VertexOnPath[e->getVertex2()->getLabel()] = false ;
				
				// matched edge: pop from path
				myassert (path[pathlen - 1] == e) ;
				pathlen-- ;

				// unmatched edge: pop from path and delete (has been created only for path)
#if 0
				Edge *delme = path->back() ;
				myassert (!TheMatching->includesEdge(delme)) ;
				path->pop_back() ;
				delete delme ;
#endif
				myassert (!TheMatching->includesEdge(path[pathlen - 1])) ;
				pathlen-- ;

				// set w,e,w_next to complete backtracking step
				if (pathlen > 0) {
					e = path[pathlen - 1] ;
					const Edge* before_e = path[pathlen - 2] ;
					if (before_e->contains (e->getVertex1())) {
						w = e->getVertex1() ;
						w_next = e->getVertex2() ;
					}
					else if (before_e->contains (e->getVertex2())) {
						w = e->getVertex2() ;
						w_next = e->getVertex1() ;
					}
					else {
						myassert(false) ;
					}
				}
			}
		}
	}

	return pathlen ;
}

const Edge* AugmentingPathHeuristic::getNextEdge (Vertex *v)
{
	if (isVisited(v)) {
		std::cerr << "incrementing edgeit of visited vertex" << std::endl ;
		EdgeIterators[v->getLabel()].print(1) ;
		++(EdgeIterators[v->getLabel()]) ;
	}
	else {
		EdgeIterators[v->getLabel()].reset() ;
		markVisited(v) ;
	}

	std::cerr << "entering main search loop" << std::endl ;
	const Edge* e = NULL ;
	bool found = false ;
	do {
		if (EdgeIterators[v->getLabel()].isFinished()) {
			// no more unexamined edges for this vertex
#ifdef DEBUG
			printDebug (7, "AugmentingPathHeuristic::getNextEdge: no more unexamined edges for vertex %lu", v->getLabel()) ;
#endif
			found = true ;
		}
		else {
			VertexLabel pvlbl = EdgeIterators[v->getLabel()].getPartnerVertexLabel() ;
			if (!(VertexOnPath[pvlbl] && isVisited(pvlbl))) { // edge is admissible
				e = *EdgeIterators[v->getLabel()] ;
				found = true ;
#ifdef DEBUG
				printDebug (7, "AugmentingPathHeuristic::getNextEdge: admissible edge for vertex %lu goes to vertex %lu", v->getLabel(), pvlbl) ;
#endif
			}
			else {
				++(EdgeIterators[v->getLabel()]) ;
			}
		}
	} while (!found) ;
	return e ;
}
