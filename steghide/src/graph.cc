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

#include "bmpsamplevalue.h"
#include "common.h"
#include "cvrstgfile.h"
#include "graph.h"
#include "msg.h"
#include "vertex.h"

Graph::Graph (CvrStgFile *f, vector<SamplePos*>& sposs)
{
	File = f ;
	SamplesPerEBit = File->getSamplesPerEBit() ;

	vector<SampleValue**> svalues ;
	hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual> vc_set ;
	constructSamples (sposs, svalues) ;
	constructVertices (sposs, svalues, vc_set) ;
	constructEdges (vc_set) ;
}

void Graph::constructSamples (const vector<SamplePos*>& sposs,
	vector<SampleValue**>& svalues)
{
	const VertexLabel numvertices = sposs.size() ;
	svalues.clear() ;
	svalues.resize (numvertices) ;

	// fill a hash table with the (unique) sample values
	hash_set<SampleValue*,hash<SampleValue*>,SampleValuesEqual> SampleValues_set ;
	for (unsigned long i = 0 ; i < numvertices ; i++) {
		svalues[i] = new SampleValue*[SamplesPerEBit] ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValue *sv = File->getSampleValue (sposs[i][j]) ;
			hash_set<SampleValue*,hash<SampleValue*>,SampleValuesEqual>::iterator res = SampleValues_set.find (sv) ;
			if (res == SampleValues_set.end()) { // sample has not been found - add it !
				SampleValues_set.insert (sv) ;
				svalues[i][j] = sv ;
			}
			else { // sample value is already in SampleValues_set
				delete sv ;
				svalues[i][j] = *res ;
			} 
		}
	}

	// move the hash_set SampleValues_set into the vector SampleValues, set sample labels
	SampleValues = vector<SampleValue*> (SampleValues_set.size()) ;
	unsigned long label = 0 ;
	for (hash_set<SampleValue*,hash<SampleValue*>,SampleValuesEqual>::const_iterator i = SampleValues_set.begin() ; i != SampleValues_set.end() ; i++) {
		SampleValues[label] = *i ;
		SampleValues[label]->setLabel (label) ;
		label++ ;
	}
}

void Graph::constructVertices (vector<SamplePos*>& sposs, vector<SampleValue**>& svalues,
	hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>& vc_set)
{
	const VertexLabel numvertices = sposs.size() ;
#if 0
	VertexContents = vector<list<VertexContent*> > (SampleValues.size()) ;
#endif
	Vertices = vector<Vertex*> (numvertices) ;
	vc_set = hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>() ;

	for (VertexLabel i = 0 ; i < numvertices ; i++) {
		// has vertex content already been created ?
		VertexContent *vc = new VertexContent (this, svalues[i], sposs[i]) ;
		hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>::iterator res = vc_set.find (vc) ;
		if (res == vc_set.end()) { // vc has not been found - add it!
			vc_set.insert (vc) ;
		}
		else { // vc is already there
			delete vc ;
			vc = *res ;
		}

		// fill Vertices and VertexContents
		Vertices[i] = new Vertex (this, i, sposs[i], vc) ;
#if 0
		for (unsigned short j = 0 ; j < SamplesPerEBit ; i++) {
			VertexContents[vc->getSampleValue(j)->getLabel()].push_back (vc) ;
		}
#endif
	}

#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTSTATS) {
		NumVertexContents = vc_set.size() ;
	}
#endif
}

void Graph::constructEdges (const hash_set<VertexContent*,hash<VertexContent*>,VertexContentsEqual>& vc_set)
{
	// create SampleValueOppositeNeighbourhood
	SValueOppNeighs = SampleValueOppositeNeighbourhood (this, SampleValues) ;

	// fill SampleOccurences
	SampleOccurences = vector<list<SampleOccurence> > (SampleValues.size()) ;
	for (vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleOccurence occ (*it, j) ;
			SampleOccurences[(*it)->getSampleValue(j)->getLabel()].push_back (occ) ;
		}
	}

	// compute NumEdges for all sample values
	for (SampleValueLabel lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		unsigned long numedges = 0 ;
		unsigned long noppneighs = SValueOppNeighs[lbl].size() ;
		for (unsigned long i = 0 ; i < noppneighs ; i++) {
			// FIXME nc - .size() needs linear time!
			numedges += SampleOccurences[SValueOppNeighs[lbl][i]->getLabel()].size() ;
		}
		SampleValues[lbl]->setNumEdges (numedges) ;
	}
}


