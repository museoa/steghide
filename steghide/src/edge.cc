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
#include "edge.h"
#include "vertex.h"

Edge::Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2)
	: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2)
{
	assert (v1->getLabel() != v2->getLabel()) ;
	SampleValue *sv1 = v1->getSampleValue(idx1) ;
	SampleValue *sv2 = v2->getSampleValue(idx2) ;
	Weight = sv1->calcDistance(sv2) ;
}

Edge::Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, float w)
	: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2), Weight(w)
{
	assert (v1->getLabel() != v2->getLabel()) ;
}

Vertex *Edge::getOtherVertex (Vertex *v) const
{
	Vertex *retval = NULL ;
	if (v->getLabel() == Vertex1->getLabel()) {
		retval = Vertex2 ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		retval = Vertex1 ;
	}
	else {
		assert (0) ;
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
		assert (0) ;
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
		assert (0) ;
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
		assert (0) ;
	}
	return retval ;
}
