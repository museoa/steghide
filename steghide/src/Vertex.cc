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
#include "Vertex.h"
#include "common.h"

Vertex::Vertex (VertexLabel l, SamplePos* sposs, SampleValue** svalues, EmbValue t)
{
	setLabel (l) ;
	SamplePositions = sposs ;
	SampleValues = svalues ;

	SampleOccurenceIts = new std::list<SampleOccurence>::iterator[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	ShortestEdge = NULL ;
	valid = true ;

	// calculate sample target values
	TargetValues = new EmbValue[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	EmbValue msum = 0 ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) { // fill TargetValues with source values temporarily
		TargetValues[i] = SampleValues[i]->getEmbeddedValue() ;
		msum = (msum + TargetValues[i]) % Globs.TheCvrStgFile->getEmbValueModulus() ;
	}

	// by solving msum + d = t (mod m)
	if (t < msum) {
		t += Globs.TheCvrStgFile->getEmbValueModulus() ;
	}
	EmbValue d = t - msum ;

	// and adding d to source values to produce target values
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		TargetValues[i] = (TargetValues[i] + d) % Globs.TheCvrStgFile->getEmbValueModulus() ;
	}
}

Vertex::~Vertex ()
{
	delete[] TargetValues ;
	delete[] SampleOccurenceIts ;
	delete[] SamplePositions ;
	delete[] SampleValues ;
	delete ShortestEdge ;
}

void Vertex::markDeleted ()
{
#ifdef DEBUG
	printDebug (2, "marking vertex with label %lu as deleted.", getLabel()) ;
#endif

	if (valid) {
		// delete from sample occurences in graph
		for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
			SampleOccurenceIts[i] = Globs.TheGraph->markDeletedSampleOccurence (SampleOccurenceIts[i]) ;
		}

		valid = false ;
	}
}

void Vertex::unmarkDeleted ()
{
#ifdef DEBUG
	printDebug (2, "unmarking deletion of vertex with label %lu.", getLabel()) ;
#endif

	if (!valid) {
		// undelete into sample occurences in graph
		for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
			SampleOccurenceIts[i] = Globs.TheGraph->unmarkDeletedSampleOccurence (SampleOccurenceIts[i]) ;
		}

		valid = true ;
	}
}

void Vertex::updateShortestEdge ()
{
#ifdef DEBUG
	printDebug (3, "updating shorted edge for vertex with label %lu", getLabel()) ;
#endif

	delete ShortestEdge ;

	if (getDegree() == 0) {
		ShortestEdge = NULL ;
	}
	else {
		EdgeIterator edgeit (this) ;
		ShortestEdge = new Edge (**edgeit) ;
	}
}

EmbValue Vertex::getEmbeddedValue () const
{
	EmbValue retval = 0 ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		retval = (retval + SampleValues[i]->getEmbeddedValue()) % Globs.TheCvrStgFile->getEmbValueModulus() ;
	}
	return retval ;
}

UWORD32 Vertex::getDegree () const // FIXME - speed this up (other algorithm) if possible
{
	UWORD32 degree = 0 ;

	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		// iterate through target neighbours summing up degree
		const std::vector<SampleValue*>& tneighs = (*(Globs.TheGraph->SVALists[TargetValues[i]]))[SampleValues[i]] ;
		for (std::vector<SampleValue*>::const_iterator tnit = tneighs.begin() ; tnit != tneighs.end() ; tnit++) {
			degree += Globs.TheGraph->NumSampleOccurences[(*tnit)->getLabel()] ;
		}

		// substract self degree (loops)
		for (unsigned short j = i + 1 ; j < Globs.TheCvrStgFile->getSamplesPerVertex() ; j++) {
			if (SampleValues[i]->isNeighbour(SampleValues[j])) {
				if (TargetValues[i] == SampleValues[j]->getEmbeddedValue()) {
					degree-- ;
				}
				if (TargetValues[j] == SampleValues[i]->getEmbeddedValue()) {
					degree-- ;
				}
			}
		}
	}

	return degree ;
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
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		std::cerr << space << " SamplePosition: " << getSamplePos(i) << std::endl ;
		getSampleValue(i)->print (spc + 1) ;
	}
	std::cerr << space << " ShortestEdge:" << std::endl ;
	if (ShortestEdge) {
		ShortestEdge->print(spc + 2) ;
	}
	else {
		std::cerr << space << "  NULL" << std::endl ;
	}
	std::cerr << space << " getDegree(): " << getDegree() << std::endl ;
}

void Vertex::printEdges() const
{
	std::cerr << "edges of vertex with label " << getLabel() << std::endl ;
	EdgeIterator edgeit (Globs.TheGraph->getVertex(getLabel())) ;
	while (*edgeit != NULL) {
		Edge* e = new Edge (**edgeit) ;
		std::cerr << "  label of other vertex: " << e->getOtherVertex(this)->getLabel() << std::endl ;
		std::cerr << "  weight: " << e->getWeight() << std::endl ;
		delete e ;
		++edgeit ;
	}
}
#endif
