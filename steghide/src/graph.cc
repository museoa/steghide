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

#include <hash_set>
#include <list>
#include <map>
#include <queue>
#include <vector>

#include "cvrstgfile.h"
#include "graph.h"
#include "msg.h"
#include "vertex.h"

Graph::Graph()
{
}

Graph::Graph (CvrStgFile *f)
{
	File = f ;
	SamplesPerEBit = File->getSamplesPerEBit() ;
}

Graph::~Graph()
{
	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		delete *i ;
	}
	// TODO - mehr löschen
}

void Graph::startAdding()
{
	Vertices.clear() ;
}

void Graph::addVertex (const vector<SamplePos> &poss)
{
	assert (poss.size() == SamplesPerEBit) ;

	static VertexLabel CurVertexLabel = 0 ;
	Vertex *v = new Vertex (CurVertexLabel, SamplesPerEBit) ;

	for (vector<SamplePos>::const_iterator i = poss.begin() ; i != poss.end() ; i++) {
		CvrStgSample *sample = File->getSample (*i) ;

		hash_set<CvrStgSample*,hash<CvrStgSample*>,SamplesEqual>::iterator j = Samples_set.find (sample) ;
		if (j == Samples_set.end()) { // sample has not been found - add it !
			Samples_set.insert (sample) ;
		}
		else { // sample is already in Samples_map
			delete sample ;
			sample = *j ;
		} 

		v->addSample (*i, sample) ;
	}

	Vertices.push_back (v) ;
	CurVertexLabel++ ;
}

void Graph::finishAdding()
{
	// move the hash_set Samples_set into the vector Samples, set sample labels and initialize other data structures
	// needs: filled Sample_set
	Samples = vector<CvrStgSample*> (Samples_set.size()) ;
	unsigned long label = 0 ;
	for (hash_set<CvrStgSample*,hash<CvrStgSample*>,SamplesEqual>::const_iterator i = Samples_set.begin() ; i != Samples_set.end() ; i++) {
		Samples[label] = *i ;
		Samples[label]->setLabel (label) ;
		label++ ;
	}

	// initializations
	SampleOccurences = vector<map<SamplePos,pair<Vertex*,unsigned short> > > (Samples.size()) ;
	VertexContents = vector<list<VertexContent*> > (Samples.size()) ;
	SampleOppositeNeighbourhood = vector<list<SampleLabel> > (Samples.size()) ;

	// create the SampleOppositeNeighbourhood data structure
	// needs: filled Samples_set, filled Samples
	for (unsigned long label = 0 ; label < Samples.size() ; label++) {
		list<CvrStgSample*> *potoppneighs = Samples[label]->getOppositeNeighbours() ;
		for (list<CvrStgSample*>::iterator i = potoppneighs->begin() ; i != potoppneighs->end() ; i++) {
			hash_set<CvrStgSample*, hash<CvrStgSample*>, SamplesEqual>::const_iterator searchres = Samples_set.find (*i) ;
			if (searchres != Samples_set.end()) { // this sample does exist in this graph
				SampleOppositeNeighbourhood[label].push_back ((*searchres)->getLabel()) ;
			}
			delete (*i) ; // delete the copy of the sample made in getOppositeNeighbours
		}
		delete potoppneighs ; // delete the list
	}

	Samples_set.clear() ;

	// create the (hash_)set of (unique) vertex contents and set vertex contents in vertices
	// needs: filled Vertices
	hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual> vc_set ;
	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		vector<SampleLabel> samplelabels (SamplesPerEBit) ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			samplelabels[j] = (*i)->getSample(j)->getLabel() ;
		}
		VertexContent *vcontent = new VertexContent (samplelabels) ;

		hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>::iterator uvc = vc_set.find (vcontent) ;
		if (uvc == vc_set.end()) { // vcontent has not been found - add it!
			vc_set.insert (vcontent) ;
		}
		else { // vcontent is already there
			delete vcontent ;
			vcontent = *uvc ;
		}

		(*i)->connectToContent (vcontent) ;
	}

	// fill the SampleOccurences data structure
	// needs: filled Vertices, sorted SampleData in Vertex Contents and Vertices
	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			pair<SamplePos,pair<Vertex*,unsigned short> > occ ((*i)->getSamplePos(j), pair<Vertex*,unsigned short> (*i, j)) ;
			pair<map<SamplePos,pair<Vertex*,unsigned short> >::iterator,bool> res = SampleOccurences[(*i)->getSample(j)->getLabel()].insert (occ) ;
			assert (res.second) ;
		}
	}

	// compute degrees and fill the VertexContents data structure
	// needs: filled vc_set, filled SampleOccurences
	for (hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>::iterator i = vc_set.begin() ; i != vc_set.end() ; i++) {
		unsigned long degree = 0 ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			const list<SampleLabel> &oppneighbours = SampleOppositeNeighbourhood[(*i)->getSampleLabel(j)] ;
			for (list<SampleLabel>::const_iterator k = oppneighbours.begin() ; k != oppneighbours.end() ; k++) {
				degree += SampleOccurences[*k].size() ;

				// no loops
				for (unsigned short l = 0 ; l < SamplesPerEBit ; l++) {
					if (*k == (*i)->getSampleLabel(l)) {
						degree-- ;
					}
				}
			}
		}
		assert (degree >= 0) ;
		(*i)->setDegree (degree) ;

		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			VertexContents[(*i)->getSampleLabel(j)].insert (VertexContents[(*i)->getSampleLabel(j)].end(), *i) ;
		}
	}

	// calculate the shortest edge for each vertex
	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		updateShortestEdge (*i) ;
	}

