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

#include <list>
#include <map>
#include <queue>
#include <vector>

#include "bmpsamplevalue.h"
#include "common.h"
#include "cvrstgfile.h"
#include "graph.h"
#include "msg.h"
#include "permutation.h"
#include "vertex.h"
#include "wrapper_hash_set.h"

Graph::Graph (CvrStgFile *cvr, const BitString& emb)
{
	VerboseMessage v (_("creating the graph...")) ;
	v.printMessage() ;

	File = cvr ;
	SamplesPerEBit = File->getSamplesPerEBit() ;

	// construct sposs
	std::vector<SamplePos*> sposs ;
	Permutation perm (File->getNumSamples(), Args.Passphrase.getValue()) ;
	unsigned long n = emb.getLength() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SamplePos *poss = new SamplePos[SamplesPerEBit] ;
		Bit parity = 0 ;
		for (unsigned int j = 0 ; j < SamplesPerEBit ; j++) {
			poss[j] = *perm ;
			parity ^= File->getSampleBit (*perm) ;
			++perm ;
		}

		if (parity != emb[i]) {
			sposs.push_back (poss) ;
		}
		else {
			delete[] poss ;
		}
	}

	std::vector<SampleValue**> svalues ;
	sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual> vc_set ;
	constructSamples (sposs, svalues) ;
	constructVertices (sposs, svalues, vc_set) ;
	constructEdges (vc_set) ;

	// free svalues
	unsigned long nvertices = sposs.size() ;
	for (unsigned long i = 0 ; i < nvertices ; i++) {
		delete[] svalues[i] ;
	}
}

void Graph::constructSamples (const std::vector<SamplePos*>& sposs,
	std::vector<SampleValue**>& svalues)
{
	const VertexLabel numvertices = sposs.size() ;
	svalues.clear() ;
	svalues.resize (numvertices) ;

	// fill a hash table with the (unique) sample values
	sgi::hash_set<SampleValue*,sgi::hash<SampleValue*>,SampleValuesEqual> SampleValues_set ;
	for (unsigned long i = 0 ; i < numvertices ; i++) {
		svalues[i] = new SampleValue*[SamplesPerEBit] ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValue *sv = File->getSampleValue (sposs[i][j]) ;
			sgi::hash_set<SampleValue*,sgi::hash<SampleValue*>,SampleValuesEqual>::iterator res = SampleValues_set.find (sv) ;
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

	// move the sgi::hash_set SampleValues_set into the std::vector SampleValues, set sample labels
	SampleValues = std::vector<SampleValue*> (SampleValues_set.size()) ;
	unsigned long label = 0 ;
	for (sgi::hash_set<SampleValue*,sgi::hash<SampleValue*>,SampleValuesEqual>::const_iterator i = SampleValues_set.begin() ; i != SampleValues_set.end() ; i++) {
		SampleValues[label] = *i ;
		SampleValues[label]->setLabel (label) ;
		label++ ;
	}
}

void Graph::constructVertices (std::vector<SamplePos*>& sposs, std::vector<SampleValue**>& svalues,
	sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>& vc_set)
{
	const VertexLabel numvertices = sposs.size() ;
	Vertices = std::vector<Vertex*> (numvertices) ;
	vc_set = sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>() ;
	VertexContents = std::vector<std::list<VertexContent*> > (SampleValues.size()) ;
	for (SampleValueLabel svlbl = 0 ; svlbl < SampleValues.size() ; svlbl++) {
		VertexContents[svlbl] = std::list<VertexContent*>() ;
	}

	for (VertexLabel i = 0 ; i < numvertices ; i++) {
		// has vertex content already been created ?
		VertexContent *vc = new VertexContent (this, svalues[i], sposs[i]) ;
		sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>::iterator res = vc_set.find (vc) ;
		if (res == vc_set.end()) { // vc has not been found - add it!
			vc_set.insert (vc) ;
			for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
				SampleValue *sv = vc->getSampleValue(j) ;
				SampleValueLabel svlbl = sv->getLabel() ;
				VertexContents[svlbl].push_back (vc) ;
			}
		}
		else { // vc is already there
			delete vc ;
			vc = *res ;
		}

		Vertices[i] = new Vertex (this, i, sposs[i], vc) ;
	}

#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTSTATS) {
		NumVertexContents = vc_set.size() ;
	}