Graph::~Graph()
{
	for (vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		delete *i ;
	}
	// TODO nc - delete more...
}

void Graph::unmarkDeletedAllVertices ()
{
	for (vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		(*it)->unmarkDeleted() ;
	}
}

list<SampleOccurence>::iterator Graph::markDeletedSampleOccurence (list<SampleOccurence>::iterator it)
{
	Vertex *v = it->getVertex() ;
	unsigned short i = it->getIndex() ;
	SampleValueLabel lbl = v->getSampleValue(i)->getLabel() ;
	SampleOccurences[lbl].erase (it) ;
	return DeletedSampleOccurences[lbl].insert (DeletedSampleOccurences[lbl].end(), SampleOccurence (v, i)) ;
}

list<SampleOccurence>::iterator Graph::unmarkDeletedSampleOccurence (list<SampleOccurence>::iterator it)
{
	Vertex *v = it->getVertex() ;
	unsigned short i = it->getIndex() ;
	SampleValueLabel lbl = v->getSampleValue(i)->getLabel() ;
	DeletedSampleOccurences[lbl].erase (it) ;
	return SampleOccurences[lbl].insert (SampleOccurences[lbl].end(), SampleOccurence (v, i)) ;
}

void Graph::printVerboseInfo()
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
		assert (sumdeg % 2 == 0) ;

#ifdef DEBUG
		if (Args.DebugCommand.getValue() == PRINTSTATS) {
			printf ("%lu:%lu:%lu:%lu:%lu:%.1f:%lu:",
					(unsigned long) SampleValues.size(),	// number of distinct sample values
					(unsigned long) Vertices.size(),	// number of vertices
					sumdeg / 2, // number of edges
					mindeg, // minimum vertex degree
					maxdeg, // maximum vertex degree
					avgdeg, // average vertex degree
					NumVertexContents // number of distinct vertex contents
				   ) ;
		}
#endif

		if (Args.Verbosity.getValue() == VERBOSE) {
			VerboseMessage vmsg1 (_("number of distinct sample values: %lu"), SampleValues.size()) ;
			vmsg1.printMessage() ;

			VerboseMessage vmsg2 (_("number of vertices: %lu"), Vertices.size()) ;
			vmsg2.printMessage() ;

			VerboseMessage vmsg3 (_("average vertex degree: %.1f"), avgdeg) ;
			vmsg3.printMessage() ;
			VerboseMessage vmsg4 (_("minimum vertex degree: %lu"), mindeg) ;
			vmsg4.printMessage() ;
			VerboseMessage vmsg5 (_("maximum vertex degree: %lu"), maxdeg) ;
			vmsg5.printMessage() ;

			VerboseMessage vmsg6 (_("number of edges: %lu"), sumdeg / 2) ;
			vmsg6.printMessage() ;
		}
	}
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
			SampleValue *srcsample = Vertices[i]->getSampleValue(j) ;
			for (unsigned long k = 0 ; k != Vertices.size() ; k++) {
				if (i != k) { // no loops
					for (unsigned short l = 0 ; l < SamplesPerEBit ; l++) {
						SampleValue *destsample = Vertices[k]->getSampleValue(l) ;
						if ((srcsample->isNeighbour(destsample)) && (srcsample->getBit() != destsample->getBit())) {
							// is opposite neighbour
							cout << (k + 1) << " 0" << endl ;
						}
					}
				}
			}

		}
	}
}