#ifdef DEBUG
	//cerr << "checking data structures:" << endl ;
	//assert (check_ds()) ;
	// FIXME - wenn alle bugs draußen - einmal bbtest damit durchlaufen lassen
#endif
}

unsigned long Graph::absdiff (unsigned long a, unsigned long b)
{
	return ((a > b) ? (a - b) : (b - a)) ;
}

Vertex *Graph::getVertex(unsigned long i) const
{
	assert (i < Vertices.size()) ;
	return Vertices[i] ;
}

void Graph::updateShortestEdge (Vertex *v1)
{
	Edge *e = NULL ;

	if (v1->getDegree() > 0) {
		bool found = false ;
		unsigned short v1_idx = 0 , v2_idx = 0 ;
		Vertex *v2 = NULL ;
		unsigned long min_weight = ULONG_MAX ;

		for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
			const list<SampleLabel> &oppneighbours = SampleOppositeNeighbourhood[v1->getSample(i)->getLabel()] ;
			for (list<SampleLabel>::const_iterator j = oppneighbours.begin() ; j != oppneighbours.end() ; j++) {
				// search the nearest samples to v1->getSamplePos(i)
				map<SamplePos,pair<Vertex*,unsigned short> >::iterator upbound = SampleOccurences[(*j)].upper_bound (v1->getSamplePos(i)) ;
				map<SamplePos,pair<Vertex*,unsigned short> >::iterator lowbound = upbound ;

				// do not use samples in vertex v1 (no loops)
				while ((upbound != SampleOccurences[(*j)].end()) && (upbound->second.first->getLabel() == v1->getLabel())) {
					upbound++ ;
				}

				if (lowbound == SampleOccurences[(*j)].begin()) {
					lowbound = SampleOccurences[(*j)].end() ;
				}
				else {
					lowbound-- ;
					// do not use samples in vertex v1 (no loops)
					while ((lowbound != SampleOccurences[(*j)].begin()) && (lowbound->second.first->getLabel() == v1->getLabel())) {
						lowbound-- ;
					}
					if ((lowbound == SampleOccurences[(*j)].begin()) && (lowbound->second.first->getLabel() == v1->getLabel())) {
						lowbound = SampleOccurences[(*j)].end() ;
					}
				}

				if (upbound != SampleOccurences[(*j)].end()) {
					// upbound is next larger SamplePos in another vertex
					unsigned long weight = absdiff (upbound->first, v1->getSamplePos(i)) ;
					if (weight < min_weight) {
						v1_idx = i ;
						v2_idx = upbound->second.second ;
						v2 = upbound->second.first ;
						min_weight = weight ;
						found = true ;
					}
				}
				if (lowbound != SampleOccurences[(*j)].end()) {
					// lowbound is next larger SamplePos in another vertex
					unsigned long weight = absdiff (lowbound->first, v1->getSamplePos(i)) ;
					if (weight < min_weight) {
						v1_idx = i ;
						v2_idx = lowbound->second.second ;
						v2 = lowbound->second.first ;
						min_weight = weight ;
						found = true ;
					}
				}
			}
		}

		assert (found) ;
		assert (v1->getLabel() != v2->getLabel()) ;
		e = new Edge (v1, v1_idx, v2, v2_idx, min_weight) ;
	}

	v1->replaceShortestEdge (e) ;
}

