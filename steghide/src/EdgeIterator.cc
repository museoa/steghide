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
#include "SampleOccurence.h"
#include "Vertex.h"

EdgeIterator::EdgeIterator (Graph* g, Vertex *v)
	: GraphAccess (g)
{
	SrcVertex = v ;
	SVOppNeighsIndices = new unsigned long[TheGraph->getSamplesPerVertex()] ;
	reset() ;
}

EdgeIterator::EdgeIterator (const EdgeIterator& eit)
	: GraphAccess (eit.TheGraph)
{
	SrcVertex = eit.SrcVertex ;
	SrcIndex = eit.SrcIndex ;
	SVOppNeighsIndices = new unsigned long[eit.TheGraph->getSamplesPerVertex()] ;
	for (unsigned short i = 0 ; i < eit.TheGraph->getSamplesPerVertex() ; i++) {
		SVOppNeighsIndices[i] = eit.SVOppNeighsIndices[i] ;
	}
	Finished = eit.Finished ;
	SampleOccurenceIt = eit.SampleOccurenceIt ;
}

EdgeIterator::~EdgeIterator ()
{
	delete[] SVOppNeighsIndices ;
}

Edge* EdgeIterator::operator* ()
{
	Edge* retval = NULL ;
	if (!Finished) {
		retval = new Edge (SrcVertex, SrcIndex, SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}
	return retval ;
}

EdgeIterator& EdgeIterator::operator++ ()
{
	myassert (!Finished) ;
	SampleValue* srcsv = SrcVertex->getSampleValue(SrcIndex) ;
	SampleValue* destsv = TheGraph->SampleValueOppNeighs[srcsv][SVOppNeighsIndices[SrcIndex]] ;

	do { // to avoid looping edge (SrcVertex,SrcVertex)
		SampleOccurenceIt++ ;
	} while ((SampleOccurenceIt != TheGraph->SampleOccurences[destsv->getLabel()].end()) &&
		(SampleOccurenceIt->getVertex()->getLabel() == SrcVertex->getLabel())) ;

	if (SampleOccurenceIt == TheGraph->SampleOccurences[destsv->getLabel()].end()) {
		// search new destination sample value
		SVOppNeighsIndices[SrcIndex]++ ;
		findNextEdge() ;
	}

	// increment EdgeIndex while checking that it has not become too high
	if (++EdgeIndex >= MaxNumEdges) {
		Finished = true ;
	}

	return *this ;
}

void EdgeIterator::reset ()
{
	Finished = false ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SVOppNeighsIndices[i] = 0 ;
	}
	findNextEdge() ;
	EdgeIndex = 0 ;
}

void EdgeIterator::findNextEdge ()
{
	UWORD32 mindist = UWORD32_MAX ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SampleValue* srcsv = SrcVertex->getSampleValue(i) ;
		SampleValue* destsv = NULL ;
		std::list<SampleOccurence>::const_iterator soccit_candidate ;

		// increment SVOppNeighsIndices[i] until it points to a valid destination sample value
		while (SVOppNeighsIndices[i] < TheGraph->SampleValueOppNeighs[srcsv].size()) {
			destsv = TheGraph->SampleValueOppNeighs[srcsv][SVOppNeighsIndices[i]] ;

			// look for a sample occurence of destsv - thereby setting soccit_candidate
			bool found = false ;
			const std::list<SampleOccurence>& socc = TheGraph->SampleOccurences[destsv->getLabel()] ;
			soccit_candidate = socc.begin() ;
			while (!found && soccit_candidate != socc.end()) {
				if (soccit_candidate->getVertex()->getLabel() == SrcVertex->getLabel()) {
					soccit_candidate++ ;
				}
				else {
					found = true ;
				}
			}

			if (found) {
				break ;
			}
			else {
				SVOppNeighsIndices[i]++ ;
			}
		}

		// test if the destination sample value leads to edge with (until now) minimal distance - thereby setting SrcIndex and SampleOccurenceIt
		if (SVOppNeighsIndices[i] < TheGraph->SampleValueOppNeighs[srcsv].size()) {
			UWORD32 thisdist = srcsv->calcDistance(destsv) ;
			if (thisdist < mindist) {
				mindist = thisdist ;
				SrcIndex = i ;
				SampleOccurenceIt = soccit_candidate ;
			}
		}
	}

	if (mindist == UWORD32_MAX) {
		// no edge has been found
		Finished = true ;
	}
}

UWORD32 EdgeIterator::MaxNumEdges = UWORD32_MAX ;