#endif

	return ;
}

void Graph::constructEdges (const sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>& vc_set)
{
	// create SampleValueOppositeNeighbourhood
	SampleValueOppNeighs = SampleValueOppositeNeighbourhood (this, SampleValues) ;

	// fill SampleOccurences
	SampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	DeletedSampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	for (std::vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleOccurence occ (*it, j) ;
			SampleValueLabel lbl = (*it)->getSampleValue(j)->getLabel() ;
			std::list<SampleOccurence>::iterator occit = SampleOccurences[lbl].insert (SampleOccurences[lbl].end(), occ) ;
			(*it)->setSampleOccurenceIt (j, occit) ;
		}
	}

	// compute NumEdges for all sample values
	for (SampleValueLabel lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		unsigned long numedges = 0 ;
		unsigned long noppneighs = SampleValueOppNeighs[lbl].size() ;
		for (unsigned long i = 0 ; i < noppneighs ; i++) {
			// FIXME - .size() needs linear time!
			numedges += SampleOccurences[SampleValueOppNeighs[lbl][i]->getLabel()].size() ;
		}
		SampleValues[lbl]->setNumEdges (numedges) ;
	}
}

Graph::~Graph()
{
	for (std::vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		delete *i ;
	}
	// TODO - delete more...
}

void Graph::unmarkDeletedAllVertices ()
{
	for (std::vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		(*it)->unmarkDeleted() ;
	}
}

std::list<SampleOccurence>::iterator Graph::markDeletedSampleOccurence (std::list<SampleOccurence>::iterator it)
{
	Vertex *v = it->getVertex() ;
	unsigned short i = it->getIndex() ;
	SampleValueLabel lbl = v->getSampleValue(i)->getLabel() ;
	SampleOccurences[lbl].erase (it) ;
	return DeletedSampleOccurences[lbl].insert (DeletedSampleOccurences[lbl].end(), SampleOccurence (v, i)) ;
}

std::list<SampleOccurence>::iterator Graph::unmarkDeletedSampleOccurence (std::list<SampleOccurence>::iterator it)
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
		for (std::vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
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
		myassert (sumdeg % 2 == 0) ;

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

#ifdef DEBUG
void Graph::print (void) const
{
	unsigned long sumdeg = 0 ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		sumdeg += (*i)->getDegree() ;
	}
	myassert (sumdeg % 2 == 0) ;
	std::cout << Vertices.size() << " " << (sumdeg / 2) << " U" << std::endl ;

	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		std::cout << Vertices[i]->getDegree() << " " << (i + 1) << " 0 0" << std::endl ;

		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValue *srcsample = Vertices[i]->getSampleValue(j) ;
			for (unsigned long k = 0 ; k != Vertices.size() ; k++) {
				if (i != k) { // no loops
					for (unsigned short l = 0 ; l < SamplesPerEBit ; l++) {
						SampleValue *destsample = Vertices[k]->getSampleValue(l) ;
						if ((srcsample->isNeighbour(destsample)) && (srcsample->getBit() != destsample->getBit())) {
							// is opposite neighbour
							std::cout << (k + 1) << " 0" << std::endl ;
						}
					}
				}
			}

		}
	}
}

#if 0
// TODO - move to Matching
void Graph::printUnmatchedVertices (void) const
{
	unsigned long num = 0 ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		if (!(*i)->isMatched()) {
			num++ ;
			for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
				std::cerr << (*i)->getSample(j)->getKey() ;
				if (j + 1 != SamplesPerEBit) {
					std::cerr << "/" ;
				}
			}
			std::cerr << std::endl ;
		}
	}
	std::cerr << num << " unmatched vertices found." << std::endl ;
}
#endif