void Graph::insertInMatching (Edge *e)
{
	Vertex *v1 = e->getVertex1() ;
	Vertex *v2 = e->getVertex2() ;

	assert (!v1->isMatched() && !v2->isMatched()) ;

	v1->replaceMatchingEdge (e) ;
	v2->replaceMatchingEdge (e) ;
	Matching.push_back (e) ;

	// delete samples of these vertices from SampleOccurences
	for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
		SampleOccurences[v1->getSample(i)->getLabel()].erase (v1->getSamplePos(i)) ;
		SampleOccurences[v2->getSample(i)->getLabel()].erase (v2->getSamplePos(i)) ;
	}

	// get all opposite neighbour samples of these two vertices
	list<SampleLabel> oppneighbours ;	// duplicates are valid
	for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
		// FIXME - move vs. copy <-> splice vs. ?
		list<SampleLabel> tmp = SampleOppositeNeighbourhood[v1->getSample(i)->getLabel()] ; // copy for Vertex1
		oppneighbours.splice (oppneighbours.begin(), tmp) ;
		tmp = SampleOppositeNeighbourhood[v2->getSample(i)->getLabel()] ;	// copy for Vertex2
		oppneighbours.splice (oppneighbours.begin(), tmp) ;
	}

	// for all neighbour samples decrement the degree of all vertex contents
	for (list<SampleLabel>::iterator i = oppneighbours.begin() ; i != oppneighbours.end() ; i++) {
		for (list<VertexContent*>::iterator j = VertexContents[(*i)].begin() ; j != VertexContents[(*i)].end() ; j++) {
			(*j)->decDegree() ;
			if ((*j)->getDegree() == 1) {
				// move all vertices with this vertex content to the VerticesDeg1 Priority Queue
				list<Vertex*> occ = (*j)->getOccurences() ;
				for (list<Vertex*>::iterator k = occ.begin() ; k != occ.end() ; k++) {
					VerticesDeg1.push (*k) ;
					// vertices are deleted from VerticesDegG in doConstrHeuristic
				}
			}
		}
	}

	// delete vertices from their vertex contents
	v1->deleteFromContent() ;
	v2->deleteFromContent() ;

}

void Graph::calcMatching()
{
	unsigned long nvertices = Vertices.size() ;

	if (Args.Verbosity.getValue() == VERBOSE) { // don't do something needing linear time if it is not useful
		VerboseMessage vmsg1 (_("number of distinct samples: %lu"), Samples.size()) ;
		vmsg1.printMessage() ;

		VerboseMessage vmsg2 (_("number of vertices: %lu"), nvertices) ;
		vmsg2.printMessage() ;

		unsigned long sumdeg = 0 ;
		unsigned long mindeg = ULONG_MAX ;
		unsigned long maxdeg = 0 ;
		for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
			unsigned long deg = (*i)->getDegree() ;

			sumdeg += deg ;
			if (deg < mindeg) {
				mindeg = deg ;
			}
			if (deg > maxdeg) {
				maxdeg = deg ;
			}
		}

		VerboseMessage vmsg3 (_("average vertex degree: %.1f"), ((float) sumdeg / (float) nvertices)) ;
		vmsg3.printMessage() ;
		VerboseMessage vmsg4 (_("minimum vertex degree: %lu"), mindeg) ;
		vmsg4.printMessage() ;
		VerboseMessage vmsg5 (_("maximum vertex degree: %lu"), maxdeg) ;
		vmsg5.printMessage() ;

		assert (sumdeg % 2 == 0) ;

		VerboseMessage vmsg6 (_("number of edges: %lu"), sumdeg / 2) ;
		vmsg6.printMessage() ;
		VerboseMessage vmsg7 (_("calculating the matching...")) ;
		vmsg7.printMessage() ;
	}

	setupConstrHeuristic() ;
	doConstrHeuristic() ;

