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
}

Embedder::Embedder (string cfn, string sfn, string efn)
{
	CoverFileName = cfn ;
	StegoFileName = sfn ;
	EmbedFileName = efn ;
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
#if 0
		Bit xorresult = 0 ;
		for (unsigned int j = 0 ; j < sam_ebit ; j++) {
			// process one sample
			SBitPos firstsbitpos = (*perm) * sbit_sam ;
			for (unsigned int k = 0 ; k < sbit_sam ; k++) {
				// process one sbit
				xorresult ^= cvrstgfile->getSBitValue (firstsbitpos + k) ;
			}
			++perm ;
		}

		if (xorresult != toembed[i]) {
			cvrstgfile->replaceSample (sel.getIndex (i_modify), sel.getSample (i_modify)) ;
			i_modify++ ;
		}
	}
#endif

	cvrstgfile->transform (StegoFileName) ;
	cvrstgfile->write() ;
}

void Embedder::calculate (CvrStgFile *csf, BitString e)
{
	Permutation perm (csf->getNumSamples(), Args.Passphrase.getValue()) ;

	unsigned long n = e.getLength() ;
	unsigned int sam_ebit = csf->getSamplesPerEBit() ;
	unsigned int sbit_sam = csf->getSBitsPerSample() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		vector<SamplePos> sampleposs ;
		vector<CvrStgSample*> samples ;
		Bit xorresult = 0 ;
		for (unsigned int j = 0 ; j < sam_ebit ; j++) {
			// process one sample
			sampleposs.push_back (*perm) ;
			samples.push_back (csf->getSample (*perm)) ;
			SBitPos firstsbitpos = (*perm) * sbit_sam ;	// FIXME - CvrStgFile(?Object) erweitern um getFirstSBitPos (SamplePos) und getSamplePos (SBitPos)
			for (unsigned int k = 0 ; k < sbit_sam ; k++) {
				// process one sbit
				xorresult ^= csf->getSBitValue (firstsbitpos + k) ;
			}
			++perm ;
		}

		if (xorresult == e[i]) {
			NeedsChange.push_back (false) ;
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
