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

#include "vertexcontent.h"

//
// class VertexContent
//
VertexContent::VertexContent (const vector<unsigned long> &sl)
{
	SampleLabels = sl ;
	sort (SampleLabels.begin(), SampleLabels.end()) ;

	Occurences = list<Vertex*>() ;
	Degree = 0 ;
}

void VertexContent::setDegree (unsigned long deg)
{
	Degree = deg ;
}

unsigned long VertexContent::getDegree() const
{
	return Degree ;
}

void VertexContent::decDegree()
{
	if (Degree > 0) {
		Degree-- ;
	}
}

SampleLabel VertexContent::getSampleLabel (unsigned short n) const
{
	return SampleLabels[n] ;
}

const vector<SampleLabel> &VertexContent::getSampleLabels() const
{
	return SampleLabels ;
}

list<Vertex*>::iterator VertexContent::addOccurence (Vertex *v)
{
	return Occurences.insert (Occurences.end(), v) ;
}

const list<Vertex*> &VertexContent::getOccurences() const
{
	return Occurences ;
}

void VertexContent::deleteFromOccurences (list<Vertex*>::iterator it)
{
	Occurences.erase (it) ;
}

//
// struct VertexContentsEqual
//
bool VertexContentsEqual::operator() (const VertexContent *vc1, const VertexContent* vc2) const
{
	// it is assumed that the SampleLabels returned from vc1,vc2 are sorted - FIXME - is this ok ?
	vector<SampleLabel> samplelabels1 = vc1->getSampleLabels() ;
	vector<SampleLabel> samplelabels2 = vc2->getSampleLabels() ;
	bool retval = true ;
	if (samplelabels1.size() == samplelabels2.size()) {
		for (unsigned short i = 0 ; i != samplelabels1.size() ; i++) {
			if (samplelabels1[i] != samplelabels2[i]) {
				retval = false ;
				break ;
			}
		}
	}
	else {
		retval = false ;
	}
	return retval ;
}

//
// struct hash<VertexContent*>
//
// TODO - this should be tested
size_t hash<VertexContent*>::operator() (const VertexContent *vc) const
{
	size_t retval = 0 ;
	vector<SampleLabel> samplelabels = vc->getSampleLabels() ;
	assert (samplelabels.size() <= 4) ;
	for (unsigned short i = 0 ; i < samplelabels.size() ; i++) {
		size_t newval = 0 ;
		switch (i) {
			case 0: {
			newval = (size_t) samplelabels[i] ;
			break ; }

			case 1: {
			newval = (samplelabels[i] << 16) | (samplelabels[i] >> 16) ;
			break ; }

			case 2: {
			newval = (samplelabels[i] << 8) | (samplelabels[i] >> 24 ) ;
			break ; }

			case 3: {
			newval = (samplelabels[i] << 24) | (samplelabels[i] >> 8) ;
			break ; }
		}
		retval ^= newval ;
	}
	return retval ;
}