#if 0
// TODO nc - move to Matching
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
#endif

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
		SampleValue *srcsample = v->getSampleValue(i) ;
		for (vector<Vertex*>::const_iterator j = Vertices.begin() ; j != Vertices.end() ; j++) {
			if ((*j)->getLabel() != v->getLabel()) { // no loops
				for (unsigned short k = 0 ; k < SamplesPerEBit ; k++) {
					SampleValue *destsample = (*j)->getSampleValue(k) ;
					if ((srcsample->isNeighbour(destsample)) && (srcsample->getBit() != destsample->getBit())) {
						// is opposite neighbour
						degree++ ;
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
	unsigned long n = SampleValues.size() ;
	bool retval = (n == SampleOccurences.size()) ;
	if (!retval) {
		cerr << "FAILED: sizes don't match" << endl ;
	}
	return retval ;
}

bool Graph::check_samples (void) const
{
	bool retval = true ;
	
	unsigned long n = SampleValues.size() ;

	cerr << "checking sample values: index - label consistency" << endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SampleValueLabel l = SampleValues[i]->getLabel() ;
		if (l != i) {
			retval = false ;
			cerr << "FAILED: wrong sample label" << endl ;
			break ;
		}
	}

	cerr << "checking sample values: uniqueness of samples" << endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		for (unsigned long j = 0 ; j < n ; j++) {
			if (i != j) {
				if (SampleValues[i]->getKey() == SampleValues[j]->getKey()) {
					retval = false ;
					cerr << "FAILED: duplicate sample in SampleValues[" << i << "] and SampleValues[" << j << "]" << endl ;
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

	cerr << "checking SampleValueOppositeNeighbourhood: sample values are opposite" << endl ;
	for (SampleValueLabel srclbl = 0 ; srclbl < SampleValues.size() ; srclbl++) {
		const vector<SampleValue*> &oppneighs = SValueOppNeighs[srclbl] ;
		for (vector<SampleValue*>::const_iterator destsv = oppneighs.begin() ; destsv != oppneighs.end() ; destsv++) {
			Bit srcbit = SampleValues[srclbl]->getBit() ;
			Bit destbit = (*destsv)->getBit() ;
			if (srcbit == destbit) {
				retval = false ;
				cerr << "FAILED: SampleOppositeNeighbourhood contains a non-opposite sample" << endl ;
				break ;
			}
		}
	}

	// FIXME nc - also check all are neighbours

	cerr << "checking SampleValueOppositeNeighbourhood: all oppneighs are in this list" << endl ;
	for (unsigned long i = 0 ; i < SampleValues.size() ; i++) {
		for (unsigned long j = 0 ; j < SampleValues.size() ; j++) {
			if (SampleValues[i]->getBit() != SampleValues[j]->getBit()) {
				// they are opposite...
				if (SampleValues[i]->isNeighbour (SampleValues[j])) {
					// ...and they are neighbours => there must be an entry in SampleOppositeNeighbourhood
					assert (SampleValues[j]->isNeighbour (SampleValues[i])) ;
					const vector<SampleValue*> &oppneighs = SValueOppNeighs[i] ;
					bool found = false ;
					for (vector<SampleValue*>::const_iterator k = oppneighs.begin() ; k != oppneighs.end() ; k++) {
						if ((*k)->getLabel() == j) {
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

// FIXME nc - needs Vertex::getContent().... !?
bool Graph::check_vertexcontents (void) const
{
	bool retval = true ;
#if 0

	cerr << "checking VertexContents: vcontent sample labels are pointer equivalent to those in Samples" << endl ;
	for (vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vcontent = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValueLabel lbl = vcontent->getSampleValueLabel (j) ;
			SampleValue *s = (*i)->getSample (j) ;
			if (SampleValues[lbl] != s) { // pointer equivalence
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
			SampleValueLabel lbl = vcontent->getSampleValueLabel (j) ;
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

#endif
	return retval ;
}

// FIXME nc - fix this
bool Graph::check_sampleoccurences (void) const
{
	bool retval = true ;
#if 0

	unsigned long nsamples = File->getNumSamples() ;

	cerr << "checking SampleOccurences: every samplepos is a valid samplepos for this file" << endl ;
	for (unsigned long i = 0 ; i < SampleValues.size() ; i++) {
		for (map<SamplePos,pair<Vertex*,unsigned short> >::const_iterator j = SampleOccurences[i].begin() ; j != SampleOccurences[i].end() ; j++) {
			if (j->first >= nsamples) {
				retval = false ;
				cerr << "FAILED: invalid sample position" << endl ;
			}
		}
	}

	cerr << "checking SampleOccurences: every samplepos occurs not more than once" << endl ;
	vector<bool> hasoccured (nsamples, false) ;
	for (unsigned long i = 0 ; i < SampleValues.size() ; i++) {
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

#endif
	return retval ;
}

#if 0
// move to Matching - TODO nc
void Graph::printUnmatchedVerticescontaining (unsigned long samplekey) const
{
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		for (unsigned int j = 0 ; j < SamplesPerEBit ; j++) {
			if (!Vertices[i]->isMatched() && Vertices[i]->getSample(j)->getKey() == samplekey) {
				std::cerr << Vertices[i]->getLabel() << " at pos " << j << std::endl ;
			}
		}
	}
}
#endif
#endif // def DEBUG
