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

#include "constrheur.h"
#include "randomsource.h"

// FIXME - wie werden nach Einfügeoperation alle Knoten mit Grad 1 erkannt ?

ConstructionHeuristic::ConstructionHeuristic (Graph *g)
	: GraphAccess(g)
{
	unsigned long nvertices = g->getNumVertices() ;

	// fill the VerticesDeg1 and VerticesDegG priority queues
	VerticesDeg1 = priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> () ;
	VerticesDegG = priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> () ;

	for (VertexLabel l = 0 ; l < nvertices ; l++) {	
		Vertex *v = g->getVertex(l) ;
		v->updateShortestEdge() ;
		if (v->getDegree() != 0) {
			if (v->getDegree() == 1) {
				assert (v->getShortestEdge() != NULL) ;
				VerticesDeg1.push (v) ;
			}
			else {
				assert (v->getShortestEdge() != NULL) ;
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
	for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
		const vector<SampleValue*> oppneighs = TheGraph->getOppNeighs(v->getSampleValue(i)) ;
		for (vector<SampleValue*>::const_iterator it = oppneighs.begin() ; it != oppneighs.end() ; it++) {
			const list<VertexContent*> vcontents = TheGraph->getVertexContents(*it) ;
			for (list<VertexContent*>::const_iterator jt = vcontents.begin() ; jt != vcontents.end() ; jt++) {
				if ((*jt)->getDegree() == 1) {
					list<Vertex*> occ = (*jt)->getOccurences() ;
					for (list<Vertex*>::iterator kt = occ.begin() ; kt != occ.end() ; kt++) {
						VerticesDeg1.push (*kt) ;
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
	vector<Vertex*> topk ;

	// get the vertex that is the k-nearest to top (with updated len of shortest edge) and has degree > 1
	do {
		assert (!VerticesDegG.empty()) ;
		v = VerticesDegG.top() ;
		VerticesDegG.pop() ;

		if ((v->getDegree() > 1) && TheMatching->isExposed(v)) { // implicitly delete vertices that have been moved to VerticesDeg1 or have already been matched
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
					assert (topk.size() < k - 1) ;
					topk.push_back (v) ;
				}
			}
			else {
				assert (v->getShortestEdge()->getWeight() > weight_before) ;	// weight can only rise
				VerticesDegG.push (v) ; // push v into a position that is further away from top
			}
		}

		if (k != 1 && VerticesDegG.empty()) { // there were less than k valid vertices in VerticesDegree1
			assert (!usethisvertex) ;
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
	vector<Vertex*> topk ;
	bool usethisvertex = false ;

	// get the vertex that is the k-nearest to top and still has degree 1
	do {
		assert (!VerticesDeg1.empty()) ;
		v = VerticesDeg1.top() ;
		VerticesDeg1.pop() ;
		assert (v->getDegree() <= 1) ;

		if ((v->getDegree() == 1) && TheMatching->isExposed(v)) { // implicitly delete vertices that have degree zero or have already been matched
#if 0
			v->updateShortestEdge() ;
#endif
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
			assert (!usethisvertex) ;
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
