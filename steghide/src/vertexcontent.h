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

#ifndef SH_DEGREE_H
#define SH_DEGREE_H

#include <functional>
#include <hash_set>
#include <list>
#include <vector>

#include "common.h"
// declared here to prevent circulating includes
class Vertex ;

class VertexContent {
	public:
	/**
	 * construct a vertex content object using the sorted sample labels
	 * \param sl the sample labels - will be sorted in the constructor
	 **/
	VertexContent (const vector<unsigned long> &sl) ;

	/**
	 * set the degree of this vertex content
	 * \param deg the degree (calculated by the calling function)
	 **/
	void setDegree (unsigned long deg) ;

	/**
	 * get the degree
	 **/
	unsigned long getDegree (void) const ;

	/**
	 * decrement the degree
	 **/
	void decDegree (void) ;

	SampleLabel getSampleLabel (unsigned short n) const ;
	const vector<SampleLabel> &getSampleLabels (void) const ;

	/**
	 * add a vertex to the vertex occurences of this vertex content
	 * \param v the vertex to add
	 * \return the iterator pointing to the added vertex in the Occurences list (should be used as argument to deleteFromOccurences)
	 **/
	list<Vertex*>::iterator addOccurence (Vertex *v) ;

	/**
	 * get the list of vertices where this content occurs
	 **/
	const list<Vertex*> &getOccurences (void) const ;

	/**
	 * delete a vertex from the Occurences list
	 * \param it the iterator pointing to the vertex to be deleted
	 **/
	void deleteFromOccurences (list<Vertex*>::iterator it) ;
	
	private:
	vector<unsigned long> SampleLabels ;
	list<Vertex*> Occurences ;
	unsigned long Degree ;
} ;

struct VertexContentsEqual : binary_function<VertexContent*, VertexContent*, bool> {
	bool operator() (const VertexContent *vc1, const VertexContent* vc2) const ;
} ;

struct hash<VertexContent*> {
	size_t operator() (const VertexContent *vc) const ;
} ;

#endif // ndef SH_DEGREE_H
