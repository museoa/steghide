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
#include "ConstructionHeuristic.h"
#include "CvrStgFile.h"
#include "EmbData.h"
#include "Embedder.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "ProgressOutput.h"
#include "Selector.h"
#include "Vertex.h"
#include "common.h"
#include "error.h"
#include "msg.h"

Globals Globs ;

Embedder::Embedder ()
{
	// read embfile
	std::vector<BYTE> emb ;
	BinaryIO embio (Args.EmbFn.getValue(), BinaryIO::READ) ;
	while (!embio.eof()) {
		emb.push_back (embio.read8()) ;
	}
	embio.close() ;

	// create bitstring to be embedded
	std::string fn = "" ;
	if (Args.EmbedEmbFn.getValue()) {
		fn = Args.EmbFn.getValue() ;
	}
	EmbData embdata (EmbData::EMBED, Args.Passphrase.getValue(), fn) ;
	embdata.setEncAlgo (Args.EncAlgo.getValue()) ;
	embdata.setEncMode (Args.EncMode.getValue()) ;
	embdata.setCompression (Args.Compression.getValue()) ;
	embdata.setChecksum (Args.Checksum.getValue()) ;
	embdata.setData (emb) ;
	ToEmbed = embdata.getBitString() ;

	// read cover-/stego-file
	CvrStgFile::readFile (Args.CvrFn.getValue()) ;
	
	if ((ToEmbed.getLength() * Globs.TheCvrStgFile->getSamplesPerEBit()) > Globs.TheCvrStgFile->getNumSamples()) {
		throw SteghideError (_("the cover file is too short to embed the data.")) ;
	}

	// create graph
	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Args.Passphrase.getValue()) ;
	new Graph (Globs.TheCvrStgFile, ToEmbed, sel) ;
	Globs.TheGraph->printVerboseInfo() ;

#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTGRAPH) {
		Globs.TheGraph->print() ;
		exit (EXIT_SUCCESS) ;
	}
#endif
}

Embedder::~Embedder ()
{
	delete Globs.TheCvrStgFile ;
	delete Globs.TheGraph ;
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

	Globs.TheCvrStgFile->transform (Args.StgFn.getValue()) ;
	Globs.TheCvrStgFile->write() ;
}

const Matching* Embedder::calculateMatching ()
{
	VerboseMessage vmsg1 (_("calculating the matching...")) ;
	vmsg1.printMessage() ;

	ProgressOutput* prout = NULL ;
	if (Args.Verbosity.getValue() == NORMAL) {
		prout = new ProgressOutput () ;
	}

	// do construction heuristic (maybe more than once)
	unsigned int nconstrheur = Default_NConstrHeur ;
#ifdef DEBUG
	if (Args.NConstrHeur.is_set()) {
		nconstrheur = Args.NConstrHeur.getValue() ;
	}
#endif
	Matching* bestmatching = NULL ;
	for (unsigned int i = 0 ; i < nconstrheur ; i++) {
		// create an empty matching for the construction heuristic to start with
		Matching* thismatching = new Matching (Globs.TheGraph, prout) ;

		ConstructionHeuristic ch (Globs.TheGraph, thismatching, Args.Goal.getValue()) ;
		ch.run() ;

		if ((bestmatching == NULL) || (thismatching->getCardinality() > bestmatching->getCardinality())) {
			if (bestmatching != NULL) {
				delete bestmatching ;
			}
			bestmatching = thismatching ;
		}
		else {
			delete thismatching ;
		}
	}

	VerboseMessage vmsg2 (_("best matching after construction heuristic:")) ;
	vmsg2.printMessage() ;
	bestmatching->printVerboseInfo() ;

	if (true) {
#if 0
		AugmentingPathHeuristic aph (Globs.TheGraph, bestmatching, Args.Goal.getValue(), (UWORD32) (Globs.TheGraph->getAvgVertexDegree() / 20)) ;
		aph.run() ;
		bestmatching = aph.getMatching() ;

		VerboseMessage vmsg3 (_("best matching after bounded augmenting path heuristic:")) ;
		vmsg3.printMessage() ;
		bestmatching->printVerboseInfo() ;
#endif
		AugmentingPathHeuristic aph (Globs.TheGraph, bestmatching, Args.Goal.getValue()) ;

		// do unbounded augmenting path heuristic
		if (true) {
			aph.reset() ;
			aph.run() ;
			bestmatching = aph.getMatching() ;

			VerboseMessage vmsg4 (_("best matching after unbounded augmenting path heuristic:")) ;
			vmsg4.printMessage() ;
			bestmatching->printVerboseInfo() ;
		}
	}

	if (prout) {
		prout->update (((float) (2 * bestmatching->getCardinality())) / ((float) Globs.TheGraph->getNumVertices()), true) ;
		delete prout ;
	}

	return bestmatching ;
}
 
void Embedder::embedEdge (Edge *e)
{
	Vertex* v1 = e->getVertex1() ;
	Vertex* v2 = e->getVertex2() ;

	Globs.TheCvrStgFile->replaceSample (e->getSamplePos(v1), e->getReplacingSampleValue (v1)) ;
	Globs.TheCvrStgFile->replaceSample (e->getSamplePos(v2), e->getReplacingSampleValue (v2)) ;
}

void Embedder::embedExposedVertex (Vertex *v)
{
	SamplePos samplepos = 0 ;
	SampleValue *newsample = NULL ;
	float mindistance = FLT_MAX ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerEBit() ; i++) {
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

#ifdef DEBUG
	printDebug (1, "embedding vertex with label %lu by changing sample position %lu.", v->getLabel(), samplepos) ;
#endif

	BIT oldbit = Globs.TheCvrStgFile->getSampleBit (samplepos) ;
	Globs.TheCvrStgFile->replaceSample (samplepos, newsample) ;
	myassert (oldbit != Globs.TheCvrStgFile->getSampleBit (samplepos)) ;
	delete newsample ;
}
