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
#include <vector>

#include "BitString.h"
#include "CvrStgFile.h"
#include "Graph.h"
#include "Permutation.h"
#include "Vertex.h"
#include "common.h"
#include "msg.h"
#include "wrapper_hash_set.h"

Graph::Graph (CvrStgFile *cvr, const BitString& emb, const Permutation& perm)
{
	VerboseMessage v (_("creating the graph...")) ;
	v.printMessage() ;

	File = cvr ;
	SamplesPerEBit = File->getSamplesPerEBit() ;

	// construct sposs
	std::vector<SamplePos*> sposs ;
	unsigned long n = emb.getLength() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SamplePos *poss = new SamplePos[SamplesPerEBit] ;
		BIT parity = 0 ;
		for (unsigned int j = 0 ; j < SamplesPerEBit ; j++) {
			poss[j] = perm[(i * SamplesPerEBit) + j] ;
			parity ^= File->getSampleBit (poss[j]) ;
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

	// move the hash_set SampleValues_set into the vector SampleValues, set sample labels
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
}

void Graph::constructEdges (const sgi::hash_set<VertexContent*,sgi::hash<VertexContent*>,VertexContentsEqual>& vc_set)
{
	// create SampleValueOppositeNeighbourhood
	SampleValueOppNeighs = SampleValueOppositeNeighbourhood (this, SampleValues) ;

	// fill SampleOccurences
	SampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	std::vector<unsigned long> NumSampleOccurences (SampleValues.size()) ; // save number of sample occurences temporarily

	DeletedSampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	for (std::vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleOccurence occ (*it, j) ;
			SampleValueLabel lbl = (*it)->getSampleValue(j)->getLabel() ;
			std::list<SampleOccurence>::iterator occit = SampleOccurences[lbl].insert (SampleOccurences[lbl].end(), occ) ;
			NumSampleOccurences[lbl]++ ;
			(*it)->setSampleOccurenceIt (j, occit) ;
		}
	}

	// compute NumEdges for all sample values
	for (SampleValueLabel lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		unsigned long numedges = 0 ;
		unsigned long noppneighs = SampleValueOppNeighs[lbl].size() ;
		for (unsigned long i = 0 ; i < noppneighs ; i++) {
			numedges += NumSampleOccurences[SampleValueOppNeighs[lbl][i]->getLabel()] ;
		}
		SampleValues[lbl]->setNumEdges (numedges) ;
	}
}

Graph::~Graph()
{
	for (std::vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		delete *i ;
	}
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

bool Graph::check (bool verbose) const
{
	bool retval = true ;
	retval = check_Vertices(verbose) && retval ;
	retval = check_SampleValues(verbose) && retval ;
	retval = check_VertexContents(verbose) && retval ;
	retval = check_SampleOccurences(verbose) && retval ;
	retval = SampleValueOppNeighs.check(verbose) && retval ;
	return retval ;
}

bool Graph::check_Vertices (bool verbose) const
{
	bool label_consistency = true ;
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		label_consistency = (Vertices[i]->getLabel() == i) && label_consistency ;
	}
	return label_consistency ;
}

bool Graph::check_SampleValues (bool verbose) const
{
	unsigned long n = SampleValues.size() ;

	bool label_consistency = true ;
	for (unsigned long i = 0 ; i < n ; i++) {
		label_consistency = (SampleValues[i]->getLabel() == i) && label_consistency ;
	}

	bool sv_uniqueness = true ;
	for (unsigned long i = 0 ; i < n ; i++) {
		for (unsigned long j = 0 ; j < n ; j++) {
			if (i != j) {
				sv_uniqueness = (*(SampleValues[i]) != *(SampleValues[j])) && sv_uniqueness ;
			}
		}
	}

	return (label_consistency && sv_uniqueness) ;
}

bool Graph::check_VertexContents (bool verbose) const
{
	bool retval = true ;
	retval = check_VertexContents_size (verbose) && retval ;
	retval = check_VertexContents_soundness (verbose) && retval ;
	retval = check_VertexContents_completeness (verbose) && retval ;
	retval = check_VertexContents_pointerequiv (verbose) && retval ;
	retval = check_VertexContents_degrees (verbose) && retval ;
	return retval ;
}

bool Graph::check_VertexContents_size (bool verbose) const
{
	bool size = (VertexContents.size() == SampleValues.size()) ;
	if (!size && verbose) {
		std::cerr << std::endl << "---- FAILED: check_VertexContents_size ----" << std::endl ;
		std::cerr << "VertexContents.size(): " << VertexContents.size() << std::endl ;
		std::cerr << "SampleValues.size(): " << SampleValues.size() << std::endl ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	return size ;
}

bool Graph::check_VertexContents_soundness (bool verbose) const
{
	bool soundness = true ;

	// check for all sample value labels lbl that every vertex content
	// in VertexContents[lbl] contains a sample with the label lbl
	for (unsigned long lbl = 0 ; lbl < VertexContents.size() ; lbl++) {
		for (std::list<VertexContent*>::const_iterator vcit = VertexContents[lbl].begin() ; vcit != VertexContents[lbl].end() ; vcit++) {
			bool found = false ;
			for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
				if ((*vcit)->getSampleValue(j)->getLabel() == lbl) {
					found = true ;
				}
			}
			soundness = found && soundness ;
		}
	}

	return soundness ;
}

