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

#include <string>

#include "bitstring.h"
#include "common.h"
#include "cvrstgfile.h"
#include "embdata.h"
#include "embedder.h"
#include "error.h"
#include "msg.h"
#include "permutation.h"
#include "vertex.h"

Embedder::Embedder ()
{
	CoverFileName = Args.CvrFn.getValue() ;
	StegoFileName = Args.StgFn.getValue() ;
	EmbedFileName = Args.EmbFn.getValue() ;
}

void Embedder::embed ()
{
	EmbData embdata (EmbData::EMBED, EmbedFileName) ;
	embdata.setCryptAlgo (MCryptpp::getAlgorithm (Args.EncAlgo.getValue())) ;
	embdata.setCryptMode (MCryptpp::getMode (Args.EncMode.getValue())) ;
	embdata.setCompression (false) ;
	embdata.setChecksum (Args.Checksum.getValue()) ;

	CvrStgFile *cvrstgfile = CvrStgFile::readFile (CoverFileName) ;

	BitString toembed = embdata.getBitString() ;
	unsigned long n = toembed.getLength() ;

	if ((n * cvrstgfile->getSamplesPerEBit()) > cvrstgfile->getNumSamples()) {
		throw SteghideError (_("the cover file is too short to embed the data.")) ;
	}

	VerboseMessage vmsg (_("embedding %lu bits in %lu samples."), n, cvrstgfile->getNumSamples()) ;
	vmsg.printMessage() ;

	calculate (cvrstgfile, toembed) ;

	unsigned long i_modify = 0 ; // is the vertex label
	for (unsigned long i = 0 ; i < n ; i++) {
		if (NeedsChange[i]) {
			embedVertex (cvrstgfile, MGraph.getVertex (i_modify)) ;
			i_modify++ ;
		}
	}

	cvrstgfile->transform (StegoFileName) ;
	cvrstgfile->write() ;
}

void Embedder::calculate (CvrStgFile *csf, const BitString &e)
{
	Permutation perm (csf->getNumSamples(), Args.Passphrase.getValue()) ;

	MGraph = Graph (csf) ;
	MGraph.startAdding() ;
	
	unsigned long n = e.getLength() ;
	unsigned int sam_ebit = csf->getSamplesPerEBit() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		vector<SamplePos> poss ;
		Bit xorresult = 0 ;
		for (unsigned int j = 0 ; j < sam_ebit ; j++) {
			poss.push_back (*perm) ;
			xorresult ^= csf->getSampleBit (*perm) ;
			++perm ;
		}

		if (xorresult == e[i]) {
			NeedsChange.push_back (false) ;
		}
		else {
			MGraph.addVertex (poss) ;
			NeedsChange.push_back (true) ;
		}
	}

	MGraph.finishAdding() ;

	MGraph.calcMatching() ;
}
 
void Embedder::embedVertex (CvrStgFile *csf, Vertex *v)
{
	SamplePos samplepos = 0 ;
	SampleValue *newsample = NULL ;
	if (v->isMatched()) {
		Edge *e = v->getMatchingEdge() ;
		samplepos = e->getSamplePos (v) ;
		newsample = e->getReplacingSample (v) ;
	}
	else {
		// choose a random sample (of those that are in the vertex) to embed data
		// FIXME - the sample with the nearest NearestOppositeSample() should be chosen
		unsigned short rnd = RndSrc.getValue (csf->getSamplesPerEBit()) ;
		samplepos = v->getSamplePos (rnd) ;
		newsample = v->getSample(rnd)->getNearestOppositeSampleValue() ;
	}

	Bit oldbit = csf->getSampleBit (samplepos) ;
	csf->replaceSample (samplepos, newsample) ;
	assert (oldbit != csf->getSampleBit (samplepos)) ;
}
