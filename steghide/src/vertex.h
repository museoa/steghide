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

#ifndef SH_VERTEX_H
#define SH_VERTEX_H

#include <map>
#include <vector>

#include "common.h"
#include "cvrstgsample.h"
#include "edge.h"
#include "vertexcontent.h"

/**
 * \class Vertex
 * \brief a vertex in a graph
 **/
class Vertex {
	public:
	Vertex (VertexLabel l, unsigned short spebit) ;

	/**
	 * add a sample to this vertex
	 * \param pos the position of the sample in the cvrstgfile
	 * \param a pointer to the only(!) copy of the sample with this value
	 **/
	void addSample (SamplePos spos, CvrStgSample *s) ;

	/**
	 * of how many samples does this vertex consist ?
	 * \return the number of samples this vertex consists of
	 *
	 * The return value of this function is (must be equal to) the number of samples per ebit.
	 **/
	unsigned int getNumSamples (void) const ;

	/**
	 * get the i-th sample position
	 * \param i an index of a sample in this vertex
	 * \return the position of the sample in the associated cvrstgfile
	 **/
	SamplePos getSamplePos (unsigned int i) const ;

	/**
	 * get the i-th sample value
	 * \param i an index of a sample in this vertex
	 * \return the value of the sample in the associated cvrstgfile
	 **/
	CvrStgSample *getSample (unsigned int i) const ;

	/**
	 * set the content of the vertex (i.e. tell the vertex about his sample labels)
	 * This function also adds this to the list of Occurences in the vertex content and keeps the iterator.
	 * The SampleData in this vertex is sorted in the same way as it is in the vertex content.
	 **/
	void connectToContent (VertexContent *vc) ;
	VertexContent *getContent (void) const ;
	void deleteFromContent (void) ;
	unsigned long getDegree (void) const ;

	VertexLabel getLabel (void) const ;
	void setLabel (VertexLabel l) ;

	Edge *getShortestEdge (void) const ;
	void setShortestEdge (Edge *e) ;

	Edge *getMatchingEdge (void) const ;
	void setMatchingEdge (Edge *e) ;
	bool isMatched (void) const ;

#ifdef DEBUG
	void print (void) const ;
#endif
	private:
	VertexLabel Label ;

	unsigned short NumSamples ;
	vector<SamplePos> SamplePositions ;
	vector<CvrStgSample*> Samples ;
	
	VertexContent *Content ;
	list<Vertex*>::iterator OccurencesInContentIt ;

	Edge *ShortestEdge ;
	Edge *MatchingEdge ;
} ;

struct LongerShortestEdge : public binary_function<Vertex*,Vertex*,bool> {
	bool operator() (Vertex *v1, Vertex *v2)
		{ return (v1->getShortestEdge()->getWeight() > v2->getShortestEdge()->getWeight()) ; }
} ;

#endif // ndef SH_VERTEX_H
