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

#include <hash_set>
#include <list>
#include <map>
#include <queue>
#include <vector>

#include "common.h"
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
	// move the hash_set Samples_set into the vector Samples, set sample labels
	// needs: filled Sample_set
	Samples = vector<CvrStgSample*> (Samples_set.size()) ;
	unsigned long label = 0 ;
	for (hash_set<CvrStgSample*,hash<CvrStgSample*>,SamplesEqual>::const_iterator i = Samples_set.begin() ; i != Samples_set.end() ; i++) {
		Samples[label] = *i ;
		Samples[label]->setLabel (label) ;
		label++ ;
	}

	// partition the samples into those with getBit()==1 and those with getBit()==0
	unsigned long nsamples = Samples.size() ;
	vector<CvrStgSample*> Samples0 = vector<CvrStgSample*>() ;
	vector<CvrStgSample*> Samples1 = vector<CvrStgSample*>() ;
	Samples0.reserve (nsamples / 2) ;
	Samples1.reserve (nsamples / 2) ;
	for (unsigned long label = 0 ; label < nsamples ; label++) {
		if (Samples[label]->getBit()) {
			Samples1.push_back (Samples[label]) ;
		}
		else {
			Samples0.push_back (Samples[label]) ;
		}
	}

	// create the SampleOppositeNeighbourhood data structure
	SampleOppositeNeighbourhood = vector<vector<SampleLabel> > (nsamples) ;
	unsigned long nsamples0 = Samples0.size() ;
	unsigned long nsamples1 = Samples1.size() ;
	for (unsigned long i0 = 0 ; i0 < nsamples0 ; i0++) {
		for (unsigned long i1 = 0 ; i1 < nsamples1 ; i1++) {
			if (Samples0[i0]->isNeighbour(Samples1[i1])) {
				unsigned long label0 = Samples0[i0]->getLabel() ;
				unsigned long label1 = Samples1[i1]->getLabel() ;
				SampleOppositeNeighbourhood[label0].push_back (label1) ;
				SampleOppositeNeighbourhood[label1].push_back (label0) ;
			}
		}
	}
	
	Samples_set.clear() ;
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
	if (v1->getDegree() == 0) {
		if (v1->getShortestEdge() != NULL) {
			delete v1->getShortestEdge() ;
		}
		v1->setShortestEdge (NULL) ;
	}
	else {
		bool found = false ;
		unsigned short v1_idx = 0 , v2_idx = 0 ;
		Vertex *v2 = NULL ;
		unsigned long min_weight = ULONG_MAX ;

		for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
			const vector<SampleLabel> &oppneighbours = SampleOppositeNeighbourhood[v1->getSample(i)->getLabel()] ;
			for (vector<SampleLabel>::const_iterator j = oppneighbours.begin() ; j != oppneighbours.end() ; j++) {
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
		Edge *e = new Edge (v1, v1_idx, v2, v2_idx, min_weight) ;
		replaceShortestEdge (v1, e) ;
	}
}

void Graph::replaceShortestEdge (Vertex *v1, Edge *e)
{
	Edge *oldedge_v1 = v1->getShortestEdge() ;
	if (oldedge_v1 != NULL) {
		Vertex *oldmate_v1 = oldedge_v1->getOtherVertex(v1) ;
		if (oldmate_v1->getShortestEdge() == oldedge_v1) {
			oldmate_v1->setShortestEdge (NULL) ;
		}
		delete oldedge_v1 ;
	}

	v1->setShortestEdge (e) ;
}

void Graph::replaceMatchingEdge (Vertex *v1, Vertex *v2, Edge *e)
{
	Edge *oldedge_v1 = v1->getMatchingEdge(), *oldedge_v2 = v2->getMatchingEdge() ;
	if (oldedge_v1 != NULL) {
		oldedge_v1->getOtherVertex(v1)->setMatchingEdge (NULL) ;
		delete oldedge_v1 ;
	}
	if (oldedge_v2 != NULL) {
		oldedge_v2->getOtherVertex(v2)->setMatchingEdge (NULL) ;
		delete oldedge_v2 ;
	}

	v1->setMatchingEdge (e) ;
	v2->setMatchingEdge (e) ;
}

