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

Edge::Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2, unsigned long w)
	: Vertex1(v1), Index1(idx1), Vertex2(v2), Index2(idx2), Weight(w)
{
	assert (v1->getLabel() != v2->getLabel()) ;
}

Vertex *Edge::getVertex1() const
{
	return Vertex1 ;
}

Vertex *Edge::getVertex2() const
{
	return Vertex2 ;
}

unsigned long Edge::getWeight() const
{
	return Weight ;
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

CvrStgSample *Edge::getOriginalSample (Vertex *v) const
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
	return v->getSample (index) ;
}

CvrStgSample *Edge::getReplacingSample (Vertex *v) const
{
	CvrStgSample *retval = NULL ;
	if (v->getLabel() == Vertex1->getLabel()) {
		retval = Vertex2->getSample (Index2) ;
	}
	else if (v->getLabel() == Vertex2->getLabel()) {
		retval = Vertex1->getSample (Index1) ;
	}
	else {
		assert (0) ;
	}
	return retval ;
}
