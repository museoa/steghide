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
#include "edgeiterator.h"

EdgeIterator::EdgeIterator (Graph *g, Vertex *v)
	: GraphAccess (g)
{
	SrcVertex = v ;
	SVOppNeighsIndices = new unsigned long[g->getSamplesPerVertex()] ;
	Finished = false ;
	reset() ;
}

EdgeIterator::~EdgeIterator ()
{
	// TODO delete[] SVOppNeighsIndices ;
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
	assert (!Finished) ;
	SampleValue* srcsv = SrcVertex->getSampleValue(SrcIndex) ;
	SampleValue* destsv = TheGraph->getOppNeighs(srcsv)[SVOppNeighsIndices[SrcIndex]] ;

	SampleOccurenceIt++ ;
	if (SampleOccurenceIt == TheGraph->getSampleOccurences(destsv).end()) {
		// search new destination sample value
		SVOppNeighsIndices[SrcIndex]++ ;
		findNextEdge() ;
	}

	return *this ;
}

void EdgeIterator::reset ()
{
	for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
		SVOppNeighsIndices[i] = 0 ;
	}
	findNextEdge() ;
}

void EdgeIterator::findNextEdge ()
{
	float mindist = FLT_MAX ;
	unsigned short idx_mindist = SamplesPerVertex ;
	for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
		SampleValue* srcsv = SrcVertex->getSampleValue(i) ;
		while (SVOppNeighsIndices[i] < TheGraph->getNumOppNeighs(srcsv)) {
			SampleValue* destsv = (TheGraph->getOppNeighs(srcsv))[SVOppNeighsIndices[i]] ;
			if(isDestSampleValueOK(destsv)) {
				break ;
			}
			else {
				SVOppNeighsIndices[i]++ ;
			}
		}

		if (SVOppNeighsIndices[i] < TheGraph->getNumOppNeighs(srcsv)) {
			// there are edges from the i-th sample
			SampleValue* destsv = (TheGraph->getOppNeighs(srcsv))[SVOppNeighsIndices[i]] ;
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
		SampleValue* destsv = (TheGraph->getOppNeighs(srcsv))[SVOppNeighsIndices[SrcIndex]] ;
		list<SampleOccurence>::const_iterator it  = TheGraph->getSampleOccurences(destsv).begin() ;
		while (*(it->getVertex()) == *SrcVertex) {
			it++ ;
		}
		SampleOccurenceIt = it ;
	}
}

bool EdgeIterator::isDestSampleValueOK (const SampleValue *sv)
{
	bool found = false ;
	list<SampleOccurence> socc = TheGraph->getSampleOccurences(sv) ;
	list<SampleOccurence>::const_iterator it = socc.begin() ;
	do {
		if (*(it->getVertex()) == *SrcVertex) {
			it++ ;
		}
		else {
			found = true ;
		}
	} while (!found && it != socc.end()) ;
	return found ;
}
