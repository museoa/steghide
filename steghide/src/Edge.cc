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

#include "Edge.h"
#include "SampleValue.h"
#include "Vertex.h"
#include "common.h"

Edge::Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2)
	: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2), Weight(FLT_MAX)
{
	myassert (v1->getLabel() != v2->getLabel()) ;
}

Edge::Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, float w)
	: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2), Weight(w)
{
	myassert (v1->getLabel() != v2->getLabel()) ;
}

float Edge::getWeight (void)
{
	if (Weight == FLT_MAX) {
		SampleValue *sv1 = Vertex1->getSampleValue(Index1) ;
		SampleValue *sv2 = Vertex2->getSampleValue(Index2) ;
		Weight = sv1->calcDistance(sv2) ;
	}
	return Weight ;
}

bool Edge::operator== (const Edge& e) const
{
	if ((Vertex1->getLabel() == e.Vertex1->getLabel()) && (Vertex2->getLabel() == e.Vertex2->getLabel()) &&
		(Index1 == e.Index1) && (Index2 == e.Index2)) {
		return true ;
	}
	if ((Vertex1->getLabel() == e.Vertex2->getLabel()) && (Vertex2->getLabel() == e.Vertex1->getLabel()) &&
		(Index1 == e.Index2) && (Index2 == e.Index1)) {
		return true ;
	}
	return false ;
}

void Edge::swap (void)
{
	Vertex* v_tmp = Vertex1 ;
	Vertex1 = Vertex2 ;
	Vertex2 = v_tmp ;

	unsigned short idx_tmp = Index1 ;
	Index1 = Index2 ;
	Index2 = idx_tmp ;
}

bool Edge::contains (const Vertex* v) const
{
	return ((v->getLabel() == Vertex1->getLabel()) || (v->getLabel() == Vertex2->getLabel())) ;
}

Vertex *Edge::getOtherVertex (const Vertex *v) const
{
	Vertex *retval = NULL ;
	if (v->getLabel() == Vertex1->getLabel()) {
		retval = Vertex2 ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		retval = Vertex1 ;
	}
	else {
		myassert (0) ;
	}
	return retval ;
}

SamplePos Edge::getSamplePos (Vertex *v) const
{
	SamplePos retval = 0 ;
	if (v->getLabel() == Vertex1->getLabel()) {
		retval = Vertex1->getSamplePos (Index1) ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		retval = Vertex2->getSamplePos (Index2) ;
	}
	else {
		myassert (0) ;
	}
	return retval ;
}

SampleValue *Edge::getOriginalSampleValue (Vertex *v) const
{
	unsigned short index = 0 ;
	if (v->getLabel() == Vertex1->getLabel()) {
		index = Index1 ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		index = Index2 ;
	}
	else {
		myassert (0) ;
	}
	return v->getSampleValue (index) ;
}

SampleValue *Edge::getReplacingSampleValue (Vertex *v) const
{
	SampleValue *retval = NULL ;
	if (v->getLabel() == Vertex1->getLabel()) {
		retval = Vertex2->getSampleValue (Index2) ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		retval = Vertex1->getSampleValue (Index1) ;
	}
	else {
		myassert (0) ;
	}
	return retval ;
}

#ifdef DEBUG
void Edge::print (unsigned short spc)
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "Edge:" << std::endl ;
	Vertex1->print (spc + 1) ;
	std::cerr << space << " Index1: " << Index1 << std::endl ;
	Vertex2->print (spc + 1) ;
	std::cerr << space << " Index2: " << Index2 << std::endl ;
	std::cerr << space << " Weight: " << getWeight() << std::endl ;
}
#endif
