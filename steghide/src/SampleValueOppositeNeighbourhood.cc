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

#include <algorithm>
#include <vector>

#include "BmpRGBSampleValue.h"
#include "common.h"
#include "Graph.h"
#include "SampleValue.h"
#include "SampleValueOppositeNeighbourhood.h"
#include "WavPCMSampleValue.h"

SampleValueOppositeNeighbourhood::SampleValueOppositeNeighbourhood (Graph* g, const std::vector<SampleValue*>& svalues)
	: GraphAccess(g)
{
	if (svalues.size() > 0) { // graph contains at least one vertex
		if (dynamic_cast<BmpRGBSampleValue*> (svalues[0])) {
			calcOppNeighs_rgb (svalues) ;
		}
		else if (dynamic_cast<WavPCMSampleValue*> (svalues[0])) {
			calcOppNeighs_wav (svalues) ;
		}
		else {
			calcOppNeighs_generic (svalues) ;
		}
	}
}

void SampleValueOppositeNeighbourhood::calcOppNeighs_generic (const std::vector<SampleValue*> &svalues)
{
	// partition samples into those with getBit() == 0 and with getBit() == 1
	unsigned long numsvs = svalues.size() ;

	std::vector<SampleValue*> svalues0 ;
	svalues0.reserve (numsvs / 2) ;
	std::vector<SampleValue*> svalues1 ;
	svalues1.reserve (numsvs / 2) ;

	for (unsigned long l = 0 ; l < numsvs ; l++) {
		if (svalues[l]->getBit() == 0) {
			svalues0.push_back (svalues[l]) ;
		}
		else {
			svalues1.push_back (svalues[l]) ;
		}
	}

	// fill OppNeighs data structure
	OppNeighs = std::vector<std::vector<SampleValue*> > (numsvs) ;
	for (unsigned long i0 = 0 ; i0 < svalues0.size() ; i0++) {
		for (unsigned long i1 = 0 ; i1 < svalues1.size() ; i1++) {
			if (svalues0[i0]->isNeighbour(svalues1[i1])) {
				OppNeighs[svalues0[i0]->getLabel()].push_back (svalues1[i1]) ;
				OppNeighs[svalues1[i1]->getLabel()].push_back (svalues0[i0]) ;
			}
		}
	}

	// sort OppNeighs data structure
	for (unsigned long lbl = 0 ; lbl < numsvs ; lbl++) {
		sort (OppNeighs[lbl].begin(), OppNeighs[lbl].end(), SmallerDistance(TheGraph->SampleValues[lbl])) ;
	}
}

void SampleValueOppositeNeighbourhood::calcOppNeighs_rgb (const std::vector<SampleValue*> &svalues)
{
	unsigned long numsvs = svalues.size() ;

	std::vector<BmpRGBSampleValue*> svalues0 ;
	svalues0.reserve (numsvs / 2) ;

	unsigned short numcubes = 0 ;
	short cubelen = (short int) roundup(svalues[0]->getRadius()) ;
	if (256 % cubelen == 0) {
		numcubes = 256 / cubelen ;
	}
	else {
		numcubes = (256 / cubelen) + 1 ;
	}

	// init cubes
	std::vector<BmpRGBSampleValue*> cubes[numcubes][numcubes][numcubes] ;

	// fill svalues0 and cubes
	for (unsigned long l = 0 ; l < numsvs ; l++) {
		BmpRGBSampleValue *s = dynamic_cast<BmpRGBSampleValue*> (svalues[l]) ;
		myassert (s != NULL) ;
		if (s->getBit() == 0) {
			svalues0.push_back (s) ;
		}
		else {
			unsigned short i_red = s->getRed() / cubelen ;
			unsigned short i_green = s->getGreen() / cubelen ;
			unsigned short i_blue = s->getBlue() / cubelen ;
			cubes[i_red][i_green][i_blue].push_back (s) ;
		}
	}

	// fill OppNeighs data structure
	OppNeighs = std::vector<std::vector<SampleValue*> > (numsvs) ;
	unsigned long numsv0 = svalues0.size() ;

	for (unsigned long i = 0 ; i < numsv0 ; i++) {
		// find all opposite neighbours of svalues0[i] (in the 3^3 cubes around samplesvalues0[i])
		short start_red = (svalues0[i]->getRed() / cubelen) - 1, end_red = start_red + 2 ;
		short start_green = (svalues0[i]->getGreen() / cubelen) - 1, end_green = start_green + 2 ;
		short start_blue = (svalues0[i]->getBlue() / cubelen) - 1, end_blue = start_blue + 2 ;
		if (start_red < 0) start_red = 0 ;
		if (start_green < 0) start_green = 0 ;
		if (start_blue < 0) start_blue = 0 ;
		if (end_red >= numcubes) end_red = numcubes - 1 ;
		if (end_green >= numcubes) end_green = numcubes - 1 ;
		if (end_blue >= numcubes) end_blue = numcubes - 1 ;
		SampleValue* thissv = svalues0[i] ;
		SampleValueLabel thissv_label = thissv->getLabel() ;

		for (short i_red = start_red ; i_red <= end_red ; i_red++) {
			for (short i_green = start_green ; i_green <= end_green ; i_green++) {
				for (short i_blue = start_blue ; i_blue <= end_blue ; i_blue++) {
					const std::vector<BmpRGBSampleValue*> &thiscube = cubes[i_red][i_green][i_blue] ;
					for (unsigned int j = 0 ; j < thiscube.size() ; j++) {
						if (svalues0[i]->isNeighbour (thiscube[j])) {
							OppNeighs[thissv_label].push_back (thiscube[j]) ;
							OppNeighs[thiscube[j]->getLabel()].push_back (thissv) ;
						}
					}
				}
			}
		}
	}

	// sort OppNeighs data structure
	for (unsigned long lbl = 0 ; lbl < numsvs ; lbl++) {
		sort (OppNeighs[lbl].begin(), OppNeighs[lbl].end(), SmallerDistance(TheGraph->SampleValues[lbl])) ;
	}
}

