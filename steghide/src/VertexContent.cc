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

#include "Graph.h"
#include "VertexContent.h"

//
// class VertexContent
//
VertexContent::VertexContent (Graph* g, SampleValue**& svs, SamplePos*& sposs)
	: GraphAccess(g)
{
	SampleValues = new SampleValue*[TheGraph->getSamplesPerVertex()] ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		unsigned short idx_minkey = i ;
		SampleKey minkey = SAMPLEKEY_MAX ;
		for (unsigned short j = i ; j < TheGraph->getSamplesPerVertex() ; j++) {
			if (svs[j]->getKey() < minkey) {
				minkey = svs[j]->getKey() ;
				idx_minkey = j ;
			}
		}

		// swap svs[i] <-> svs[idx_minkey] and sposs[i] <-> sposs[idx_minkey]
		SampleValue *tmp1 = svs[i] ;
		svs[i] = svs[idx_minkey] ;
		svs[idx_minkey] = tmp1 ;
		
		SamplePos tmp2 = sposs[i] ;
		sposs[i] = sposs[idx_minkey] ;
		sposs[idx_minkey] = tmp2 ;

		SampleValues[i] = svs[i] ;
	}

	Occurences = std::list<Vertex*>() ;
	DeletedOccurences = std::list<Vertex*>() ;

	// calculate SelfDegree
	SelfDegree = 0 ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		for (unsigned short j = i + 1 ; j < TheGraph->getSamplesPerVertex() ; j++) {
			if ((SampleValues[i]->isNeighbour (SampleValues[j])) && (SampleValues[i]->getBit() != SampleValues[j]->getBit())) {
				SelfDegree += 2 ;
			}
		}
	}
}

VertexContent::~VertexContent ()
{
	delete[] SampleValues ;
	Occurences.clear() ;
	DeletedOccurences.clear() ;
}

bool VertexContent::operator== (const VertexContent& vc) const
{
	bool retval = true ;
	SampleValue** vc_svs = vc.getSampleValues() ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		if (*(SampleValues[i]) != *(vc_svs[i])) {
			retval = false ;
			break ;
		}
	}
	return retval ;
}

unsigned long VertexContent::getDegree (void) const
{
	unsigned long retval = 0 ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		retval += SampleValues[i]->getNumEdges() ;
	}
	if (hasOccurences()) {
		myassert (SelfDegree <= retval) ;
		retval -= SelfDegree ;
	}
	return retval ;
}

std::list<Vertex*>::iterator VertexContent::markDeletedFromOccurences (std::list<Vertex*>::iterator it)
{
	Occurences.erase (it) ;
	return DeletedOccurences.insert (DeletedOccurences.end(), *it) ;
}

std::list<Vertex*>::iterator VertexContent::unmarkDeletedFromOccurences (std::list<Vertex*>::iterator it)
{
	DeletedOccurences.erase (it) ;
	return Occurences.insert (Occurences.end(), *it) ;
}

// FIXME - ? pre-compute hash in constructor (to gain speed) - similar to Key of SampleValue->...
// TODO - this should be tested
size_t VertexContent::getHash (void) const
{
	size_t retval = 0 ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		size_t newval = 0 ;
		size_t key = SampleValues[i]->getKey() ;
		switch (i) {
			case 0:
			newval = (size_t) key ;
			break ;

			case 1:
			newval = (key << 16) | (key >> 16) ;
			break ;

			case 2:
			newval = (key << 8) | (key >> 24) ;
			break ;

			case 3:
			newval = (key << 24) | (key >> 8) ;
			break ;
		}
		retval ^= newval ;
	}
	return retval ;
}

#ifdef DEBUG
void VertexContent::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;
	std::cerr << space << "VertexContent:" << std::endl ;
	for (unsigned short i = 0 ; i < TheGraph->getSamplesPerVertex() ; i++) {
		SampleValues[i]->print (spc + 1) ;
	}

	std::cerr << space << " Occurences (Labels):"  ;
	for (std::list<Vertex*>::const_iterator it = Occurences.begin() ; it != Occurences.end() ; it++) {
		std::cerr << " " << (*it)->getLabel() ;
	}
	std::cerr << std::endl ;
	
	std::cerr << space << " DeletedOccurences (Labels):" ;
	for (std::list<Vertex*>::const_iterator it = DeletedOccurences.begin() ; it != DeletedOccurences.end() ; it++) {
		std::cerr << " " << (*it)->getLabel() ;
	}
	std::cerr << std::endl ;
}
#endif

//
// struct VertexContentsEqual
//
bool VertexContentsEqual::operator() (const VertexContent *vc1, const VertexContent* vc2) const
{
	return (*vc1 == *vc2) ;
}

//
// struct hash<VertexContent*>
//
size_t sgi::hash<VertexContent*>::operator() (const VertexContent* vc) const
{
	return vc->getHash() ;
}