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

#ifndef SH_MATCHING_H
#define SH_MATCHING_H

#include "graph.h"
#include "graphaccess.h"
#include "vertex.h"

/**
 * \class Matching
 * \brief represent a matching on a graph
 **/
class Matching : private GraphAccess {
	public:
	/**
	 * create an empty matching that is ready for adding and augmenting
	 * \param g the underlying graph
	 **/
	Matching (Graph* g) ;
	
	/**
	 * returns true iff the vertex v is matched in this matching.
	 **/
	bool isMatched (Vertex *v) const
		{ return VertexInformation[v->getLabel()].isMatched() ; } ;

	/**
	 * returns true iff the vertex v is exposed (not matched) in this matching.
	 **/
	bool isExposed (Vertex *v) const
		{ return VertexInformation[v->getLabel()].isExposed() ; } ;

	/**
	 * get the edge that is in the matching and adjacent to v
	 * \return the matched edge or NULL if v is exposed
	 **/
	Edge *getMatchingEdge (Vertex *v) const
		{ return VertexInformation[v->getLabel()].getMatchingEdge() ; } ;

	/**
	 * get the cardinality (the number of matched edges)
	 **/
	unsigned long getCardinality (void) const
		{ return Cardinality ; } ;

	const std::list<Vertex*>& getExposedVertices (void) const
		{ return ExposedVertices ; } ;

	/**
	 * get access to the std::list of exposed vertices
	 * \return a pointer to the std::list of exposed vertices in this matching.
	 *
	 * The std::list that is pointed to by return value contains the exposed vertices
	 * even after augment has been called (it is the ExposedVertices member) an
	 * arbitrary number of times.
	 **/
	const std::list<Vertex*> *getExposedVerticesLink (void) const
		{ return &ExposedVertices ; } ;

	/**
	 * add an edge to the matching
	 * \param e the edge to add.
	 *
	 * For e=(v1,v2): neither v1 nor v2 are allowed to be adjacent
	 * to an edge that is already in the matching, 
	 **/
	Matching &addEdge (Edge *e) ;

	/**
	 * remove an edge from the matching
	 * \param e the edge to remove
	 *
	 * The edge e _must_ be in this matching
	 **/
	Matching &removeEdge (Edge *e) ;

	/**
	 * get the std::list of all edges in this matching
	 **/
	const std::list<Edge*>& getEdges (void) const
		{ return MatchingEdges ; } ;

	/**
	 * augment this matching along the given augmenting path
	 * \param path an augmenting path
	 *
	 * An augementing path is a path where edges with odd indices (the first, third,...) are not
	 * in the matching and edges with even indices are and the path has
	 * an odd length.
	 *
	 * The Edge objects in path must not be deleted by the caller, they
	 * will be deleted later by the Matching object.
	 **/
	Matching &augment (const std::vector<Edge*>& path) ;

	void printVerboseInfo (void) const ;

	private:
	/**
	 * \class VertexInfo
	 * \brief contains information about a vertex that is possibly in a matching
	 **/
	class VertexInfo {
		public:
		VertexInfo (std::list<Edge*>::iterator mit)
			{ setMatched (mit) ; } ;

		VertexInfo (std::list<Vertex*>::iterator eit)
			{ setExposed (eit) ; } ;

		bool isExposed (void) const
			{ return !Matched ; } ;

		bool isMatched (void) const
			{ return Matched ; } ;

		Edge *getMatchingEdge (void) const
			{ return *MatchedIterator ; } ;

		std::list<Edge*>::iterator getMatchedIterator (void) const
			{ return MatchedIterator ; } ;

		std::list<Vertex*>::iterator getExposedIterator (void) const
			{ return ExposedIterator ; } ;

		void setMatched (std::list<Edge*>::iterator mit)
			{ Matched = true ; MatchedIterator = mit ; } ;

		void setExposed (std::list<Vertex*>::iterator eit)
			{ Matched = false ; ExposedIterator = eit ; } ;

		private:
		bool Matched ;
		std::list<Edge*>::iterator MatchedIterator ;
		std::list<Vertex*>::iterator ExposedIterator ;
	} ;

	/// contains a VertexInfo object for every vertex
	std::vector<VertexInfo> VertexInformation ;

	/// the std::list of all exposed vertices
	std::list<Vertex*> ExposedVertices ;

	/// the std::list of all edges in the matching
	std::list<Edge*> MatchingEdges ;

	unsigned long Cardinality ;

#ifdef DEBUG
	public:
	bool check (void) const ;
	bool check_MatchingEdges_vs_VertexInformation (void) const ;
	bool check_ExposedVertices_vs_VertexInformation (void) const ;
	bool check_VertexInformation_Integrity (void) const ;

	bool check_ValidAugPath (const std::vector<Edge*>& path) const ;
#endif
} ;

#endif // ndef SH_MATCHING_H
