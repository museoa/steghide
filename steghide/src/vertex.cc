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

Vertex::Vertex (VertexLabel l, unsigned short spebit)
	: Label(l)
{
	NumSamples = 0 ;
	SamplePositions = vector<SamplePos> (spebit) ;
	Samples = vector<SampleValue*> (spebit) ;
	Content = NULL ;
	ShortestEdge = NULL ;
	MatchingEdge = NULL ;
}

void Vertex::addSample (SamplePos spos, SampleValue *s)
{
	assert (NumSamples < SamplePositions.size()) ;
	SamplePositions[NumSamples] = spos ;
	Samples[NumSamples] = s ;
	NumSamples++ ;
}

unsigned int Vertex::getNumSamples() const
{
	return NumSamples ;
}

SamplePos Vertex::getSamplePos (unsigned int i) const
{
	assert (i < SamplePositions.size()) ;
	return SamplePositions[i] ;
}

SampleValue *Vertex::getSample (unsigned int i) const
{
	assert (i < Samples.size()) ;
	return Samples[i] ;
}

void Vertex::connectToContent (VertexContent *vc)
{
	Content = vc ;
	OccurencesInContentIt = Content->addOccurence (this) ;

	// reorder SamplePositions and Samples to match the order of SampleLabels in the vertex content
	vector<SampleValueLabel> sl = Content->getSampleValueLabels() ;
	
	vector<SamplePos> new_sposs (SamplePositions.size()) ;
	vector<SampleValue*> new_samples (Samples.size()) ;

	vector<bool> srcused (Samples.size(), false) ;

	for (unsigned short dest = 0 ; dest < sl.size() ; dest++) {
		for (unsigned short src = 0 ; src < Samples.size() ; src++) {
			if (Samples[src]->getLabel() == sl[dest]) {
				if (!srcused[src]) {
					// to prevent problems when a vertex contains the same sample more than one time:
					// only the first occurence of this sample would be used as source
					new_sposs[dest] = SamplePositions[src] ;
					new_samples[dest] = Samples[src] ;
					srcused[src] = true ;
					break ;
				}
			}
		}
	}

	SamplePositions = new_sposs ;
	Samples = new_samples ;
}

VertexContent *Vertex::getContent() const
{
	return Content ;
}

void Vertex::deleteFromContent()
{
	Content->deleteFromOccurences (OccurencesInContentIt) ;
}

unsigned long Vertex::getDegree() const
{
	return Content->getDegree() ;
}

VertexLabel Vertex::getLabel() const
{
	return Label ;
}

void Vertex::setLabel (VertexLabel l)
{
	Label = l ;
}

Edge *Vertex::getShortestEdge() const
{
	return ShortestEdge ;
}

void Vertex::setShortestEdge (Edge *e)
{
	ShortestEdge = e ;
}

Edge *Vertex::getMatchingEdge() const
{
	return MatchingEdge ;
}

void Vertex::setMatchingEdge (Edge *e)
{
	MatchingEdge = e ;
}

bool Vertex::isMatched() const
{
	return (MatchingEdge != NULL) ;
}

#ifdef DEBUG
void Vertex::print() const
{
	cerr << "vertex with label " << Label << endl ;
	for (unsigned short i = 0 ; i < NumSamples ; i++) {
		cerr << "  SampleLabel[" << i << "] is " << Samples[i]->getLabel() << endl ;
		cerr << "  SamplePositon[" << i << "] is " << SamplePositions[i] << endl ;
		cerr << "  SampleKey[" << i << "] is " << hex << Samples[i]->getKey() << endl ;
	}
}
#endif
