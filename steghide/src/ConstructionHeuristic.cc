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

#include "ConstructionHeuristic.h"
#include "RandomSource.h"

ConstructionHeuristic::ConstructionHeuristic (Graph* g)
	: GraphAccess(g)
{
	unsigned long nvertices = g->getNumVertices() ;

	// fill the VerticesDeg1 and VerticesDegG priority queues
	VerticesDeg1 = std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> () ;
	VerticesDegG = std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> () ;

	for (VertexLabel l = 0 ; l < nvertices ; l++) {	
		Vertex *v = g->getVertex(l) ;
		v->updateShortestEdge() ;
		if (v->getDegree() != 0) {
			if (v->getDegree() == 1) {
				myassert (v->getShortestEdge() != NULL) ;
				VerticesDeg1.push (v) ;
			}
			else {
				myassert (v->getShortestEdge() != NULL) ;
				VerticesDegG.push (v) ;
			}
		}
	}

	TheMatching = new Matching (g) ;
}

void ConstructionHeuristic::run ()
{
	unsigned int pqr = PriorityQueueRange ;
#ifdef DEBUG
	if (Args.PriorityQueueRange.is_set()) {
		pqr = Args.PriorityQueueRange.getValue() ;
	}
#endif

	while (!(VerticesDegG.empty() && VerticesDeg1.empty())) {
		Vertex *v = NULL ;
		unsigned int k = 1 ;
		if (pqr > 1) {
			k = RndSrc.getValue (pqr) + 1 ;
		}

		if (!VerticesDeg1.empty()) {
			v = findVertexDeg1 (k) ;
		}
		else {
			v = findVertexDegG (k) ;
		}

		if (v != NULL) {
			insertInMatching (v->getShortestEdge()) ;
		}
	}
}

void ConstructionHeuristic::insertInMatching (Edge *e)
{
	Vertex *v1 = e->getVertex1() ;
	Vertex *v2 = e->getVertex2() ;

	printDebug (2, "inserting vertices %lu and %lu in matching", v1->getLabel(), v2->getLabel()) ;
	TheMatching->addEdge (e) ;

	v1->markDeleted() ;
	v2->markDeleted() ;
	checkNeighboursDeg1 (v1) ;
	checkNeighboursDeg1 (v2) ;
}

void ConstructionHeuristic::checkNeighboursDeg1 (Vertex *v)
{
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		const std::vector<SampleValue*>& oppneighs = TheGraph->SampleValueOppNeighs[v->getSampleValue(i)] ;
		for (std::vector<SampleValue*>::const_iterator it = oppneighs.begin() ; it != oppneighs.end() ; it++) {
			const std::list<VertexContent*> vcontents = TheGraph->VertexContents[(*it)->getLabel()] ;
			for (std::list<VertexContent*>::const_iterator jt = vcontents.begin() ; jt != vcontents.end() ; jt++) {
				if ((*jt)->hasOccurences()) {
					if ((*jt)->getDegree() == 1) {
						std::list<Vertex*> occ = (*jt)->getOccurences() ;
						for (std::list<Vertex*>::iterator kt = occ.begin() ; kt != occ.end() ; kt++) {
							(*kt)->updateShortestEdge() ;
							VerticesDeg1.push (*kt) ;
						}
					}
				}
			}
		}
	}
}

Vertex* ConstructionHeuristic::findVertexDegG (unsigned int k)
{
	Vertex *v = NULL ;
	bool usethisvertex = false ;
	// buffer for the top k-1 vertices in the priority queue
	std::vector<Vertex*> topk ;

	// get the vertex that is the k-nearest to top (with updated len of shortest edge) and has degree > 1
	do {
		myassert (!VerticesDegG.empty()) ;
		v = VerticesDegG.top() ;
		VerticesDegG.pop() ;

		if ((TheMatching->isExposed(v)) && (v->getDegree() > 1)) { // implicitly delete vertices that have been moved to VerticesDeg1 or have already been matched
			float weight_before = v->getShortestEdge()->getWeight() ;
			
			v->updateShortestEdge() ;

			if (v->getShortestEdge()->getWeight() == weight_before) {
				if (topk.size() == k - 1) {
					for (unsigned int i = 0 ; i < k - 1 ; i++) {
						VerticesDegG.push (topk[i]) ;
					}
					usethisvertex = true ;
				}
				else {
					myassert (topk.size() < k - 1) ;
					topk.push_back (v) ;
				}
			}
			else {
				myassert (v->getShortestEdge()->getWeight() > weight_before) ;	// weight can only rise
				VerticesDegG.push (v) ; // push v into a position that is further away from top
			}
		}

		if (k != 1 && VerticesDegG.empty()) { // there were less than k valid vertices in VerticesDegree1
			myassert (!usethisvertex) ;
			unsigned int nfound = topk.size() ;
			if (nfound > 0) {
				usethisvertex = true ;
				v = topk[nfound - 1] ;
				for (unsigned int i = 0 ; i < nfound - 1 ; i++) {
					VerticesDegG.push (topk[i]) ;
				}
			}
		}
	} while (!(usethisvertex || VerticesDegG.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

Vertex *ConstructionHeuristic::findVertexDeg1 (unsigned int k)
{
	Vertex *v = NULL ;
	std::vector<Vertex*> topk ;
	bool usethisvertex = false ;

	// get the vertex that is the k-nearest to top and still has degree 1
	do {
		myassert (!VerticesDeg1.empty()) ;
		v = VerticesDeg1.top() ;
		VerticesDeg1.pop() ;

		if ((TheMatching->isExposed(v)) && (v->getDegree() == 1)) { // implicitly delete vertices that have degree zero or have already been matched
			if (topk.size() == k - 1) {
				for (unsigned int i = 0 ; i < k - 1 ; i++) {
					VerticesDeg1.push (topk[i]) ;
				}
				usethisvertex = true ;	// v is the vertex k-nearest to top with degree 1
			}
			else {
				topk.push_back (v) ;
			}
		}

		if (k != 1 && VerticesDeg1.empty()) {	// there were less than k vertices with degree 1 in VerticesDegree1
			myassert (!usethisvertex) ;
			unsigned int nfound = topk.size() ;
			if (nfound > 0) {
				usethisvertex = true ;
				v = topk[nfound - 1] ;
				for (unsigned int i = 0 ; i < nfound - 1 ; i++) {
					VerticesDeg1.push (topk[i]) ;
				}
			}
		}
	} while (!(usethisvertex || VerticesDeg1.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

bool ConstructionHeuristic::LongerShortestEdge::operator() (const Vertex* v1, const Vertex* v2)
{
	bool retval = false ;
	if (v1->getDegree() == 0 && v2->getDegree() == 0) {
		retval = (v1->getLabel() > v2->getLabel()) ;
	}
	else if (v1->getDegree() == 0) {
		retval = true ;
	}
	else if (v2->getDegree() == 0) {
		retval = false ;
	}
	else {
		retval = v1->getShortestEdge()->getWeight() > v2->getShortestEdge()->getWeight();
	}
	return retval ;
}
