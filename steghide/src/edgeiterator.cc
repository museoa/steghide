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
	Finished = false ;
	reset() ;
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
	SampleValue* cursrcsv = SrcVertex->getSampleValue(SrcIndex) ;
	SampleValue* curdestsv = TheGraph->getOppNeighs(cursrcsv)[SVOppNeighsIndices[SrcIndex]] ;

	SampleOccurenceIt++ ;
	if (SampleOccurenceIt == TheGraph->getSampleOccurences(curdestsv).end()) {
		// search new destination sample value
		SVOppNeighsIndices[SrcIndex]++ ;
		SrcIndex = findShortestSampleValue() ;
		if (!Finished) {
			SampleOccurenceIt = TheGraph->getSampleOccurences(SrcVertex->getSampleValue(SrcIndex)).begin() ;
		}
	}

	return *this ;
}

void EdgeIterator::reset ()
{
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SVOppNeighsIndices[i] = 0 ;
	}
	SrcIndex = findShortestSampleValue() ;
	if (!Finished) {
		SampleOccurenceIt = TheGraph->getSampleOccurences(SrcVertex->getSampleValue(SrcIndex)).begin() ;
	}
}

unsigned short EdgeIterator::findShortestSampleValue ()
{
	unsigned short retval = SamplesPerVertex ;

	float mindist = FLT_MAX ;
	for (unsigned short i = 0 ; i < SamplesPerVertex ; i++) {
		SampleValue* srcsv = SrcVertex->getSampleValue(i) ;
		SampleValue* destsv = TheGraph->getOppNeighs(srcsv)[SVOppNeighsIndices[i]] ;
		if (!(TheGraph->getSampleOccurences(destsv).empty())) {
			if (srcsv->calcDistance(destsv) < mindist) {
				mindist = srcsv->calcDistance(destsv) ;
				retval = i ;
			}
		}
	}
	if (mindist == FLT_MAX) {
		Finished = true ;
	}

	return retval ;
}
