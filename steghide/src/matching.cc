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

#include "matching.h"

Matching::Matching (Graph *g)
	: GraphAccess(g)
{
	unsigned long nvertices = TheGraph->getNumVertices() ;
	for (unsigned long i = 0 ; i < nvertices ; i++) {
		ExposedVertices.push_back (TheGraph->getVertex (i)) ;
	}

	VertexInformation.reserve (nvertices) ;
	for (list<Vertex*>::iterator i = ExposedVertices.begin() ; i != ExposedVertices.end() ; i++) {
		VertexInformation.push_back (VertexInfo (i)) ;
	}

	Cardinality = 0 ;
}

Matching& Matching::addEdge (Edge *e)
{
	VertexLabel vlbl1 = e->getVertex1()->getLabel() ;
	VertexLabel vlbl2 = e->getVertex2()->getLabel() ;

	assert (VertexInformation[vlbl1].isExposed()) ;
	assert (VertexInformation[vlbl2].isExposed()) ;
	list<Edge*>::iterator edgeit = MatchingEdges.insert (MatchingEdges.end(), e) ;

	ExposedVertices.erase (VertexInformation[vlbl1].getExposedIterator()) ;
	ExposedVertices.erase (VertexInformation[vlbl2].getExposedIterator()) ;
	VertexInformation[vlbl1].setMatched (edgeit) ;
	VertexInformation[vlbl2].setMatched (edgeit) ;

	return *this ;
}

Matching& Matching::augment (const vector<Edge*> &path)
{
	// TODO - test if this vector of edges really is a valid path ?
	assert (path.size() & 2 == 1) ;
	for (unsigned int i = 0 ; i < path.size() ; i += 2) {
		Edge* e_add = path[i] ;
		list<Edge*>::iterator edgeit = MatchingEdges.insert (MatchingEdges.end(), e_add) ;
		VertexInformation[e_add->getVertex1()->getLabel()].setMatched (edgeit) ;
		VertexInformation[e_add->getVertex2()->getLabel()].setMatched (edgeit) ;
	}
	return *this ;
}

#ifdef DEBUG
bool Matching::check () const
{
	cerr << "checking Matching" << endl ;
	bool retval = true ;
	retval = check_MatchingEdges_vs_VertexInformation() && retval ;
	retval = check_ExposedVertices_vs_VertexInformation() && retval ;
	return retval ;
}

bool Matching::check_MatchingEdges_vs_VertexInformation () const
{
	bool err = false ;
	// for every e = (v1,v2) in MatchingEdges: isMatched(v1) && isMatched(v2)
	for (list<Edge*>::const_iterator it = MatchingEdges.begin() ; it != MatchingEdges.end() ; it++) {
		Vertex *v1 = (*it)->getVertex1() ;
		Vertex *v2 = (*it)->getVertex2() ;
		if (VertexInformation[v1->getLabel()].isExposed() || VertexInformation[v2->getLabel()].isExposed()) {
			err = true ;
			break ;
		}
	}

	if (err) {
		cerr << "FAILED: There is an edge in MatchingEdges that is adjacent to a vertex marked as exposed." << endl ;
	}

	return !err ;
}

bool Matching::check_ExposedVertices_vs_VertexInformation () const
{
	bool err = false ;

	// for every exposed vertex v: isExposed(v)
	for (list<Vertex*>::const_iterator it = ExposedVertices.begin() ; it != ExposedVertices.end() ; it++) {
		if (VertexInformation[(*it)->getLabel()].isMatched()) {
			err = true ;
			break ;
		}
	}

	if (err) {
		cerr << "FAILED: There is a vertex in ExposedVertices that is marked matched." << endl ;
	}

	return !err ;
}

#endif