void Graph::insertInMatching (vector<Edge*> *m, Edge *e)
{
	Vertex *v1 = e->getVertex1() ;
	Vertex *v2 = e->getVertex2() ;

	assert (!v1->isMatched() && !v2->isMatched()) ;

	replaceMatchingEdge (v1, v2, e) ;
	m->push_back (e) ;

	// delete samples of these vertices from SampleOccurences
	for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
		SampleOccurences[v1->getSample(i)->getLabel()].erase (v1->getSamplePos(i)) ;
		SampleOccurences[v2->getSample(i)->getLabel()].erase (v2->getSamplePos(i)) ;
	}

	// get all opposite neighbour samples of these two vertices
	vector<SampleLabel> oppneighbours ;	// duplicates are valid(!)
	for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
		// FIXME - rewrite this! - don't use copy - use references
		vector<SampleLabel> tmp = SampleOppositeNeighbourhood[v1->getSample(i)->getLabel()] ; // copy for Vertex1
		copy (tmp.begin(), tmp.end(), back_inserter(tmp)) ;
		tmp = SampleOppositeNeighbourhood[v2->getSample(i)->getLabel()] ;	// copy for Vertex2
		copy (tmp.begin(), tmp.end(), back_inserter(tmp)) ;
	}

	// for all neighbour samples decrement the degree of all vertex contents
	for (vector<SampleLabel>::iterator i = oppneighbours.begin() ; i != oppneighbours.end() ; i++) {
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

void Graph::printVOutputVertices()
{
#ifdef DEBUG
	if (Args.Verbosity.getValue() == VERBOSE || Args.DebugCommand.getValue() == PRINTSTATS) {
#else
	if (Args.Verbosity.getValue() == VERBOSE) {
#endif
		VerboseMessage vmsg1 (_("number of distinct samples: %lu"), Samples.size()) ;
		vmsg1.printMessage() ;

		VerboseMessage vmsg2 (_("number of vertices: %lu"), Vertices.size()) ;
		vmsg2.printMessage() ;

#ifdef DEBUG
		if (Args.DebugCommand.getValue() == PRINTSTATS) {
			printf ("%lu:%lu:",
					(unsigned long) Samples.size(),	// number of distinct samples
					(unsigned long) Vertices.size()	// number of vertices
				   ) ;
		}
#endif
	}

}

void Graph::printVOutputEdges()
{
#ifdef DEBUG
	if (Args.Verbosity.getValue() == VERBOSE || Args.DebugCommand.getValue() == PRINTSTATS) {
#else
	if (Args.Verbosity.getValue() == VERBOSE) {
#endif
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
		float avgdeg = ((float) sumdeg / (float) Vertices.size()) ;

		VerboseMessage vmsg3 (_("average vertex degree: %.1f"), avgdeg) ;
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

#ifdef DEBUG
		if (Args.DebugCommand.getValue() == PRINTSTATS) {
			unsigned long nvertexcontents = 0 ;
			for (vector<list<VertexContent*> >::iterator i = VertexContents.begin() ; i != VertexContents.end() ; i++) {
				nvertexcontents += i->size() ; // is linear
			}
			assert (nvertexcontents % SamplesPerEBit == 0) ;
			nvertexcontents /= SamplesPerEBit ;

			printf ("%lu:%lu:%lu:%.1f:%lu:",
					sumdeg / 2, // number of edges
					mindeg, // minimum vertex degree
					maxdeg, // maximum vertex degree
					avgdeg, // average vertex degree
					nvertexcontents // number of distinct vertex contents
				   ) ;
		}
#endif
	}
}

void Graph::printVOutputMatching (vector<Edge*> *m)
{
#ifdef DEBUG
	if (Args.Verbosity.getValue() == VERBOSE || Args.DebugCommand.getValue() == PRINTSTATS) {
#else
	if (Args.Verbosity.getValue() == VERBOSE) {
#endif
		VerboseMessage vmsg8 (_("size of the matching: %lu"), m->size()) ;
		vmsg8.printMessage() ;
		unsigned long numvertices = Vertices.size() - (2 * m->size()) ;
		VerboseMessage vmsg9 (_("number of unmatched vertices: %lu (%.1f%)"), numvertices, 100.0 * ((float) numvertices / (float) Vertices.size())) ;
		vmsg9.printMessage() ;

		unsigned long sumweights = 0 ;
		for (vector<Edge*>::iterator i = m->begin() ; i != m->end() ; i++) {
			sumweights += (*i)->getWeight() ;
		}

		VerboseMessage vmsg10 (_("average edge weight: %.1f"), ((float) sumweights / (float) m->size())) ;
		vmsg10.printMessage() ;

#ifdef DEBUG
		if (Args.DebugCommand.getValue() == PRINTSTATS) {
			printf ("%.3f:%.1f:",
					((float) numvertices / (float) Vertices.size()), // ratio of unmatched vertices
					((float) sumweights / (float) m->size()) // average edge weight
				   ) ;
		}
#endif
	}
}

void Graph::calcMatching()
{
	vector<Edge*> *bestmatching = new vector<Edge*>(), *curmatching = NULL ;
	unsigned int nconstrheur = NConstrHeur ;
#ifdef DEBUG
	if (Args.NConstrHeur.is_set()) {
		nconstrheur = Args.NConstrHeur.getValue() ;
	}
#endif

	printVOutputVertices() ;

	bool firsttime = true ;
	for (unsigned int i = 0 ; i < nconstrheur ; i++) {
		setupConstrHeuristic() ;
		if (firsttime) {
			printVOutputEdges() ;
			firsttime = false ;
		}
		curmatching = doConstrHeuristic() ;
		if (curmatching->size() > bestmatching->size()) {
			delete bestmatching ;
			bestmatching = curmatching ;
		}
	}

	printVOutputMatching (bestmatching) ;

#ifdef DEBUG
	//check_matching (bestmatching) ;
#endif
}

//
// Construction Heuristic (modified Karp&Sisper)
//
void Graph::setupConstrHeuristic()
{
	// initializations
	SampleOccurences = vector<map<SamplePos,pair<Vertex*,unsigned short> > > (Samples.size()) ;
	VertexContents = vector<list<VertexContent*> > (Samples.size()) ;

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
			const vector<SampleLabel> &oppneighbours = SampleOppositeNeighbourhood[(*i)->getSampleLabel(j)] ;
			for (vector<SampleLabel>::const_iterator k = oppneighbours.begin() ; k != oppneighbours.end() ; k++) {
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
		(*i)->setMatchingEdge (NULL) ; // FIXME - only needed after matching has been done at least once
	}

	// fill the VerticesDeg1 and VerticesDegG priority queues
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


#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTGRAPH) {
		print() ;
		exit (EXIT_SUCCESS) ;
	}
#endif

#if 0
	cerr << "checking data structures:" << endl ;
	assert (check_ds()) ;
	// FIXME - wenn alle bugs draußen - einmal bbtest damit durchlaufen lassen
#endif
	
}

Vertex *Graph::findVertexDeg1 (unsigned int k)
{
	Vertex *v = NULL ;
	vector<Vertex*> topk ;
	bool usethisvertex = false ;

	// get the vertex that is the k-nearest to top and still has degree 1
	do {
		assert (!VerticesDeg1.empty()) ;
		v = VerticesDeg1.top() ;
		VerticesDeg1.pop() ;
		assert (v->getDegree() <= 1) ;

		if ((v->getDegree() == 1) && (!v->isMatched())) { // implicitly delete vertices that have degree zero or have already been matched
			// v is valid vertex
			updateShortestEdge (v) ; // FIXME - wozu ?
			if (topk.size() == k - 1) {
				for (unsigned int i = 0 ; i < k - 1 ; i++) {
					VerticesDeg1.push (topk[i]) ;
				}
				usethisvertex = true ;	// v is the vertex k-nearest to top with degree 1
			}
			else {
				topk.push_back (v) ;
			}
		}

		if (k != 1 && VerticesDeg1.empty()) {	// there were less than k vertices with degree 1 in VerticesDegree1
			assert (!usethisvertex) ;
			unsigned int nfound = topk.size() ;
			if (nfound > 0) {
				usethisvertex = true ;
				v = topk[nfound - 1] ;
				for (unsigned int i = 0 ; i < nfound - 1 ; i++) {
					VerticesDeg1.push (topk[i]) ;
				}
			}
		}
	} while (!(usethisvertex || VerticesDeg1.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

Vertex *Graph::findVertexDegG (unsigned int k)
{
	Vertex *v = NULL ;
	vector<Vertex*> topk ;
	bool usethisvertex = false ;

	// get the vertex that is the k-nearest to top (with updated len of shortest edge) and has degree > 1
	do {
		assert (!VerticesDegG.empty()) ;
		v = VerticesDegG.top() ;
		VerticesDegG.pop() ;

		if ((v->getDegree() > 1) && (!v->isMatched())) { // implicitly delete vertices that have been moved to VerticesDeg1 or have already been matched
			// v is valid vertex
			unsigned long weight_before = v->getShortestEdge()->getWeight() ;
			updateShortestEdge (v) ;
			if (v->getShortestEdge()->getWeight() == weight_before) {
				if (topk.size() == k - 1) {
					for (unsigned int i = 0 ; i < k - 1 ; i++) {
						VerticesDegG.push (topk[i]) ;
					}
					usethisvertex = true ;
				}
				else {
					topk.push_back (v) ;
				}
			}
			else {
				assert (v->getShortestEdge()->getWeight() > weight_before) ;	// weight can only rise
				VerticesDegG.push (v) ; // push v into a position that is further away from top
			}
		}

		if (k != 1 && VerticesDegG.empty()) { // there were less than k valid vertices in VerticesDegree1
			assert (!usethisvertex) ;
			unsigned int nfound = topk.size() ;
			if (nfound > 0) {
				usethisvertex = true ;
				v = topk[nfound - 1] ;
				for (unsigned int i = 0 ; i < nfound - 1 ; i++) {
					VerticesDegG.push (topk[i]) ;
				}
			}
		}
	} while (!(usethisvertex || VerticesDegG.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

vector<Edge*> *Graph::doConstrHeuristic()
{
	vector<Edge*> *Matching = new vector<Edge*> ;
	unsigned int pqr = PriorityQueueRange ;
#ifdef DEBUG
	if (Args.PriorityQueueRange.is_set()) {
		pqr = Args.PriorityQueueRange.getValue() ;
	}
#endif

	while (!(VerticesDegG.empty() && VerticesDeg1.empty())) {
		Vertex *v = NULL ;
		unsigned int k = 1 ;
		if (pqr > 1) {
			k = RndSrc.getValue (pqr) + 1 ;
		}

		if (!VerticesDeg1.empty()) {
			v = findVertexDeg1 (k) ;
		}
		else {
			v = findVertexDegG (k) ;
		}

		if (v != NULL) {
			insertInMatching (Matching, v->getShortestEdge()) ;
		}
	}

	return Matching ;
}

#ifdef DEBUG
void Graph::print (void) const
{
	unsigned long sumdeg = 0 ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		sumdeg += (*i)->getDegree() ;
	}
	assert (sumdeg % 2 == 0) ;
	cout << Vertices.size() << " " << (sumdeg / 2) << " U" << endl ;

	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		cout << Vertices[i]->getDegree() << " " << (i + 1) << " 0 0" << endl ;

		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			CvrStgSample *srcsample = Vertices[i]->getSample(j) ;
			for (unsigned long k = 0 ; k != Vertices.size() ; k++) {
				if (i != k) { // no loops
					for (unsigned short l = 0 ; l < SamplesPerEBit ; l++) {
						CvrStgSample *destsample = Vertices[k]->getSample(l) ;
						if ((srcsample->isNeighbour(destsample)) && (srcsample->getBit() != destsample->getBit())) { // is opposite neighbour
							cout << (k + 1) << " 0" << endl ;
						}
					}
				}
			}

		}
	}
}

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

bool Graph::check_matching (vector<Edge*> *m) const
{
	bool retval = true ;

	cerr << "checking if vector<Edge*> Matching is a matching (without loops)" << endl ;
	vector<bool> inmatching (Vertices.size(), false) ;
	for (vector<Edge*>::const_iterator i = m->begin() ; i != m->end() ; i++) {
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
		const vector<SampleLabel> &oppneighs = SampleOppositeNeighbourhood[srclbl] ;
		for (vector<SampleLabel>::const_iterator destlbl = oppneighs.begin() ; destlbl != oppneighs.end() ; destlbl++) {
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
					const vector<SampleLabel> &oppneighs = SampleOppositeNeighbourhood[i] ;
					bool found = false ;
					for (vector<SampleLabel>::const_iterator k = oppneighs.begin() ; k != oppneighs.end() ; k++) {
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
