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

#include "common.h"
#include "edgeiterator.h"
#include "vertex.h"

Vertex::Vertex (Graph* g, VertexLabel l, SamplePos* sposs, VertexContent *vc)
	: GraphAccess(g)
{
	setLabel (l) ;
	SamplePositions = sposs ;
	Content = vc ;
	VertexOccurenceIt = Content->addOccurence (this) ;
	SampleOccurenceIts = new list<SampleOccurence>::iterator[SamplesPerVertex] ;
	ShortestEdge = NULL ;
	valid = true ;
}

Vertex::~Vertex ()
{
	// TODO delete[] SampleOccurenceIts ;
}

void Vertex::markDeleted ()
{
	printDebug (2, "marking vertex with label %lu as deleted.", getLabel()) ;

	if (valid) {
		// decrement neighbour degrees
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			const vector<SampleValue*> oppneighs = TheGraph->getOppNeighs(getSampleValue(i)) ;
			for (unsigned long j = 0 ; j < oppneighs.size() ; j++) {
				oppneighs[j]->decNumEdges() ;
			}
		}

		// delete from vertex occurences in vertex content
		VertexOccurenceIt = Content->markDeletedFromOccurences (VertexOccurenceIt) ;

		// delete from sample occurences in graph
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			SampleOccurenceIts[i] = TheGraph->markDeletedSampleOccurence (SampleOccurenceIts[i]) ;
		}
		valid = false ;
	}
}

void Vertex::unmarkDeleted ()
{
	printDebug (2, "unmarking deletion of vertex with label %lu.", getLabel()) ;

	if (!valid) {
		// undelete into sample occurences in graph
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			SampleOccurenceIts[i] = TheGraph->unmarkDeletedSampleOccurence (SampleOccurenceIts[i]) ;
		}

		// undelete into sample occurences in graph
		VertexOccurenceIt = Content->unmarkDeletedFromOccurences (VertexOccurenceIt) ;

		// increment neighbour degrees
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			const vector<SampleValue*> oppneighs = TheGraph->getOppNeighs(getSampleValue(i)) ;
			for (unsigned long j = 0 ; j < oppneighs.size() ; j++) {
				oppneighs[j]->incNumEdges() ;
			}
		}

		valid = true ;
	}
}

void Vertex::updateShortestEdge ()
{
	if (getDegree() == 0) {
		if (ShortestEdge != NULL) {
			delete ShortestEdge ;
		}
		ShortestEdge = NULL ;
	}
	else {
		EdgeIterator edgeit (TheGraph, this) ;
		ShortestEdge = *edgeit ;
	}
}

#ifdef DEBUG
void Vertex::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	cerr << space << "Vertex:" << endl ;
	cerr << space << " Label: " << getLabel() << endl ;
	for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
		cerr << space << " SamplePosition: " << getSamplePos(i) << endl ;
		getSampleValue(i)->print (spc + 1) ;
	}
}

void Vertex::printEdges() const
{
	cerr << "edges of vertex with label " << getLabel() << endl ;
	EdgeIterator edgeit (TheGraph, TheGraph->getVertex(getLabel())) ;
	while (*edgeit != NULL) {
		Edge* e = *edgeit ;
		cerr << "  label of other vertex: " << e->getOtherVertex(this)->getLabel() << endl ;
		cerr << "  weight: " << e->getWeight() << endl ;
		++edgeit ;
	}
}
#endif
