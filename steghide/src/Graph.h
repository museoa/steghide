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

#ifndef SH_GRAPH_H
#define SH_GRAPH_H

#include <list>
#include <map>
#include <queue>
#include <vector>

#include "EdgeIterator.h"
#include "SampleValueOppositeNeighbourhood.h"
#include "wrapper_hash_set.h"
#include "common.h"

class BitString ;
class SampleOccurence ;
class Permutation ;
class Vertex ;
class VertexContent ;
struct VertexContentsEqual ;

/**
 * \class Graph
 * \brief a graph constructed from a cover file and a message to be embedded
 *
 * This class provides a purely graph-theoretic interface to any other class.
 * Some classes however need access to the internal (steganographic) representation,
 * for example: Vertex, EdgeIterator,... . These are declared as friends of Graph here and
 * thus have direct access to the private data structures.
 **/
class Graph {
	public:
	Graph (void) {} ;

	/**
	 * construct a graph
	 * \param cvr the underlying cover file
	 * \param emb the bitstring to be embedded
	 **/
	Graph (CvrStgFile* cvr, const BitString& emb, const Permutation& perm) ;

	/**
	 * destructor
	 **/
	~Graph (void) ;

	/**
	 * get the number of vertices in this graph
	 **/
	unsigned long getNumVertices (void) const
		{ return Vertices.size() ; } ;

	/**
	 * get a vertex
	 * \param l the vertex label (index) of the vertex to be returned (must be < getNumVertices())
	 * \return the vertex with label l
	 **/
	Vertex* getVertex (VertexLabel l) const
		{ return Vertices[l] ; } ;

	void unmarkDeletedAllVertices (void) ;

	void printVerboseInfo (void) ;

	/// check the integrity of the data structures - only used for debugging and testing
	bool check (void) const ;
	/// check the integrity of the Vertices data structure - only used for debugging and testing
	bool check_Vertices (void) const ;
	/// check the integrity of the SampleValues data structure - only used for debugging and testing
	bool check_SampleValues (void) const ;
	/// check the integrity of the VertexContents data structure - only used for debugging and testing
	bool check_VertexContents (void) const ;

	private:
	//
	// friend-declarations
	// Note: private members of Graph that are declared before this point
	//       should not be used by friends.
	//
	friend class ConstructionHeuristic ;
	friend class EdgeIterator ;
	friend class GraphAccess ;
	friend class SampleValueOppositeNeighbourhood ;
	friend class Vertex ;
	friend class VertexContent ;

	/// contains the vertices in this graph - Vertices[l] is the vertex with label l
	std::vector<Vertex*> Vertices ;

	/// contains the list of (unique) sample values - SampleValues[l] is the sample value with label l
	std::vector<SampleValue*> SampleValues ;

	/**
	 * contains pointers to all vertex contents - VertexContents[l] contains a pointer
	 * to a VertexContent object iff the sample value with label l is part of this vertex content.
	 **/
	// FIXME - time/memory ?? use std::vector instead of std::list ??
	std::vector<std::list<VertexContent*> > VertexContents ;

	/// SampleValueOppNeighs[l] is the vector of opposite neighbours of the sample value with label l
	SampleValueOppositeNeighbourhood SampleValueOppNeighs ;

	/**
	 * SampleOccurences[l] contains all occurences of the sample value with label l
	 **/
	std::vector<std::list<SampleOccurence> > SampleOccurences ;

	/// contains those sample occurences that have been deleted from SampleOccurences
	std::vector<std::list<SampleOccurence> > DeletedSampleOccurences ;

	unsigned short getSamplesPerVertex (void) const
		{ return SamplesPerEBit ; } ;

	std::list<SampleOccurence>::iterator markDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;
	std::list<SampleOccurence>::iterator unmarkDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;

	//
	// end of friend-declarations
	// Note: private members of Graph that are declared beyond this point should
	//       not be used by friends.
	//

	/**
	 * construct sample-related data structures
	 *
	 * needs: sposs(unsorted)
	 * provides: svalues(unsorted), SampleValues
	 **/
	void constructSamples (const std::vector<SamplePos*> &sposs, std::vector<SampleValue**>& svalues) ;

	/**
	 * construct vertex-related data structures
	 *
	 * needs: sposs(unsorted), svalues(unsorted), SampleValues
	 * provides: sposs(sorted), svalues(sorted), vc_set, VertexContents (values), Vertices (except SampleOccurenceIts)
	 **/
	void constructVertices (std::vector<SamplePos*>& sposs, std::vector<SampleValue**>& svalues,
		sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>& vc_set) ;

	/**
	 * construct edge-related data structures
	 *
	 * needs: vc_set, SampleValues, Vertices (except SampleOccurenceIts)
	 * provides: SValueOppNeighs, SampleOccurences, VertexContents (degrees), Vertices (SampleOccurenceIts)
	 **/
	void constructEdges (const sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>& vc_set) ;

	CvrStgFile *File ;
	unsigned short SamplesPerEBit ;

	bool check_VertexContents_soundness (void) const ;
	bool check_VertexContents_completeness (void) const ;
	bool check_VertexContents_pointerequiv (void) const ;

#ifdef DEBUG
	unsigned long NumVertexContents ;

	public:
	/**
	 * prints graph in a format suitable as input to the C implementation
	 * of Gabow's non-weighted matching algorithm by E. Rothberg to stdout
	 * (available at: ftp://ftp.zib.de/pub/Packages/mathprog/matching/index.html
	 **/
	void print (void) const ;

	// FIXME - clean up these functions

	void printUnmatchedVertices (void) const ;
	void printUnmatchedVerticescontaining (unsigned long samplekey) const ;

	unsigned long check_degree (Vertex *v) const ;

	bool check_matching (std::vector<Edge*> *m) const ;

	bool check_ds (void) const ;
	bool check_sizes (void) const ;
	bool check_samples (void) const ;
	bool check_vertices (void) const ;
	bool check_degrees (void) const ;
	bool check_vertexcontents (void) const ;

	bool check_SampleOccurences (void) const ;
	bool check_SampleOccurences_correctness (void) const ;
	bool check_SampleOccurences_completeness (void) const ;
#endif
} ;

#endif // ndef SH_GRAPH_H
