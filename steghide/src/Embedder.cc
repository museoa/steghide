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

#include <cfloat>
#include <cstdlib>

#include "AugmentingPathHeuristic.h"
#include "BitString.h"
#include "common.h"
#include "ConstructionHeuristic.h"
#include "CvrStgFile.h"
#include "EmbData.h"
#include "Embedder.h"
#include "error.h"
#include "Matching.h"
#include "msg.h"
#include "Permutation.h"
#include "Vertex.h"

Embedder::Embedder ()
{
	// create bitstring to be embedded
	EmbData embdata (EmbData::EMBED, Args.EmbFn.getValue()) ;
	embdata.setEncAlgo (Args.EncAlgo.getValue()) ;
	embdata.setEncMode (Args.EncMode.getValue()) ;
	embdata.setCompression (false) ;
	embdata.setChecksum (Args.Checksum.getValue()) ;
	ToEmbed = embdata.getBitString() ;

	// read cover-/stego-file
	TheCvrStgFile = CvrStgFile::readFile (Args.CvrFn.getValue()) ;
	
	if ((ToEmbed.getLength() * TheCvrStgFile->getSamplesPerEBit()) > TheCvrStgFile->getNumSamples()) {
		throw SteghideError (_("the cover file is too short to embed the data.")) ;
	}

	// create graph
	Permutation perm (TheCvrStgFile->getNumSamples(), Args.Passphrase.getValue(),
		(ToEmbed.getLength() * TheCvrStgFile->getSamplesPerEBit())) ;
	TheGraph = new Graph (TheCvrStgFile, ToEmbed, perm) ;
	TheGraph->printVerboseInfo() ;

#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTGRAPH) {
		TheGraph->print() ;
		exit (EXIT_SUCCESS) ;
	}
#endif
}

Embedder::~Embedder ()
{
	delete TheCvrStgFile ;
	delete TheGraph ;
}
	
void Embedder::embed ()
{
	const Matching* M = calculateMatching() ;

	// embed matched edges
	const std::list<Edge*> medges = M->getEdges() ;
	for (std::list<Edge*>::const_iterator it = medges.begin() ; it != medges.end() ; it++) {
		embedEdge (*it) ;
	}

	// embed exposed vertices
	const std::list<Vertex*> *expvertices = M->getExposedVerticesLink() ;
	for (std::list<Vertex*>::const_iterator it = expvertices->begin() ; it != expvertices->end() ; it++) {
		embedExposedVertex (*it) ;
	}

	delete M ;

	TheCvrStgFile->transform (Args.StgFn.getValue()) ;
	TheCvrStgFile->write() ;
}

const Matching* Embedder::calculateMatching ()
{
	VerboseMessage vmsg1 (_("calculating the matching...")) ;
	vmsg1.printMessage() ;

	// do construction heuristic (maybe more than once)
	unsigned int nconstrheur = Default_NConstrHeur ;
#ifdef DEBUG
	if (Args.NConstrHeur.is_set()) {
		nconstrheur = Args.NConstrHeur.getValue() ;
	}
#endif
	Matching *bestmatching = NULL ;
	for (unsigned int i = 0 ; i < nconstrheur ; i++) {
		ConstructionHeuristic ch (TheGraph) ;
		ch.run() ;

		if ((bestmatching == NULL) || (ch.getMatching()->getCardinality() > bestmatching->getCardinality())) {
			if (bestmatching != NULL) {
				delete bestmatching ;
			}
			bestmatching = ch.getMatching() ;
		}

		TheGraph->unmarkDeletedAllVertices() ;
	}

	VerboseMessage vmsg2 (_("best matching after construction heuristic:")) ;
	vmsg2.printMessage() ;
	bestmatching->printVerboseInfo() ;

	// do augmenting path heuristic
	if (true) { // TODO - make augmenting path heuristic optional ?
		AugmentingPathHeuristic aph (TheGraph, bestmatching) ;
		aph.run() ;
		bestmatching = aph.getMatching() ;
	}

	VerboseMessage vmsg3 (_("best matching after augmenting path heuristic:")) ;
	vmsg3.printMessage() ;
	bestmatching->printVerboseInfo() ;

#if 0
	if (!bestmatching->check()) {
		std::cerr << "check of matching to be embedded FAILED" << std::endl ;
	}
#endif

	return bestmatching ;
}
 
void Embedder::embedEdge (Edge *e)
{
	Vertex* v1 = e->getVertex1() ;
	Vertex* v2 = e->getVertex2() ;

	TheCvrStgFile->replaceSample (e->getSamplePos(v1), e->getReplacingSampleValue (v1)) ;
	TheCvrStgFile->replaceSample (e->getSamplePos(v2), e->getReplacingSampleValue (v2)) ;
}

void Embedder::embedExposedVertex (Vertex *v)
{
	SamplePos samplepos = 0 ;
	SampleValue *newsample = NULL ;
	float mindistance = FLT_MAX ;
	for (unsigned short i = 0 ; i < TheCvrStgFile->getSamplesPerEBit() ; i++) {
		SampleValue *curold = v->getSampleValue(i) ;
		SampleValue *curnew = v->getSampleValue(i)->getNearestOppositeSampleValue() ;
		if (curold->calcDistance (curnew) < mindistance) {
			samplepos = v->getSamplePos(i) ;
			newsample = curnew ;
			mindistance = curold->calcDistance (curnew) ;
		}
		else {
			delete curnew ;
		}
	}

	printDebug (1, "embedding vertex with sample position %lu.", samplepos) ;

	BIT oldbit = TheCvrStgFile->getSampleBit (samplepos) ;
	TheCvrStgFile->replaceSample (samplepos, newsample) ;
	myassert (oldbit != TheCvrStgFile->getSampleBit (samplepos)) ;
	delete newsample ;
}
