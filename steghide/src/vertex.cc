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
#include "vertex.h"

// DEBUG
Vertex::Vertex (Graph* g, VertexLabel l, SamplePos* sposs, VertexContent *vc)
	: GraphAccess(g)
{
	setLabel (l) ;
	SamplePositions = sposs ;
	Content = vc ;
	assert (Content > (VertexContent*) 0xff) ;
#if 0
	VertexOccurenceIt = Content->addOccurence (this) ;
#endif
	ShortestEdge = NULL ;
	valid = true ;
}

void Vertex::markDeleted ()
{
	if (valid) {
		// decrement neighbour degrees
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			const vector<SampleValue*> oppneighs = TheGraph->getOppNeighs(getSampleValue(i)) ;
			for (unsigned long j = 0 ; j < oppneighs.size() ; j++) {
				oppneighs[j]->decNumEdges() ;
			}
		}

#if 0
		// delete from vertex occurences in vertex content
		VertexOccurenceIt = Content->markDeletedFromOccurences (VertexOccurenceIt) ;
#endif

		// delete from sample occurences in graph
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			SampleOccurenceIt[i] = TheGraph->markDeletedSampleOccurence (SampleOccurenceIt[i]) ;
		}
		valid = false ;
	}
}

void Vertex::unmarkDeleted ()
{
	if (!valid) {
		// undelete into sample occurences in graph
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			SampleOccurenceIt[i] = TheGraph->unmarkDeletedSampleOccurence (SampleOccurenceIt[i]) ;
		}

#if 0
		// undelete into sample occurences in graph
		VertexOccurenceIt = Content->unmarkDeletedFromOccurences (VertexOccurenceIt) ;
#endif

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
		for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
			SampleValue* srcsv = getSampleValue(i) ;
			for (unsigned long j = 0 ; j < TheGraph->getNumOppNeighs(srcsv) ; j++) {
				SampleValue *destsv = TheGraph->getOppNeighs(srcsv)[j] ;
				if ((ShortestEdge == NULL) || (ShortestEdge->getWeight() > srcsv->calcDistance(destsv))) {
					if (TheGraph->getNumSampleOccurences(destsv) > 0) {
						SampleOccurence occ = *(TheGraph->getSampleOccurences(destsv).begin()) ;
						if (ShortestEdge != NULL) {
							delete ShortestEdge ;
						}
						ShortestEdge = new Edge (this, i, occ.getVertex(), occ.getIndex()) ;
					}
				}
			}
		}
	}
}

#ifdef DEBUG
void Vertex::print() const
{
	cerr << "vertex with label " << getLabel() << endl ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		cerr << "  SampleLabel[" << i << "] is " << getSampleValue(i)->getLabel() << endl ;
		cerr << "  SamplePositon[" << i << "] is " << getSamplePos(i) << endl ;
		cerr << "  SampleKey[" << i << "] is " << hex << getSampleValue(i)->getKey() << endl ;
	}
}
#endif
