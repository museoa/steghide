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
#include "CvrStgFile.h"
#include "DMDConstructionHeuristic.h"
#include "EmbData.h"
#include "Embedder.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "ProgressOutput.h"
#include "Selector.h"
#include "Vertex.h"
#include "WKSConstructionHeuristic.h"
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
	ToEmbed.setArity (Globs.TheCvrStgFile->getEmbValueModulus()) ;
	
	if ((ToEmbed.getNAryLength() * Globs.TheCvrStgFile->getSamplesPerVertex()) > Globs.TheCvrStgFile->getNumSamples()) {
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
	else if (Args.DebugCommand.getValue() == PRINTGMLGRAPH) {
		Globs.TheGraph->print_gml (std::cout) ;
		exit (EXIT_SUCCESS) ;
	}
	else if (Args.DebugCommand.getValue() == PRINTGMLVERTEX) {
		std::vector<bool> nodeprinted (Globs.TheGraph->getNumVertices()) ;
		std::vector<bool> edgesprinted (Globs.TheGraph->getNumVertices()) ;
		Globs.TheGraph->printPrologue_gml(std::cout) ;
		Globs.TheGraph->printVertex_gml (std::cout, Globs.TheGraph->getVertex(Args.GmlStartVertex.getValue()), Args.GmlGraphRecDepth.getValue(), nodeprinted, edgesprinted) ;
		Globs.TheGraph->printEpilogue_gml(std::cout) ;
		exit (EXIT_SUCCESS) ;
	}
#endif
}

Embedder::~Embedder ()
{
	delete Globs.TheGraph ;
	delete Globs.TheCvrStgFile ;
}
	
void Embedder::embed ()
{
	ProgressOutput* prout = NULL ;
	if (Args.Verbosity.getValue() == NORMAL) {
		prout = new ProgressOutput () ;
	}

	const Matching* M = calculateMatching (prout) ;

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

const Matching* Embedder::calculateMatching (ProgressOutput* prout)
{
	VerboseMessage vmsg1 (_("calculating the matching...")) ;
	vmsg1.printMessage() ;

	Matching* matching = new Matching (Globs.TheGraph) ;

	std::vector<MatchingAlgorithm*> MatchingAlgos ;
	if (Args.Algorithm.is_set()) {
		switch (Args.Algorithm.getValue()) {
			case Arguments::Algorithm_None:
			// leave MatchingAlgos empty
			break ;

			case Arguments::Algorithm_CHOnly:
			MatchingAlgos.push_back (new WKSConstructionHeuristic (Globs.TheGraph, matching)) ;
			break ;

			case Arguments::Algorithm_BoundedAPH:
			MatchingAlgos.push_back (new WKSConstructionHeuristic (Globs.TheGraph, matching)) ;
			MatchingAlgos.push_back (new AugmentingPathHeuristic (Globs.TheGraph, matching, Args.Goal.getValue(), (UWORD32) (Globs.TheGraph->getAvgVertexDegree() / 20), EdgeIterator::SAMPLEVALUE)) ;
			break ;

			case Arguments::Algorithm_UnboundedAPH:
			MatchingAlgos.push_back (new WKSConstructionHeuristic (Globs.TheGraph, matching)) ;
			MatchingAlgos.push_back (new AugmentingPathHeuristic (Globs.TheGraph, matching, Args.Goal.getValue())) ;
			break ;

			default:
			myassert(false) ;
			break ;
		}

	}
	else {
		MatchingAlgos = Globs.TheCvrStgFile->getMatchingAlgorithms (Globs.TheGraph, matching) ;
	}

	for (std::vector<MatchingAlgorithm*>::iterator ait = MatchingAlgos.begin() ; ait != MatchingAlgos.end() ; ait++) {
		(*ait)->setGoal (Args.Goal.getValue()) ;
		(*ait)->run() ;
		delete (*ait) ;
	}

	matching->printVerboseInfo() ;

	if (prout) {
		prout->update (((float) (2 * matching->getCardinality())) / ((float) Globs.TheGraph->getNumVertices()), true) ;
		delete prout ;
	}

	return matching ;
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
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SampleValue *curold = v->getSampleValue(i) ;
		SampleValue *curnew = v->getSampleValue(i)->getNearestTargetSampleValue(v->getTargetValue(i)) ;
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

	EmbValue oldev = Globs.TheCvrStgFile->getEmbeddedValue (samplepos) ;
	Globs.TheCvrStgFile->replaceSample (samplepos, newsample) ;
	myassert (oldev != Globs.TheCvrStgFile->getEmbeddedValue (samplepos)) ;
	delete newsample ;
}
