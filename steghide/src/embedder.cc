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

#include "augheur.h"
#include "bitstring.h"
#include "common.h"
#include "constrheur.h"
#include "cvrstgfile.h"
#include "embdata.h"
#include "embedder.h"
#include "error.h"
#include "matching.h"
#include "msg.h"
#include "permutation.h"
#include "vertex.h"

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
	
	// create graph
	vector<SamplePos*> sampleposs ;
	Permutation perm (TheCvrStgFile->getNumSamples(), Args.Passphrase.getValue()) ;
	unsigned long n = ToEmbed.getLength() ;
	unsigned int sam_ebit = TheCvrStgFile->getSamplesPerEBit() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SamplePos *poss = new SamplePos[sam_ebit] ;
		Bit parity = 0 ;
		for (unsigned int j = 0 ; j < sam_ebit ; j++) {
			poss[j] = *perm ;
			parity ^= TheCvrStgFile->getSampleBit (*perm) ;
			++perm ;
		}

		if (parity != ToEmbed[i]) {
			sampleposs.push_back (poss) ;
		}
	}
	TheGraph = new Graph (TheCvrStgFile, sampleposs) ;
}

Embedder::~Embedder ()
{
	delete TheCvrStgFile ;
	delete TheGraph ;
}
	
void Embedder::embed ()
{
	unsigned long n = ToEmbed.getLength() ;

	if ((n * TheCvrStgFile->getSamplesPerEBit()) > TheCvrStgFile->getNumSamples()) {
		throw SteghideError (_("the cover file is too short to embed the data.")) ;
	}

	VerboseMessage vmsg (_("embedding %lu bits in %lu samples."), n, TheCvrStgFile->getNumSamples()) ;
	vmsg.printMessage() ;

	const Matching* M = calculateMatching() ;

	// embed matched edges
	const list<Edge*> medges = M->getEdges() ;
	for (list<Edge*>::const_iterator it = medges.begin() ; it != medges.end() ; it++) {
		embedEdge (*it) ;
	}

	// embed exposed vertices
	const list<Vertex*> *expvertices = M->getExposedVerticesLink() ;
	for (list<Vertex*>::const_iterator it = expvertices->begin() ; it != expvertices->end() ; it++) {
		embedExposedVertex (*it) ;
	}

	TheCvrStgFile->transform (Args.StgFn.getValue()) ;
	TheCvrStgFile->write() ;
}

const Matching *Embedder::calculateMatching ()
{
	TheGraph->printVerboseInfo() ;
	VerboseMessage vmsg (_("calculating the matching...")) ;
	vmsg.printMessage() ;

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
			bestmatching = ch.getMatching() ;
		}

	}

	// do augmenting path heuristic
	if (false) { // TODO - make augmenting path heuristic optional ?
		AugmentingPathHeuristic aph (TheGraph, bestmatching) ;
		aph.run() ;
		bestmatching = aph.getMatching() ;
	}

	return bestmatching ;
}
 
void Embedder::embedEdge (Edge *e)
{
	SamplePos samplepos1 = e->getSamplePos (e->getVertex1()) ;
	SamplePos samplepos2 = e->getSamplePos (e->getVertex2()) ;

	SampleValue *tmp = TheCvrStgFile->getSampleValue (samplepos1) ;
	TheCvrStgFile->replaceSample (samplepos1, TheCvrStgFile->getSampleValue (samplepos2)) ;
	TheCvrStgFile->replaceSample (samplepos2, tmp) ;
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

	Bit oldbit = TheCvrStgFile->getSampleBit (samplepos) ;
	TheCvrStgFile->replaceSample (samplepos, newsample) ;
	assert (oldbit != TheCvrStgFile->getSampleBit (samplepos)) ;
	delete newsample ;
}