#ifdef DEBUG
	//check_matching() ;
#endif

	if (Args.Verbosity.getValue() == VERBOSE) {
		VerboseMessage vmsg8 (_("size of the matching after construction heuristic: %lu"), Matching.size()) ;
		vmsg8.printMessage() ;
		unsigned long numvertices = nvertices - (2 * Matching.size()) ;
		VerboseMessage vmsg9 (_("number of unmatched vertices: %lu (%.1f%)"), numvertices, 100.0 * ((float) numvertices / (float) nvertices)) ;
		vmsg9.printMessage() ;

		unsigned long sumweights = 0 ;
		for (vector<Edge*>::iterator i = Matching.begin() ; i != Matching.end() ; i++) {
			sumweights += (*i)->getWeight() ;
		}

		VerboseMessage vmsg10 (_("average edge weight: %.1f"), ((float) sumweights / (float) Matching.size())) ;
		vmsg10.printMessage() ;
	}
}

//
// Construction Heuristic (modified Karp&Sisper)
//
void Graph::setupConstrHeuristic()
{
	VerticesDeg1 = priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> () ;
	VerticesDegG = priority_queue<Vertex*, vector<Vertex*>, LongerShortestEdge> () ;

	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		unsigned long degree = (*i)->getDegree() ;
		if (degree != 0) {
			if (degree == 1) {
				VerticesDeg1.push (*i) ;
			}
			else {
				VerticesDegG.push (*i) ;
			}
		}
	}

	Matching = vector<Edge*>() ;
}

void Graph::doConstrHeuristic()
{
	while (!(VerticesDegG.empty() && VerticesDeg1.empty())) {
		bool usethisvertex = false ;
		Vertex *v = NULL ;

		if (!VerticesDeg1.empty()) {
			do { // get the vertex that is nearest to top and still has degree 1
				v = VerticesDeg1.top() ;
				VerticesDeg1.pop() ;
				assert (v->getDegree() <= 1) ;
				// implicitly delete vertices that have degree zero or have already been matched
				if ((v->getDegree() == 1) && (!v->isMatched())) {
					updateShortestEdge (v) ;
					usethisvertex = true ;
				}
			} while ((!usethisvertex) && (!VerticesDeg1.empty())) ;
		}
		else {
			// TODO !! - unsigned short nvertex = RndSrc.getValue (ShortestEdgesRange) ;

			do { // get the vertex that is at the top (with updated len of shortest edge) and has a degree > 1
				v = VerticesDegG.top() ;
				VerticesDegG.pop() ;
				// implicitly delete vertices that have been moved to VerticesDeg1 or have already been matched
				if ((v->getDegree() > 1) && (!v->isMatched())) {
					unsigned long weight_before = v->getShortestEdge()->getWeight() ;
					updateShortestEdge (v) ;
					if (v->getShortestEdge()->getWeight() == weight_before) {
						usethisvertex = true ;
					}
					else {
						assert (v->getShortestEdge()->getWeight() > weight_before) ;	// weight can only rise
						VerticesDegG.push (v) ;
					}
				}

			} while ((!usethisvertex) && (!VerticesDegG.empty())) ;
		}

		if (usethisvertex) {
			insertInMatching (v->getShortestEdge()) ;
		}
	}
}

#ifdef DEBUG
void Graph::printUnmatchedVertices (void) const
{
	unsigned long num = 0 ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		if (!(*i)->isMatched()) {
			num++ ;
			for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
				cerr << (*i)->getSample(j)->getKey() ;
				if (j + 1 != SamplesPerEBit) {
					cerr << "/" ;
				}
			}
			cerr << endl ;
		}
	}
	cerr << num << " unmatched vertices found." << endl ;
}

