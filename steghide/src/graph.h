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
#include "sampleoccurence.h"
#include "svalueoppneigh.h"
#include "vertex.h"
#include "vertexcontent.h"

/**
 * \class Graph
 * \brief a graph constructed from a cover-file and a message to be embedded
 **/
class Graph {
	public:
	Graph (void) {} ;

	/**
	 * construct a graph
	 * \param f the underlying cover file
	 * \param sposs the vector of k-tuples of sample positions that will make up the vertices
	 **/
	Graph (CvrStgFile *f, vector<SamplePos*>& sposs) ;

	/**
	 * construct sample-related data structures
	 *
	 * needs: sposs(unsorted)
	 * provides: svalues(unsorted), SampleValues
	 **/
	void constructSamples (const vector<SamplePos*> &sposs, vector<SampleValue**>& svalues) ;

	/**
	 * construct vertex-related data structures
	 *
	 * needs: sposs(unsorted), svalues(unsorted), SampleValues
	 * provides: sposs(sorted), svalues(sorted), vc_set, VertexContents (values), Vertices
	 **/
	void constructVertices (vector<SamplePos*>& sposs, vector<SampleValue**>& svalues,
		hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>& vc_set) ;

	/**
	 * construct edge-related data structures
	 *
	 * needs: vc_set, SampleValues, Vertices
	 * provides: SValueOppNeighs, SampleOccurences, VertexContents (degrees)
	 **/
	void constructEdges (const hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>& vc_set) ;

	~Graph (void) ;

	/**
	 * get the number of vertices in this graph
	 **/
	unsigned long getNumVertices (void) const
		{ return Vertices.size() ; } ;

	/**
	 * get a vertex
	 * \param i the vertex label (index) of the vertex to be returned (must be < getNumVertices())
	 * \return the i-th vertex
	 **/
	Vertex *getVertex (VertexLabel l) const
		{ return Vertices[l] ; } ;

	void unmarkDeletedAllVertices (void) ;

	SampleValue* getSampleValue (const SampleValueLabel l) const
		{ return SampleValues[l] ; } ;

#if 0
	/**
	 * get the label of an opposite neighbour of a given sample value
	 * \param svl the label of the sample value whose neighbour should be returned
	 * \param i the index of the opposite neighbour in all opposite neighbours of svl
	 * \return the i-th opposite neighbour of the sample value represented by svl
	 **/
	SampleValueLabel getOppNeighLabel (SampleValueLabel svl, unsigned long i) const
		{ return SValueOppNeighs[svl][i] ; } ;

	SampleValue *getOppNeigh (SampleValueLabel svl, unsigned long i) const
		{ return SampleValues[SValueOppNeighs[svl][i]] ; } ;

	SampleValue *getOppNeigh (SampleValue* sv, unsigned long i) const
		{ return SValueOppNeighs[sv][i] ; } ;
#endif

	/**
	 * get the number of opposite neighbours of a given sample value
	 **/
	unsigned long getNumOppNeighs (const SampleValue* sv) const
		{ return SValueOppNeighs[sv].size() ; } ;

	const vector<SampleValue*>& getOppNeighs (const SampleValue *sv) const
		{ return SValueOppNeighs[sv] ; } ;

	/**
	 * get the number of occurences of a given sample value
	 **/
	unsigned long getNumSampleOccurences (SampleValueLabel svl) const
		{ return SampleOccurences[svl].size() ; } ;

	unsigned long getNumSampleOccurences (SampleValue *sv) const
		{ return SampleOccurences[sv->getLabel()].size() ; } ;

	const list<SampleOccurence>& getSampleOccurences (SampleValue *sv) const
		{ return SampleOccurences[sv->getLabel()] ; } ;

	list<SampleOccurence>::iterator markDeletedSampleOccurence (list<SampleOccurence>::iterator it) ;
	list<SampleOccurence>::iterator unmarkDeletedSampleOccurence (list<SampleOccurence>::iterator it) ;

	void undeleteAllVertices (void) ;

	void printVerboseInfo (void) ;

	unsigned short getSamplesPerVertex (void) const
		{ return SamplesPerEBit ; } ;

	private:
	/// contains the vertices in this graph - Vertices[i] is the vertex with label i
	vector<Vertex*> Vertices ;

	/// contains the unique sample list - SampleValues[i] is the sample value with label i
	vector<SampleValue*> SampleValues ;

	SampleValueOppositeNeighbourhood SValueOppNeighs ;

#if 0
	/**
	 * contains the unique vertex contents - the size of the vector is the number of unique samples (indexed by sample labels)
	 * for a sample label lbl the list UniqueVertexContents[lbl] contains all UniqueVertextContent objects that contain
	 * the sample described by lbl.
	 * The main purpose of this data structure is to hold the vertex degrees.
	 **/
	vector<list<VertexContent*> > VertexContents ; // FIXME nc - ?? use vector instead of list - performance in time/memory ??
#endif

	vector<list<SampleOccurence> > SampleOccurences ;

	/// contains those sample occurences that have been deleted from SampleOccurences
	vector<list<SampleOccurence> > DeletedSampleOccurences ;


	CvrStgFile *File ;
	unsigned short SamplesPerEBit ;

#ifdef DEBUG
	unsigned long NumVertexContents ;

	/**
	 * prints graph in a format suitable as input to the C implementation
	 * of Gabow's non-weighted matching algorithm by E. Rothberg to stdout
	 * (available at: ftp://ftp.zib.de/pub/Packages/mathprog/matching/index.html
	 **/
	void print (void) const ;

	// FIXME nc - clean up these functions

	void printUnmatchedVertices (void) const ;
	void printUnmatchedVerticescontaining (unsigned long samplekey) const ;

	unsigned long check_degree (Vertex *v) const ;

	bool check_matching (vector<Edge*> *m) const ;

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
