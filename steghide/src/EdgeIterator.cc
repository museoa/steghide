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

#include <cfloat>

#include "common.h"
#include "EdgeIterator.h"

EdgeIterator::EdgeIterator (Graph* g, Vertex *v)
	: GraphAccess (g)
{
	SrcVertex = v ;
	SVOppNeighsIndices = new unsigned long[TheGraph->getSamplesPerVertex()] ;
	Finished = false ;
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
	} while ((SampleOccurenceIt != TheGraph->SampleOccurences[destsv->getLabel()].end())
			&& (*(SampleOccurenceIt->getVertex()) == *SrcVertex)) ;

	if (SampleOccurenceIt == TheGraph->SampleOccurences[destsv->getLabel()].end()) {
		// search new destination sample value
		SVOppNeighsIndices[SrcIndex]++ ;
		findNextEdge() ;
	}

	return *this ;
}

void EdgeIterator::reset ()
{
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SVOppNeighsIndices[i] = 0 ;
	}
	findNextEdge() ;
}

void EdgeIterator::findNextEdge ()
{
	float mindist = FLT_MAX ;
	unsigned short idx_mindist = TheGraph->getSamplesPerVertex() ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SampleValue* srcsv = SrcVertex->getSampleValue(i) ;
		while (SVOppNeighsIndices[i] < TheGraph->SampleValueOppNeighs[srcsv].size()) {
			SampleValue* destsv = TheGraph->SampleValueOppNeighs[srcsv][SVOppNeighsIndices[i]] ;
			if(isDestSampleValueOK(destsv)) {
				break ;
			}
			else {
				SVOppNeighsIndices[i]++ ;
			}
		}

		if (SVOppNeighsIndices[i] < TheGraph->SampleValueOppNeighs[srcsv].size()) {
			// there are edges from the i-th sample
			SampleValue* destsv = TheGraph->SampleValueOppNeighs[srcsv][SVOppNeighsIndices[i]] ;
			if (srcsv->calcDistance(destsv) < mindist) {
				mindist = srcsv->calcDistance(destsv) ;
				idx_mindist = i ;
			}
		}
	}

	if (mindist == FLT_MAX) {
		// no edge has been found
		Finished = true ;
	}
	else {
		SrcIndex = idx_mindist ;
		SampleValue* srcsv = SrcVertex->getSampleValue(SrcIndex) ;
		SampleValue* destsv = TheGraph->SampleValueOppNeighs[srcsv][SVOppNeighsIndices[SrcIndex]] ;
		std::list<SampleOccurence>::const_iterator it = TheGraph->SampleOccurences[destsv->getLabel()].begin() ;
		while (*(it->getVertex()) == *SrcVertex) {
			it++ ;
		}
		SampleOccurenceIt = it ;
	}
}

bool EdgeIterator::isDestSampleValueOK (const SampleValue *sv)
{
	bool found = false ;
	std::list<SampleOccurence> socc = TheGraph->SampleOccurences[sv->getLabel()] ;
	std::list<SampleOccurence>::const_iterator it = socc.begin() ;
	while (!found && it != socc.end()) {
		if (*(it->getVertex()) == *SrcVertex) {
			it++ ;
		}
		else {
			found = true ;
		}
	}
	return found ;
}