bool Graph::check_matching (void) const
{
	bool retval = true ;

	cerr << "checking if vector<Edge*> Matching is a matching (without loops)" << endl ;
	vector<bool> inmatching (Vertices.size(), false) ;
	for (vector<Edge*>::const_iterator i = Matching.begin() ; i != Matching.end() ; i++) {
		VertexLabel v1lbl = (*i)->getVertex1()->getLabel() ;
		VertexLabel v2lbl = (*i)->getVertex2()->getLabel() ;
		if (v1lbl == v2lbl) {
			retval = false ;
			cerr << "FAILED: matching contains a loop" << endl ;
			break ;
		}
		if (inmatching[v1lbl]) {
			cerr << "FAILED: matching contains vertex with label " << v1lbl << " twice." << endl ;
			retval = false ;
			break ;
		}
		else {
			inmatching[v1lbl] = true ;
		}
		if (inmatching[v2lbl]) {
			cerr << "FAILED: matching contains vertex with label " << v2lbl << " twice." << endl ;
			retval = false ;
			break ;
		}
		else {
			inmatching[v2lbl] = true ;
		}
	}

	return retval ;
}

unsigned long Graph::check_degree (Vertex *v) const
{
	unsigned long degree = 0 ;
	for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
		CvrStgSample *srcsample = v->getSample(i) ;
		for (vector<Vertex*>::const_iterator j = Vertices.begin() ; j != Vertices.end() ; j++) {
			if ((*j)->getLabel() != v->getLabel()) { // no loops
				if (!(*j)->isMatched()) { // only umatched vertices
					for (unsigned short k = 0 ; k < SamplesPerEBit ; k++) {
						CvrStgSample *destsample = (*j)->getSample(k) ;
						if ((srcsample->isNeighbour(destsample)) && (srcsample->getBit() != destsample->getBit())) { // is opposite neighbour
							degree++ ;
						}
					}
				}
			}
		}
	}
	return degree ;
}

bool Graph::check_ds (void) const
{
	bool retval = true ;

	retval = check_sizes() && retval ;
	retval = check_samples() && retval ;
	retval = check_vertices() && retval ;
	retval = check_degrees() && retval ;
	retval = check_sampleoppositeneighbourhood() && retval ;
	retval = check_vertexcontents() && retval ;
	retval = check_sampleoccurences() && retval ;

	return retval ;
}

bool Graph::check_sizes (void) const
{
	cerr << "checking sizes" << endl ;
	unsigned long n = Samples.size() ;
	bool retval = ((n == SampleOppositeNeighbourhood.size()) && (n == VertexContents.size()) && (n == SampleOccurences.size())) ;
	if (!retval) {
		cerr << "FAILED: sizes don't match" << endl ;
	}
	return retval ;
}

bool Graph::check_samples (void) const
{
	bool retval = true ;
	
	unsigned long n = Samples.size() ;

	cerr << "checking Samples: index - label consistency" << endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SampleLabel l = Samples[i]->getLabel() ;
		if (l != i) {
			retval = false ;
			cerr << "FAILED: wrong sample label" << endl ;
			break ;
		}
	}

	cerr << "checking Samples: uniqueness of samples" << endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		for (unsigned long j = 0 ; j < n ; j++) {
			if (i != j) {
				if (Samples[i]->getKey() == Samples[j]->getKey()) {
					retval = false ;
					cerr << "FAILED: duplicate sample in Sampels[" << i << "] and Samples[" << j << "]" << endl ;
				}
			}
		}
	}

	return retval ;
}

bool Graph::check_vertices (void) const
{
	bool retval = true ;
	cerr << "checking Vertices: index - label consistency" << endl ;
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		VertexLabel l = Vertices[i]->getLabel() ;
		if (l != i) {
			retval = false ;
			cerr << "FAILED: wrong vertex label" << endl ;
			break ;
		}
	}
	return retval ;
}

bool Graph::check_degrees (void) const
{
	bool retval = true ;

	cerr << "checking vertex degrees (real vs. stored)" << endl ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		unsigned long deg_real = check_degree(*i) ;
		unsigned long deg_stored = (*i)->getDegree() ;
		if (deg_real != deg_stored) {
			cerr << "check_degree == getDegree failed" << endl ;
			(*i)->print() ;
			cerr << "getDegree: " << deg_stored << endl ;
			cerr << "check_degree: " << deg_real << endl ;
			retval = false ;
			break ;
		}
	}

	return retval ;
}