void SampleValueOppositeNeighbourhood::calcOppNeighs_wav (const std::vector<SampleValue*> &svalues)
{
	// partition samples into those with getBit() == 0 and with getBit() == 1
	unsigned long n = svalues.size() ;

	std::vector<WavPCMSampleValue*> svalues0 ;
	svalues0.reserve (n / 2) ;
	std::vector<WavPCMSampleValue*> svalues1 ;
	svalues1.reserve (n / 2) ;

	for (unsigned long l = 0 ; l < n ; l++) {
		WavPCMSampleValue *s = dynamic_cast<WavPCMSampleValue*> (svalues[l]) ;
		myassert (s != NULL) ;
		if (s->getBit() == 0) {
			svalues0.push_back (s) ;
		}
		else {
			svalues1.push_back (s) ;
		}
	}

	WavPCMSmaller smaller ;
	sort (svalues0.begin(), svalues0.end(), smaller) ;
	sort (svalues1.begin(), svalues1.end(), smaller) ;

	// fill the OppNeighs std::vectors
	int r_ub = roundup (svalues[0]->getRadius()) ;
	OppNeighs = std::vector<std::vector<SampleValue*> > (n) ;
	unsigned long n0 = svalues0.size() ;
	unsigned long n1 = svalues1.size() ;
	unsigned long start1 = 0 ;
	for (unsigned long i0 = 0 ; i0 < n0 ; i0++) {
		while ((start1 < n1) && (svalues1[start1]->getValue() < svalues0[i0]->getValue() - r_ub)) {
			start1++ ;
		}
		unsigned long i1 = start1 ;
		while ((i1 < n1) && (svalues1[i1]->getValue() <= svalues0[i0]->getValue() + r_ub)) {
			OppNeighs[svalues0[i0]->getLabel()].push_back (svalues1[i1]) ;
			OppNeighs[svalues1[i1]->getLabel()].push_back (svalues0[i0]) ;
			i1++ ;
		}
	}

	// FIXME - don't use generic sort (!)
	for (unsigned long lbl = 0 ; lbl < n ; lbl++) {
		sort (OppNeighs[lbl].begin(), OppNeighs[lbl].end(), SmallerDistance(TheGraph->SampleValues[lbl])) ;
	}
}

int SampleValueOppositeNeighbourhood::roundup (float x)
{
	int retval = -1 ;
	float intpart = (float) ((int) x) ;
	if (x - intpart == 0) {
		retval = (int) x ;
	}
	else {
		retval = ((int) x) + 1 ;
	}
	return retval ;
}

bool SampleValueOppositeNeighbourhood::check (void) const
{
	return check_soundness() && check_completeness() ;
}

bool SampleValueOppositeNeighbourhood::check_soundness (void) const
{
	bool err_nonopp = false ;
	bool err_nonneigh = false ;

	unsigned long numsvs = TheGraph->SampleValues.size() ;
	for (SampleValueLabel srclbl = 0 ; srclbl < numsvs ; srclbl++) {
		SampleValue* srcsv = TheGraph->SampleValues[srclbl] ;
		const std::vector<SampleValue*> &oppneighs = OppNeighs[srclbl] ;
		for (std::vector<SampleValue*>::const_iterator destsv = oppneighs.begin() ; destsv != oppneighs.end() ; destsv++) {
			if (srcsv->getBit() == (*destsv)->getBit()) {
				err_nonopp = true ;
			}

			if (!srcsv->isNeighbour(*destsv)) {
				err_nonneigh = true ;
			}
		}
	}

	return !(err_nonopp || err_nonneigh) ;
}

bool SampleValueOppositeNeighbourhood::check_completeness (void) const
{
	bool err = false ;

	unsigned long numsvs = TheGraph->SampleValues.size() ;
	for (unsigned long i = 0 ; i < numsvs ; i++) {
		SampleValue *sv1 = TheGraph->SampleValues[i] ;
		for (unsigned long j = 0 ; j < numsvs ; j++) {
			SampleValue *sv2 = TheGraph->SampleValues[j] ;
			if (sv1->getBit() != sv2->getBit()) {
				// they are opposite...
				if (sv1->isNeighbour (sv2)) {
					// ...and they are neighbours => there must be an entry in SampleOppositeNeighbourhood
					myassert (sv2->isNeighbour (sv1)) ;
					const std::vector<SampleValue*> &oppneighs = OppNeighs[i] ;
					bool found = false ;
					for (std::vector<SampleValue*>::const_iterator k = oppneighs.begin() ; k != oppneighs.end() ; k++) {
						if ((*k)->getLabel() == j) {
							found = true ;
						}
					}
					if (!found) {
						err = true ;
					}
				}
			}
		}
	}

	return !err ;
}
