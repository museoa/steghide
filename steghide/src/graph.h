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

#ifndef SH_GRAPH_H
#define SH_GRAPH_H

#include "vertex.h"

class Graph {
	public:
	Graph (void) ;
	~Graph (void) ;

	/**
	 * add a vertex to this graph
	 * \param v a vertex
	 *
	 * The vertex pointed to by v is not copied, so don't delete it after adding it.
	 * The vertex label of this new vertex is getNumVertices()-1 (after adding).
	 **/
	void addVertex (Vertex *v) ;

	/**
	 * get the number of vertices in this graph
	 **/
	unsigned long getNumVertices (void) const ;

	/**
	 * get a vertex
	 * \param i the vertex label (index) of the vertex to be returned
	 * \return the i-th vertex
	 **/
	Vertex *getVertex (unsigned long i) const ;

	private:
	vector<Vertex*> Vertices ;
} ;

#endif // ndef SH_GRAPH_H
