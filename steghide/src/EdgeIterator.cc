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

#include "Edge.h"
#include "EdgeIterator.h"
#include "Graph.h"
#include "SampleOccurence.h"
#include "Vertex.h"
#include "common.h"

EdgeIterator::EdgeIterator (Vertex *v, ITERATIONMODE m)
{
	SrcVertex = v ;
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	reset(m) ;
}

EdgeIterator::EdgeIterator (const EdgeIterator& eit)
{
	SrcVertex = eit.SrcVertex ;
	SrcIndex = eit.SrcIndex ;
	Mode = eit.Mode ;
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SVALIndices[i] = eit.SVALIndices[i] ;
	}
	Finished = eit.Finished ;
	SampleOccurenceIt = eit.SampleOccurenceIt ;
}

EdgeIterator::~EdgeIterator ()
{
	delete[] SVALIndices ;
}

Edge* EdgeIterator::operator* () const
{
	Edge* retval = NULL ;
	if (!Finished) {
		retval = new Edge (SrcVertex, SrcIndex, SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}
	return retval ;
}

// FIXME - speed improvement if no switch is used - instead a virtual function a derived classes ?
EdgeIterator& EdgeIterator::operator++ ()
{
	myassert (!Finished) ;

	switch (Mode) {
		case SAMPLEOCCURENCE:
		{
			SampleValue* srcsv = SrcVertex->getSampleValue(SrcIndex) ;
			SampleValue* destsv = (*(Globs.TheGraph->SVALists[SrcVertex->getTargetValue(SrcIndex)]))[srcsv][SVALIndices[SrcIndex]] ;

			do {
				SampleOccurenceIt++ ;
			} while ((SampleOccurenceIt != Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) &&
				// to avoid looping edge (SrcVertex,SrcVertex)
				((SampleOccurenceIt->getVertex()->getLabel() == SrcVertex->getLabel()) ||
				// to find a valid edge
				(srcsv->getEmbeddedValue() != SampleOccurenceIt->getVertex()->getTargetValue(SampleOccurenceIt->getIndex())))) ;

			if (SampleOccurenceIt == Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
				// search new destination sample value
				SVALIndices[SrcIndex]++ ;
				findNextEdge() ;
			}
			break ;
		}
		case SAMPLEVALUE:
		{
			SVALIndices[SrcIndex]++ ;
			findNextEdge() ;
			break ;
		}
	}

	// increment EdgeIndex while checking that it has not become too high
	if (++EdgeIndex >= MaxNumEdges) {
		Finished = true ;
	}
	else { // FIXME - temporarily
		if (!Finished) { // might have been set in findNextEdge()
			myassert (check_EdgeValidity()) ;
		}
	}

	return *this ;
}

void EdgeIterator::reset (ITERATIONMODE m)
{
	Mode = m ;
	Finished = false ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SVALIndices[i] = 0 ;
	}
	findNextEdge() ;
	EdgeIndex = 0 ;

	if (!Finished) {// FIXME - temporarily
		myassert (check_EdgeValidity()) ;
	}
}

void EdgeIterator::findNextEdge ()
{
	UWORD32 mindist = UWORD32_MAX ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SampleValue* srcsv = SrcVertex->getSampleValue(i) ;
		SampleValue* destsv = NULL ;
		std::list<SampleOccurence>::const_iterator soccit_candidate ;

		// increment SVALIndices[i] until it points to a valid destination sample value
		while (SVALIndices[i] < (*(Globs.TheGraph->SVALists[SrcVertex->getTargetValue(i)]))[srcsv].size()) {
			destsv = (*(Globs.TheGraph->SVALists[SrcVertex->getTargetValue(i)]))[srcsv][SVALIndices[i]] ;

			// look for a sample occurence of destsv - thereby setting soccit_candidate
			bool found = false ;
			const std::list<SampleOccurence>& socc = Globs.TheGraph->SampleOccurences[destsv->getLabel()] ;
			soccit_candidate = socc.begin() ;
			while (!found && soccit_candidate != socc.end()) {
				if ((soccit_candidate->getVertex()->getLabel() == SrcVertex->getLabel()) || 
					(srcsv->getEmbeddedValue() != soccit_candidate->getVertex()->getTargetValue(soccit_candidate->getIndex()))) {
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
				SVALIndices[i]++ ;
			}
		}

		// test if the destination sample value leads to edge with (until now) minimal distance - thereby setting SrcIndex and SampleOccurenceIt
		if (SVALIndices[i] < (*(Globs.TheGraph->SVALists[SrcVertex->getTargetValue(i)]))[srcsv].size()) {
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

bool EdgeIterator::check_EdgeValidity () const
{
	bool retval = true ;
	Edge* e = **this ;

	unsigned short idx1 = e->getIndex1() ;
	unsigned short idx2 = e->getIndex2() ;
	Vertex* v1 = e->getVertex1() ;
	Vertex* v2 = e->getVertex2() ;
	SampleValue* sv1 = v1->getSampleValue(idx1) ;
	SampleValue* sv2 = v2->getSampleValue(idx2) ;
	EmbValue t1 = v1->getTargetValue(idx1) ;
	EmbValue t2 = v2->getTargetValue(idx2) ;

	retval = (v1->getLabel() != v2->getLabel()) ; // is already asserted in edge constructor
	retval = sv1->isNeighbour(sv2) && retval ;
	retval = (sv1->getEmbeddedValue() == t2) && retval ;
	retval = (sv2->getEmbeddedValue() == t1) && retval ;
	delete e ;

	return retval ;
}

UWORD32 EdgeIterator::MaxNumEdges = UWORD32_MAX ;
