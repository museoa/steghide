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

#include "common.h"
#include "sampleoccurence.h"
#include "svalueoppneigh.h"
#include "vertex.h"
#include "vertexcontent.h"
#include "wrapper_hash_set.h"

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
	 * \param sposs the std::vector of k-tuples of sample positions that will make up the vertices
	 **/
	Graph (CvrStgFile *f, std::vector<SamplePos*>& sposs) ;

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
	Vertex* getVertex (VertexLabel l) const
		{ return Vertices[l] ; } ;

	void unmarkDeletedAllVertices (void) ;

	unsigned long getNumSampleValues (void) const
		{ return SampleValues.size() ; } ;

	SampleValue* getSampleValue (const SampleValueLabel l) const
		{ return SampleValues[l] ; } ;

	/**
	 * get the number of opposite neighbours of a given sample value
	 **/
	unsigned long getNumOppNeighs (const SampleValue* sv) const
		{ return SValueOppNeighs[sv].size() ; } ;

	const std::vector<SampleValue*>& getOppNeighs (const SampleValue *sv) const
		{ return SValueOppNeighs[sv] ; } ;

	const std::list<VertexContent*>& getVertexContents (const SampleValue *sv) const
		{ return VertexContents[sv->getLabel()] ; } ;

	/**
	 * get the number of occurences of a given sample value
	 **/
	unsigned long getNumSampleOccurences (SampleValueLabel svl) const
		{ return SampleOccurences[svl].size() ; } ;

	unsigned long getNumSampleOccurences (SampleValue *sv) const
		{ return SampleOccurences[sv->getLabel()].size() ; } ;

	const std::list<SampleOccurence>& getSampleOccurences (const SampleValue *sv) const
		{ return SampleOccurences[sv->getLabel()] ; } ;

	std::list<SampleOccurence>::iterator markDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;
	std::list<SampleOccurence>::iterator unmarkDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;

	void undeleteAllVertices (void) ;

	void printVerboseInfo (void) ;

	unsigned short getSamplesPerVertex (void) const
		{ return SamplesPerEBit ; } ;

	private:
	/// contains the vertices in this graph - Vertices[i] is the vertex with label i
	std::vector<Vertex*> Vertices ;

	/// contains the unique sample std::list - SampleValues[i] is the sample value with label i
	std::vector<SampleValue*> SampleValues ;

	SampleValueOppositeNeighbourhood SValueOppNeighs ;

	/**
	 * contains the unique vertex contents - the size of the std::vector is the number of unique samples (indexed by sample labels)
	 * for a sample label lbl the std::list UniqueVertexContents[lbl] contains all UniqueVertextContent objects that contain
	 * the sample described by lbl.
	 * The main purpose of this data structure is to hold the vertex degrees.
	 **/
	std::vector<std::list<VertexContent*> > VertexContents ; // FIXME - ?? use std::vector instead of std::list - performance in time/memory ??

	std::vector<std::list<SampleOccurence> > SampleOccurences ;

	/// contains those sample occurences that have been deleted from SampleOccurences
	std::vector<std::list<SampleOccurence> > DeletedSampleOccurences ;

	CvrStgFile *File ;
	unsigned short SamplesPerEBit ;

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
	bool check_sampleoppositeneighbourhood (void) const ;
	bool check_vertexcontents (void) const ;

	bool check_SampleOccurences (void) const ;
	bool check_SampleOccurences_correctness (void) const ;
	bool check_SampleOccurences_completeness (void) const ;
#endif
} ;

#endif // ndef SH_GRAPH_H
