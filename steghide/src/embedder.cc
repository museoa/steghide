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

#include <string>

#include "bitstring.h"
#include "common.h"
#include "cvrstgfile.h"
#include "embdata.h"
#include "embedder.h"
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

	calculate (cvrstgfile, toembed) ;

	unsigned long i_modify = 0 ; // is the vertex label
	for (unsigned long i = 0 ; i < n ; i++) {
		if (NeedsChange[i]) {
			cvrstgfile->replaceSample (getIndex (i_modify), getSample (i_modify)) ;
			i_modify++ ;
		}
	}

	cvrstgfile->transform (StegoFileName) ;
	cvrstgfile->write() ;
}

void Embedder::calculate (CvrStgFile *csf, BitString e)
{
	Permutation perm (csf->getNumSamples(), Args.Passphrase.getValue()) ;

	unsigned long n = e.getLength() ;
	unsigned int sam_ebit = csf->getSamplesPerEBit() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		vector<SamplePos> sampleposs ;
		vector<CvrStgSample*> samples ;	// FIXME - the same samples (i.e. with the same values) are kept more than one time in vertices in graph
		Bit xorresult = 0 ;
		for (unsigned int j = 0 ; j < sam_ebit ; j++) {
			sampleposs.push_back (*perm) ;
			CvrStgSample *sample = csf->getSample (*perm) ;
			samples.push_back (sample) ;
			xorresult ^= sample->getBit() ;
			++perm ;
		}

		if (xorresult == e[i]) {
			NeedsChange.push_back (false) ;
			for (vector<CvrStgSample*>::iterator i = samples.begin() ; i != samples.end() ; i++) {
				delete *i ;
			}
		}
		else {
			Vertex *v = new Vertex (sampleposs, samples) ;
			MGraph.addVertex (v) ;
			NeedsChange.push_back (true) ;
		}
	}
}

SamplePos Embedder::getIndex (unsigned long n)
{
	// FIXME
	Vertex *v = MGraph.getVertex(n) ;
	return v->getSamplePos(0) ;
}

CvrStgSample* Embedder::getSample (unsigned long n)
{
	// FIXME
	Vertex *v = MGraph.getVertex(n) ;
	CvrStgSample *sample = v->getSample(0) ;
	return sample->getNearestOppositeNeighbour() ;
}