bool Graph::check_matching (std::vector<Edge*> *m) const
{
	bool retval = true ;

	std::cerr << "checking if std::vector<Edge*> Matching is a matching (without loops)" << std::endl ;
	std::vector<bool> inmatching (Vertices.size(), false) ;
	for (std::vector<Edge*>::const_iterator i = m->begin() ; i != m->end() ; i++) {
		VertexLabel v1lbl = (*i)->getVertex1()->getLabel() ;
		VertexLabel v2lbl = (*i)->getVertex2()->getLabel() ;
		if (v1lbl == v2lbl) {
			retval = false ;
			std::cerr << "FAILED: matching contains a loop" << std::endl ;
			break ;
		}
		if (inmatching[v1lbl]) {
			std::cerr << "FAILED: matching contains vertex with label " << v1lbl << " twice." << std::endl ;
			retval = false ;
			break ;
		}
		else {
			inmatching[v1lbl] = true ;
		}
		if (inmatching[v2lbl]) {
			std::cerr << "FAILED: matching contains vertex with label " << v2lbl << " twice." << std::endl ;
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
		const std::vector<SampleValue*> oppneighs = SampleValueOppNeighs[srcsample] ;
		for (std::vector<SampleValue*>::const_iterator it = oppneighs.begin() ; it != oppneighs.end() ; it++) {
			degree += SampleOccurences[(*it)->getLabel()].size() ;
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
	retval = check_SampleOccurences() && retval ;

	return retval ;
}

bool Graph::check_sizes (void) const
{
	std::cerr << "checking sizes" << std::endl ;
	unsigned long n = SampleValues.size() ;
	bool retval = (n == SampleOccurences.size()) ;
	if (!retval) {
		std::cerr << "FAILED: sizes don't match" << std::endl ;
	}
	return retval ;
}

bool Graph::check_samples (void) const
{
	bool retval = true ;
	
	unsigned long n = SampleValues.size() ;

	std::cerr << "checking sample values: index - label consistency" << std::endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SampleValueLabel l = SampleValues[i]->getLabel() ;
		if (l != i) {
			retval = false ;
			std::cerr << "FAILED: wrong sample label" << std::endl ;
			break ;
		}
	}

	std::cerr << "checking sample values: uniqueness of samples" << std::endl ;
	for (unsigned long i = 0 ; i < n ; i++) {
		for (unsigned long j = 0 ; j < n ; j++) {
			if (i != j) {
				if (SampleValues[i]->getKey() == SampleValues[j]->getKey()) {
					retval = false ;
					std::cerr << "FAILED: duplicate sample in SampleValues[" << i << "] and SampleValues[" << j << "]" << std::endl ;
				}
			}
		}
	}

	return retval ;
}

bool Graph::check_vertices (void) const
{
	bool retval = true ;
	std::cerr << "checking Vertices: index - label consistency" << std::endl ;
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		VertexLabel l = Vertices[i]->getLabel() ;
		if (l != i) {
			retval = false ;
			std::cerr << "FAILED: wrong vertex label" << std::endl ;
			break ;
		}
	}
	return retval ;
}

bool Graph::check_degrees (void) const
{
	bool retval = true ;

	std::cerr << "checking vertex degrees (real vs. stored)" << std::endl ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		unsigned long deg_real = check_degree(*i) ;
		unsigned long deg_stored = (*i)->getDegree() ;
		if (deg_real != deg_stored) {
			std::cerr << "check_degree == getDegree failed" << std::endl ;
			(*i)->print() ;
			std::cerr << "getDegree: " << deg_stored << std::endl ;
			std::cerr << "check_degree: " << deg_real << std::endl ;

			retval = false ;
			break ;
		}
	}

	return retval ;
}

bool Graph::check_sampleoppositeneighbourhood (void) const
{
	bool retval = true ;

	std::cerr << "checking SampleValueOppositeNeighbourhood: sample values are opposite" << std::endl ;
	for (SampleValueLabel srclbl = 0 ; srclbl < SampleValues.size() ; srclbl++) {
		const std::vector<SampleValue*> &oppneighs = SampleValueOppNeighs[srclbl] ;
		for (std::vector<SampleValue*>::const_iterator destsv = oppneighs.begin() ; destsv != oppneighs.end() ; destsv++) {
			Bit srcbit = SampleValues[srclbl]->getBit() ;
			Bit destbit = (*destsv)->getBit() ;
			if (srcbit == destbit) {
				retval = false ;
				std::cerr << "FAILED: SampleOppositeNeighbourhood contains a non-opposite sample" << std::endl ;
				break ;
			}
		}
	}

	// FIXME - also check all are neighbours

	std::cerr << "checking SampleValueOppositeNeighbourhood: all oppneighs are in this std::list" << std::endl ;
	for (unsigned long i = 0 ; i < SampleValues.size() ; i++) {
		for (unsigned long j = 0 ; j < SampleValues.size() ; j++) {
			if (SampleValues[i]->getBit() != SampleValues[j]->getBit()) {
				// they are opposite...
				if (SampleValues[i]->isNeighbour (SampleValues[j])) {
					// ...and they are neighbours => there must be an entry in SampleOppositeNeighbourhood
					myassert (SampleValues[j]->isNeighbour (SampleValues[i])) ;
					const std::vector<SampleValue*> &oppneighs = SampleValueOppNeighs[i] ;
					bool found = false ;
					for (std::vector<SampleValue*>::const_iterator k = oppneighs.begin() ; k != oppneighs.end() ; k++) {
						if ((*k)->getLabel() == j) {
							found = true ;
						}
					}
					if (!found) {
						retval = false ;
						std::cerr << "FAILED: SampleOppositeNeighbourhood does not contain all opposite neighbours" << std::endl ;
					}
				}
			}
		}
	}

	return retval ;
}