bool Graph::check_sampleoppositeneighbourhood (void) const
{
	bool retval = true ;

	cerr << "checking SampleOppositeNeighbourhood: samples are opposite" << endl ;
	for (unsigned long srclbl = 0 ; srclbl < Samples.size() ; srclbl++) {
		const list<SampleLabel> &oppneighs = SampleOppositeNeighbourhood[srclbl] ;
		for (list<SampleLabel>::const_iterator destlbl = oppneighs.begin() ; destlbl != oppneighs.end() ; destlbl++) {
			Bit srcbit = Samples[srclbl]->getBit() ;
			Bit destbit = Samples[*destlbl]->getBit() ;
			if (srcbit == destbit) {
				retval = false ;
				cerr << "FAILED: SampleOppositeNeighbourhood contains a non-opposite sample" << endl ;
				break ;
			}
		}
	}

	cerr << "checking SampleOppositeNeighbourhood: all oppneighs are in this list" << endl ;
	for (unsigned long i = 0 ; i < Samples.size() ; i++) {
		for (unsigned long j = 0 ; j < Samples.size() ; j++) {
			if (Samples[i]->getBit() != Samples[j]->getBit()) {
				// they are opposite...
				if (Samples[i]->isNeighbour (Samples[j])) {
					// ...and they are neighbours => there must be an entry in SampleOppositeNeighbourhood
					assert (Samples[j]->isNeighbour (Samples[i])) ;
					const list<SampleLabel> &oppneighs = SampleOppositeNeighbourhood[i] ;
					bool found = false ;
					for (list<SampleLabel>::const_iterator k = oppneighs.begin() ; k != oppneighs.end() ; k++) {
						if (*k == j) {
							found = true ;
						}
					}
					if (!found) {
						retval = false ;
						cerr << "FAILED: SampleOppositeNeighbourhood does not contain all opposite neighbours" << endl ;
					}
				}
			}
		}
	}

	return retval ;
}

bool Graph::check_vertexcontents (void) const
{
	bool retval = true ;

	cerr << "checking VertexContents: vcontent sample labels are pointer equivalent to those in Samples" << endl ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vcontent = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleLabel lbl = vcontent->getSampleLabel (j) ;
			CvrStgSample *s = (*i)->getSample (j) ;
			if (Samples[lbl] != s) { // pointer equivalence
				retval = false ;
				cerr << "FAILED: vertex and it's content not consistent" << endl ;
				break ;
			}
		}
	}

	cerr << "checking VertexContents: each vertex content of a vector can be reached from all sample labels in VertexContents" << endl ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vcontent = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleLabel lbl = vcontent->getSampleLabel (j) ;
			bool found = false ;	// it should be possible to reach vcontent from all sample labels
			for (list<VertexContent*>::const_iterator k = VertexContents[lbl].begin() ; k != VertexContents[lbl].end() ; k++) {
				if ((*k) == vcontent) {
					found = true ;
				}
			}
			if (!found) {
				retval = false ;
				cerr << "FAILED: vertex content can not be reached" << endl ;
			}
		}
	}

	return retval ;
}

bool Graph::check_sampleoccurences (void) const
{
	bool retval = true ;

	unsigned long nsamples = File->getNumSamples() ;

	cerr << "checking SampleOccurences: every samplepos is a valid samplepos for this file" << endl ;
	for (unsigned long i = 0 ; i < Samples.size() ; i++) {
		for (map<SamplePos,pair<Vertex*,unsigned short> >::const_iterator j = SampleOccurences[i].begin() ; j != SampleOccurences[i].end() ; j++) {
			if (j->first >= nsamples) {
				retval = false ;
				cerr << "FAILED: invalid sample position" << endl ;
			}
		}
	}

	cerr << "checking SampleOccurences: every samplepos occurs not more than once" << endl ;
	vector<bool> hasoccured (nsamples, false) ;
	for (unsigned long i = 0 ; i < Samples.size() ; i++) {
		for (map<SamplePos,pair<Vertex*,unsigned short> >::const_iterator j = SampleOccurences[i].begin() ; j != SampleOccurences[i].end() ; j++) {
			if (hasoccured[j->first]) {
				retval = false ;
				cerr << "FAILED: a sample in SampleOccurences occurs two times" << endl ;
			}
			else {
				hasoccured[j->first] = true ;
			}
		}
	}

	return retval ;
}
#endif
