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

#include <hash_set>
#include <list>
#include <map>
#include <queue>
#include <vector>

#include "common.h"
#include "vertex.h"
#include "vertexcontent.h"

class Graph {
	public:
	Graph (void) ;
	Graph (CvrStgFile *f) ;
	~Graph (void) ;

	/**
	 * add a vertex to this graph
	 * \param poss a vector of sample positions the new vertex should consist of
	 *
	 * The vertex label of this new vertex is getNumVertices()-1 (after adding).
	 **/
	void addVertex (const vector<SamplePos> &poss) ;

	/**
	 * get a vertex
	 * \param i the vertex label (index) of the vertex to be returned
	 * \return the i-th vertex
	 **/
	Vertex *getVertex (VertexLabel i) const ;

	void startAdding (void) ;
	void finishAdding (void) ;

	void calcMatching (void) ;

	private:
	/* TODO - implement this
	static const unsigned short ShortestEdgesRange = 10 ;
	*/

	void setupConstrHeuristic (void) ;
	void doConstrHeuristic (void) ;

	void updateShortestEdge (Vertex *v) ;

	unsigned long absdiff (unsigned long a, unsigned long b) ;

	/**
	 * insert the edge e into matching and invalidate the two vertices for the matching algorithm
	 **/
	void insertInMatching (Edge *e) ;

	/// contains the calculated matching
	vector<Edge*> Matching ;

	/// contains the vertices in this graph - Vertices[i] is the vertex with label i
	vector<Vertex*> Vertices ;

	/// contains the unique sample list - Samples[i] is the sample with label i
	vector<CvrStgSample*> Samples ;
	/// is used to create the unique sample list
	hash_set<CvrStgSample*,hash<CvrStgSample*>,SamplesEqual> Samples_set ;

	/**
	 * SampleOppositeNeighbourhood[sample_label] contains a list of those SampleLabels that describe samples that
	 * are in the set Samples[sample_label]->getOppositeNeighbours().
	 **/
	vector<list<SampleLabel> > SampleOppositeNeighbourhood ;

	/**
	 * contains the unique vertex contents - the size of the vector is the number of unique samples (indexed by sample labels)
	 * for a sample label lbl the list UniqueVertexContents[lbl] contains all UniqueVertextContent objects that contain
	 * the sample described by lbl.
	 * The main purpose of this data structure is to hold the vertex degrees.
	 **/
	vector<list<VertexContent*> > VertexContents ;

	/**
	 * SampleOccurences[sample_label] contains a map of the occurences of this sample in the cvrstgfile (in the first
	 * component) and in the graph (in the second component). The first component (the SamplePos) is the sample
	 * position of this occurence in the cvrstgfile. The second component is a pair whose first component
	 * is the vertex where this sample occurs and whose second component is the ushort index in the vertex of this sample.
	 * The main purpose of this data structure is to (implicitly) hold the edges.
	 **/
	vector<map<SamplePos,pair<Vertex*,unsigned short> > > SampleOccurences ;

	/// used for construction heuristic - contains all vertices of degree 1
	priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> VerticesDeg1 ;
	/// used for contruction heuristic - contains all vertices with degree greater than 1
	priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> VerticesDegG ;

	CvrStgFile *File ;
	unsigned short SamplesPerEBit ;

#ifdef DEBUG
	/**
	 * prints graph in a format suitable as input to the C implementation
	 * of Gabow's non-weighted matching algorithm by E. Rothberg to stdout
	 * (available at: ftp://ftp.zib.de/pub/Packages/mathprog/matching/index.html
	 **/
	void print (void) const ;

	void printUnmatchedVertices (void) const ;

	unsigned long check_degree (Vertex *v) const ;

	bool check_matching (void) const ;

	bool check_ds (void) const ;
	bool check_sizes (void) const ;
	bool check_samples (void) const ;
	bool check_vertices (void) const ;
	bool check_degrees (void) const ;
	bool check_sampleoppositeneighbourhood (void) const ;
	bool check_vertexcontents (void) const ;
	bool check_sampleoccurences (void) const ;
#endif
} ;

#endif // ndef SH_GRAPH_H
