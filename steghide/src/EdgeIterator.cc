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

EdgeIterator::EdgeIterator ()
	: SrcVertex(NULL), SVALIndices(NULL)
{
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
}

EdgeIterator::EdgeIterator (Vertex *v, ITERATIONMODE m)
{
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	reset(v, m) ;
}

EdgeIterator::EdgeIterator (const EdgeIterator& eit)
{
	SrcVertex = eit.SrcVertex ;
	SrcIndex = eit.SrcIndex ;
	CurrentEdge = eit.CurrentEdge ;
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

// FIXME - speed improvement if no switch is used - instead a virtual function a derived classes ?
EdgeIterator& EdgeIterator::operator++ ()
{
	myassert (!Finished) ;

	switch (Mode) {
		case SAMPLEOCCURENCE:
		{
			SampleValue* srcsv = SrcVertex->getSampleValue(SrcIndex) ;
			SampleValue* destsv = (*(Globs.TheGraph->SVALists[SrcVertex->getTargetValue(SrcIndex)]))[srcsv][SVALIndices[SrcIndex]] ;

			bool cont = false ;
			do {
				SampleOccurenceIt++ ;

				cont = false ;
				if (SampleOccurenceIt != Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
					if ((srcsv->getEmbeddedValue() != SampleOccurenceIt->getVertex()->getTargetValue(SampleOccurenceIt->getIndex())) || (SampleOccurenceIt->getVertex()->getLabel() == SrcVertex->getLabel())) {
						cont = true ;
					}
				}
			} while (cont) ;

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

	if (!Finished) {
		CurrentEdge.set (SrcVertex, SrcIndex, SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
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
	if (!Finished) {
		CurrentEdge.set (SrcVertex, SrcIndex, SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}
}

void EdgeIterator::reset (Vertex* v, ITERATIONMODE m)
{
	SrcVertex = v ;
	reset(m) ;
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

#ifdef DEBUG
void EdgeIterator::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "Source Vertex:" << std::endl ;
	SrcVertex->print(spc + 1) ;
	std::cerr << space << "Source Index: " << SrcIndex << std::endl ;
	std::cerr << space << "Current Edge:" << std::endl ;
	CurrentEdge.print(spc + 1) ;
	std::cerr << space << "SampleOccurenceIt: <" << SampleOccurenceIt->getVertex()->getLabel() << "," << SampleOccurenceIt->getIndex() << ">" << std::endl ;
	std::cerr << space << "Finished: " << Finished << std::endl ;
	std::cerr << space << "EdgeIndex: " << EdgeIndex << std::endl ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		std::cerr << space << "SVALIndices[" << i << "]: " << SVALIndices[i] << std::endl ;
	}
}
#endif

UWORD32 EdgeIterator::MaxNumEdges = UWORD32_MAX ;