bool Graph::check_VertexContents_completeness (bool verbose) const
{
	bool completeness = true ;

	// check that every vertex content (as taken from Vertices) can be reached
	// in every row of VertexContents that is indexed with one of its sample values
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		VertexContent *vc = (*i)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValueLabel lbl = vc->getSampleValue(j)->getLabel() ;
			bool found = false ;
			for (std::list<VertexContent*>::const_iterator k = VertexContents[lbl].begin() ; k != VertexContents[lbl].end() ; k++) {
				if ((*k) == vc) {
					found = true ;
				}
			}
			completeness = found && completeness ;
		}
	}

	return completeness ;
}

bool Graph::check_VertexContents_pointerequiv (bool verbose) const
{
	bool pointerequiv = true ;

	for (std::vector<Vertex*>::const_iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		VertexContent *vc = (*vit)->getContent() ;
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleValue* s = vc->getSampleValue(j) ;
			SampleValueLabel lbl = s->getLabel() ;
			pointerequiv = (SampleValues[lbl] == s) && pointerequiv ;
		}
	}

	return pointerequiv ;
}

bool Graph::check_VertexContents_degrees (bool verbose) const
{
	bool degrees = true ;

	// checking if the vertex degrees stored in contents are equal to those
	// calculated from the SampleOccurences data structure
	for (std::vector<Vertex*>::const_iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		VertexContent *vc = (*vit)->getContent() ;

		unsigned long degree_socc = 0 ;
		for (unsigned short i = 0 ; i < SamplesPerEBit ; i++) {
			SampleValue *srcsample = vc->getSampleValue(i) ;
			const std::vector<SampleValue*>& oppneighs = SampleValueOppNeighs[srcsample] ;
			for (std::vector<SampleValue*>::const_iterator nit = oppneighs.begin() ; nit != oppneighs.end() ; nit++) {
				for (std::list<SampleOccurence>::const_iterator occit = SampleOccurences[(*nit)->getLabel()].begin() ;
						occit != SampleOccurences[(*nit)->getLabel()].end() ; occit++) {
					if (occit->getVertex()->getLabel() != (*vit)->getLabel()) {
						degree_socc++ ;
					}
				}
			}
		}

		if (degree_socc != (*vit)->getDegree()) {
			degrees = false ;
			if (verbose) {
				std::cerr << std::endl << "---- FAILED: check_VertexContents_degrees ----" << std::endl ;
				std::cerr << "real degree (as calculated from SampleOccurences): " << degree_socc << std::endl ;
				std::cerr << "stored degree (from VertexContent::getDegree): " << (*vit)->getDegree() << std::endl ;
				std::cerr << "-------------------------------------" << std::endl ;
			}
		}
	}

	return degrees ;
}

bool Graph::check_SampleOccurences (bool verbose) const
{
	bool retval = true ;
	retval = check_SampleOccurences_size (verbose) && retval ;
	retval = check_SampleOccurences_correctness (verbose) && retval ;
	retval = check_SampleOccurences_completeness (verbose) && retval ;
	return retval ;
}

bool Graph::check_SampleOccurences_size (bool verbose) const
{
	bool size = (SampleOccurences.size() == SampleValues.size()) ;
	if (!size && verbose) {
		std::cerr << std::endl << "---- FAILED: check_SampleOccurences_size ----" << std::endl ;
		std::cerr << "SampleOccurences.size(): " << SampleOccurences.size() << std::endl ;
		std::cerr << "SampleValues.size(): " << SampleValues.size() << std::endl ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	return size ;
}

bool Graph::check_SampleOccurences_correctness (bool verbose) const
{
	bool correctness = true ;

	for (unsigned long lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		for (std::list<SampleOccurence>::const_iterator socit = SampleOccurences[lbl].begin() ; socit != SampleOccurences[lbl].end() ; socit++) {
			Vertex *v = socit->getVertex() ;
			unsigned short idx = socit->getIndex() ;
			correctness = (v->getSampleValue(idx) == SampleValues[lbl]) && correctness ; // pointer equivalence
		}
	}

	return correctness ;
}

bool Graph::check_SampleOccurences_completeness (bool verbose) const
{
	bool completeness = true ;

	for (unsigned long vlbl = 0 ; vlbl < Vertices.size() ; vlbl++) {
		for (unsigned short j = 0 ; j < SamplesPerEBit ; j++) {
			SampleOccurence soc (Vertices[vlbl], j) ;
			SampleValueLabel svlbl = Vertices[vlbl]->getSampleValue(j)->getLabel() ;
			completeness = (find(SampleOccurences[svlbl].begin(), SampleOccurences[svlbl].end(), soc) != SampleOccurences[svlbl].end()) && completeness ;
		}
	}

	return completeness ;
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

void Graph::print_Vertices (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "Vertices:" << std::endl ;
	for (std::vector<Vertex*>::const_iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		(*vit)->print(spc + 1) ;
	}
}
#endif