// FIXME - needs Vertex::getContent().... !?
bool Graph::check_vertexcontents (void) const
{
	bool retval = true ;
#if 0

	std::cerr << "checking VertexContents: vcontent sample labels are pointer equivalent to those in Samples" << std::endl ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vcontent = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValueLabel lbl = vcontent->getSampleValueLabel (j) ;
			SampleValue *s = (*i)->getSample (j) ;
			if (SampleValues[lbl] != s) { // pointer equivalence
				retval = false ;
				std::cerr << "FAILED: vertex and it's content not consistent" << std::endl ;
				break ;
			}
		}
	}

	std::cerr << "checking VertexContents: each vertex content of a std::vector can be reached from all sample labels in VertexContents" << std::endl ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vcontent = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValueLabel lbl = vcontent->getSampleValueLabel (j) ;
			bool found = false ;	// it should be possible to reach vcontent from all sample labels
			for (std::list<VertexContent*>::const_iterator k = VertexContents[lbl].begin() ; k != VertexContents[lbl].end() ; k++) {
				if ((*k) == vcontent) {
					found = true ;
				}
			}
			if (!found) {
				retval = false ;
				std::cerr << "FAILED: vertex content can not be reached" << std::endl ;
			}
		}
	}

#endif
	return retval ;
}

bool Graph::check_SampleOccurences (void) const
{
	bool retval = true ;
	std::cerr << "checking SampleOccurences:" << std::endl ;
	retval = check_SampleOccurences_correctness() && retval ;
	retval = check_SampleOccurences_completeness() && retval ;
	return retval ;
}

bool Graph::check_SampleOccurences_correctness (void) const
{
	bool err = false ;

	unsigned long numsvs = SampleValues.size() ;
	for (unsigned long lbl = 0 ; lbl < numsvs ; lbl++) {
		for (std::list<SampleOccurence>::const_iterator it = SampleOccurences[lbl].begin() ; it != SampleOccurences[lbl].end() ; it++) {
			Vertex *v = it->getVertex() ;
			unsigned short idx = it->getIndex() ;
			if (v->getSampleValue(idx) != SampleValues[lbl]) { // pointer equivalence
				err = true ;
			}
		}
	}

	if (err) {
		std::cerr << "FAILED: there exists a SampleOccurence in SampleOccurences which is not correct" << std::endl ;
	}

	return !err ;
}

bool Graph::check_SampleOccurences_completeness (void) const
{
	bool err = false ;

	unsigned long numvertices = Vertices.size() ;
	for (unsigned long vlbl = 0 ; vlbl < numvertices ; vlbl++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValueLabel svlbl = Vertices[vlbl]->getSampleValue(j)->getLabel() ;
			// SampleOccurence(v,j) must be in SampleOccurences[svlbl]
			bool found = false ;
			for (std::list<SampleOccurence>::const_iterator it = SampleOccurences[svlbl].begin() ; it != SampleOccurences[svlbl].end() ; it++) {
				if (it->getVertex() == Vertices[vlbl] && it->getIndex() == j) { // pointer equivalence
					found = true ;
					break ;
				}
			}
			if (!found) {
				err = true ;
			}
		}
	}

	if (err) {
		std::cerr << "FAILED: there is a vertex which is not fully represented in SampleOccurences" << std::endl ;
	}

	return !err ;

}

#if 0
// move to Matching - TODO
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
