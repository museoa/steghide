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
}

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
#if 0
				if (SampleOccurenceIt->getVertex()->getSampleValue(SampleOccurenceIt->getIndex()) != destsv) {
					std::cerr << "SampleOccurenceIt and destsv NOT CONSISTENT!" << std::endl ;
					std::cerr << " destsv->getLabel(): " << destsv->getLabel() << std::endl ;
					std::cerr << " occit->v->sv(occit->idx)->lbl: " << SampleOccurenceIt->getVertex()->getSampleValue(SampleOccurenceIt->getIndex())->getLabel() << std::endl ;
				}
				else {
					std::cerr << "SampleOccurenceIt and destsv are consistent" << std::endl ;
				}

				if (SampleOccurenceIt == Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
					std::cerr << "at end BEFORE incrementing" << std::endl ;
				}
				else {
					std::cerr << "not at end before incrementing" << std::endl ;
				}
#endif
				if (SrcVertex->getLabel() == 2) {
					std::cerr << "before incrementing" << std::endl ;
					std::cerr << " srcsv->getLabel(): " << srcsv->getLabel() << std::endl ;
					std::cerr << " destsv->getLabel(): " << destsv->getLabel() << std::endl ;
					std::cerr << " occit->v->sv(occit->idx)->lbl: " << SampleOccurenceIt->getVertex()->getSampleValue(SampleOccurenceIt->getIndex())->getLabel() << std::endl ;
					std::cerr << " SampleOccurenceIt->getVertex() is now:" << std::endl ;
					SampleOccurenceIt->getVertex()->print(2) ;
				}
				SampleOccurenceIt++ ;

				cont = false ;
				if (SampleOccurenceIt != Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
#if 0
					std::cerr << "not at end" << std::endl ;
					std::cerr << "srcsv->getEmbeddedValue(): " << srcsv->getEmbeddedValue() << std::endl ;
#endif
					if (SrcVertex->getLabel() == 2) {
						std::cerr << "after incrementing (not at end): " << std::endl ;
						std::cerr << " srcsv->getLabel(): " << srcsv->getLabel() << std::endl ;
						std::cerr << " destsv->getLabel(): " << destsv->getLabel() << std::endl ;
						std::cerr << " occit->v->sv(occit->idx)->lbl: " << SampleOccurenceIt->getVertex()->getSampleValue(SampleOccurenceIt->getIndex())->getLabel() << std::endl ;
						std::cerr << " incremented SampleOccurenceIt: SampleOccurenceIt->getVertex() is now:" << std::endl ;
						SampleOccurenceIt->getVertex()->print(1) ;
					}
#if 0
					std::cerr << "target match:" << (srcsv->getEmbeddedValue() == SampleOccurenceIt->getVertex()->getTargetValue(SampleOccurenceIt->getIndex())) << std::endl ;;
					std::cerr << "loop edge: " << (SampleOccurenceIt->getVertex()->getLabel() == SrcVertex->getLabel()) << std::endl ;
#endif
					if ((srcsv->getEmbeddedValue() != SampleOccurenceIt->getVertex()->getTargetValue(SampleOccurenceIt->getIndex())) || (SampleOccurenceIt->getVertex()->getLabel() == SrcVertex->getLabel())) {
						cont = true ;
					}
				}
				else {
					if (SrcVertex->getLabel() == 2) {
						std::cerr << "after incrementing (at end): " << std::endl ;
						std::cerr << " srcsv->getLabel(): " << srcsv->getLabel() << std::endl ;
						std::cerr << " destsv->getLabel(): " << destsv->getLabel() << std::endl ;
					}
				}
#if 0
				std::cerr << "cont: " << cont << std::endl ;
#endif
			} while (cont) ;

#if 0
			std::cerr << "found soccit" << std::endl ;
#endif

			if (SampleOccurenceIt == Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
				// search new destination sample value
				std::cerr << "incrementing SVALIndices[" << SrcIndex << "]" << std::endl ;
				SVALIndices[SrcIndex]++ ;
				findNextEdge() ;
			}
			break ;
		}
		case SAMPLEVALUE:
		{
			std::cerr << "SAMPLEVALUE MODE!!!" << std::endl ;
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
#if 0
		std::cerr << "setting CurrentEdge" << std::endl ;
#endif
		CurrentEdge.set (SrcVertex, SrcIndex, SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}

#if 0
	std::cerr << "++done" << std::endl ;
#endif

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

void EdgeIterator::findNextEdge ()
{
	if (SrcVertex->getLabel() == 2) {
		std::cerr << "EdgeIterator::findNextEdge called" << std::endl ;
	}

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

	if (SrcVertex->getLabel() == 2) {
		std::cerr << "after findNextEdge:" << std::endl ;
		for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
			std::cerr << " SVALIndices[" << i << "]: " << SVALIndices[i] << std::endl ;
		}
	}
}

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

UWORD32 EdgeIterator::MaxNumEdges = UWORD32_MAX ;
