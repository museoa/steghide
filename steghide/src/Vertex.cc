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
#include "Edge.h"
#include "EdgeIterator.h"
#include "Graph.h"
#include "Vertex.h"

Vertex::Vertex (Graph* g, VertexLabel l, SamplePos* sposs, VertexContent *vc)
	: GraphAccess(g)
{
	setLabel (l) ;
	SamplePositions = sposs ;
	Content = vc ;
	VertexOccurenceIt = Content->addOccurence (this) ;
	SampleOccurenceIts = new std::list<SampleOccurence>::iterator[TheGraph->getSamplesPerVertex()] ;
	ShortestEdge = NULL ;
	valid = true ;
}

Vertex::Vertex (const Vertex& v)
{
	// the copy constructor should never be called because
	// Vertex Occurences in corresponding VertexContent would be inconsistent
	myassert (0) ;
}

Vertex::~Vertex ()
{
	delete[] SampleOccurenceIts ;
	delete[] SamplePositions ;
}

void Vertex::markDeleted ()
{
	printDebug (2, "marking vertex with label %lu as deleted.", getLabel()) ;

	if (valid) {
		// decrement neighbour degrees
		for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
			const std::vector<SampleValue*>& oppneighs = TheGraph->SampleValueOppNeighs[getSampleValue(i)] ;
			for (unsigned long j = 0 ; j < oppneighs.size() ; j++) {
				oppneighs[j]->decNumEdges() ;
			}
		}

		// delete from vertex occurences in vertex content
		VertexOccurenceIt = Content->markDeletedFromOccurences (VertexOccurenceIt) ;

		// delete from sample occurences in graph
		for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
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
		for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
			SampleOccurenceIts[i] = TheGraph->unmarkDeletedSampleOccurence (SampleOccurenceIts[i]) ;
		}

		// undelete into sample occurences in graph
		VertexOccurenceIt = Content->unmarkDeletedFromOccurences (VertexOccurenceIt) ;

		// increment neighbour degrees
		for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
			const std::vector<SampleValue*>& oppneighs = TheGraph->SampleValueOppNeighs[getSampleValue(i)] ;
			for (unsigned long j = 0 ; j < oppneighs.size() ; j++) {
				oppneighs[j]->incNumEdges() ;
			}
		}

		valid = true ;
	}
}

void Vertex::updateShortestEdge ()
{
	if (ShortestEdge != NULL) {
		delete ShortestEdge ;
	}

	if (getDegree() == 0) {
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

	std::cerr << space << "Vertex:" << std::endl ;
	std::cerr << space << " Label: " << getLabel() << std::endl ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		std::cerr << space << " SamplePosition: " << getSamplePos(i) << std::endl ;
		getSampleValue(i)->print (spc + 1) ;
	}
}

void Vertex::printEdges() const
{
	std::cerr << "edges of vertex with label " << getLabel() << std::endl ;
	EdgeIterator edgeit (TheGraph, TheGraph->getVertex(getLabel())) ;
	while (*edgeit != NULL) {
		Edge* e = *edgeit ;
		std::cerr << "  label of other vertex: " << e->getOtherVertex(this)->getLabel() << std::endl ;
		std::cerr << "  weight: " << e->getWeight() << std::endl ;
		++edgeit ;
	}
}
#endif
