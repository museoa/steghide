/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 2002 Stefan Hetzl <shetzl@teleweb.at>
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

#include "edge.h"
#include "vertex.h"

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

//DEBUG
SamplePos Edge::getSamplePos (Vertex *v) const
{
	//cerr << "in Edge::getSamplePos" << endl << "v->getLabel(): " << v->getLabel() << endl ;
//	cerr << "Vertex1->getLabel(): " << Vertex1->getLabel() << endl ;
//	cerr << "Vertex2->getLabel(): " << Vertex2->getLabel() << endl ;
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

CvrStgSample *Edge::getSample (Vertex *v) const
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
