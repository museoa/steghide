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
#include <cmath>
#include <vector>

#include "BmpRGBSampleValue.h"
#include "CvrStgFile.h"
#include "Graph.h"
#include "SampleValue.h"
#include "SampleValueAdjacencyList.h"
#include "WavPCMSampleValue.h"
#include "common.h"

#if 0
// FIXME - incorporate specializations into *File
void SampleValueAdjacencyList::calcOppNeighs_rgb (const std::vector<SampleValue*> &svalues)
{
	unsigned long numsvs = svalues.size() ;

	std::vector<BmpRGBSampleValue*> svalues0 ;
	svalues0.reserve (numsvs / 2) ;

	unsigned short numcubes = 0 ;
	short cubelen = (short) roundup (sqrt ((float) Globs.TheCvrStgFile->getRadius())) ;
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
		BmpRGBSampleValue *s = (BmpRGBSampleValue*) svalues[l] ;
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
	sortOppNeighs() ;
}

void SampleValueAdjacencyList::calcOppNeighs_wav (const std::vector<SampleValue*> &svalues)
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

	// fill the OppNeighs vectors
	int r = Globs.TheCvrStgFile->getRadius() ;
	OppNeighs = std::vector<std::vector<SampleValue*> > (n) ;
	unsigned long n0 = svalues0.size() ;
	unsigned long n1 = svalues1.size() ;
	unsigned long start1 = 0 ;
	for (unsigned long i0 = 0 ; i0 < n0 ; i0++) {
		while ((start1 < n1) && (svalues1[start1]->getValue() < svalues0[i0]->getValue() - r)) {
			start1++ ;
		}
		// start1 is the index of the first sample in svalues1 that is >= (svalues0[i0]->getValue - r)
		unsigned long i1 = start1 ;
		while ((i1 < n1) && (svalues1[i1]->getValue() <= svalues0[i0]->getValue() + r)) {
			OppNeighs[svalues0[i0]->getLabel()].push_back (svalues1[i1]) ;
			OppNeighs[svalues1[i1]->getLabel()].push_back (svalues0[i0]) ;
			i1++ ;
		}
	}

	sortOppNeighs() ;
}

int SampleValueAdjacencyList::roundup (float x)
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
#endif

SampleValueAdjacencyList::SampleValueAdjacencyList (SampleValueLabel numsvs)
{
	AdjacencyList = std::vector<std::vector<SampleValue*> > (numsvs) ;
}

void SampleValueAdjacencyList::sort ()
{
	for (SampleValueLabel lbl = 0 ; lbl < AdjacencyList.size() ; lbl++) {
		std::vector<SampleValue*>& row = AdjacencyList[lbl] ;

		if (row.size() > 0) {
			UWORD32* distances = new UWORD32[row.size() + 1] ;
			for (unsigned int i = 0 ; i < row.size() ; i++) {
				distances[i] = Globs.TheGraph->SampleValues[lbl]->calcDistance (row[i]) ;
			}

			quicksort (row, distances, 0, row.size() - 1) ;

			delete[] distances ;
		}
	}
}

void SampleValueAdjacencyList::quicksort (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r)
{
	if (l < r) {
		unsigned int p = partition (oppneighs, distances, l, r, distances[r]) ;
		if (p > 0) {
			quicksort (oppneighs, distances, l, p - 1) ;
		}
		quicksort (oppneighs, distances, p + 1, r) ;
	}
}

unsigned int SampleValueAdjacencyList::partition (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r, UWORD32 x)
{
	unsigned int i = l, j = r ;
	while (i < j) {
		while ((distances[i] < x) && (i < r)) {
			i++ ;
		}
		while ((distances[j] >= x) && (j > l)) {
			j-- ;
		}
		if (i < j) {
			swap (oppneighs, distances, i, j) ;
		}
	}
	swap (oppneighs, distances, i, r) ;
	return i ;
}

void SampleValueAdjacencyList::swap (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int i, unsigned int j)
{
	SampleValue* tmp1 = oppneighs[i] ;
	UWORD32 tmp2 = distances[i] ;
	oppneighs[i] = oppneighs[j] ;
	distances[i] = distances[j] ;
	oppneighs[j] = tmp1 ;
	distances[j] = tmp2 ;
}


// FIXME - rewrite this
#if 0
bool SampleValueAdjacencyList::check (bool verbose) const
{
	bool retval = true ;
	retval = check_size (verbose) && retval ;
	retval = check_soundness (verbose) && retval ;
	retval = check_completeness (verbose) && retval ;
	retval = check_sorted (verbose) && retval ;
	retval = check_uniqueness (verbose) && retval ;
	return retval ;
}

bool SampleValueAdjacencyList::check_size (bool verbose) const
{
	bool size = (OppNeighs.size() == TheGraph->SampleValues.size()) ;
	if (!size && verbose) {
		std::cerr << std::endl << "---- FAILED: check_size ----" << std::endl ;
		std::cerr << "OppNeighs.size(): " << OppNeighs.size() << std::endl ;
		std::cerr << "TheGraph->SampleValues.size(): " << TheGraph->SampleValues.size() << std::endl ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	return size ;
}

bool SampleValueAdjacencyList::check_soundness (bool verbose) const
{
	unsigned long numsvs = TheGraph->SampleValues.size() ;

	// check if everything in OppNeighs really is an opposite neighbour
	bool err_nonopp = false ;
	bool err_nonneigh = false ;
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

bool SampleValueAdjacencyList::check_sorted (bool verbose) const
{
	// check if OppNeighs[l][1...n] have increasing distance
	bool err_unsorted = false ;
	for (SampleValueLabel srclbl = 0 ; srclbl < OppNeighs.size() ; srclbl++) {
		SampleValue* srcsv = TheGraph->SampleValues[srclbl] ;
		const std::vector<SampleValue*> &oppneighs = OppNeighs[srclbl] ;
		if (oppneighs.size() > 1) {
			for (unsigned int i = 0 ; i < (oppneighs.size() - 1) ; i++) {
				UWORD32 d1 = srcsv->calcDistance (oppneighs[i]) ;
				UWORD32 d2 = srcsv->calcDistance (oppneighs[i + 1]) ;
				if (!(d1 <= d2)) {
					err_unsorted = true ;
					if (verbose) {
						std::cerr << std::endl << "---- FAILED: check_sorted ----" << std::endl ;
						std::cerr << "source sample:" << std::endl ;
						srcsv->print(1) ;
						std::cerr << "dest sample at position " << i << ":" << std::endl ;
						oppneighs[i]->print(1) ;
						std::cerr << "dest sample at position " << i + 1 << ":" << std::endl ;
						oppneighs[i + 1]->print(1) ;
						std::cerr << "-------------------------------------" << std::endl ;
					}
				}
			}
		}
	}

	return !err_unsorted ;
}

bool SampleValueAdjacencyList::check_uniqueness (bool verbose) const
{
	// check if there is no sample value that has two entries in an OppNeighs[i]
	bool err_nonunique = false ;
	for (SampleValueLabel srclbl = 0 ; srclbl < OppNeighs.size() ; srclbl++) {
		for (unsigned int i = 0 ; i < OppNeighs[srclbl].size() ; i++) {
			for (unsigned int j = i + 1 ; j < OppNeighs[srclbl].size() ; j++) {
				if (*(OppNeighs[srclbl][i]) == *(OppNeighs[srclbl][j])) {
					err_nonunique = true ;
				}
			}
		}
	}
	return !err_nonunique ;
}

bool SampleValueAdjacencyList::check_completeness (bool verbose) const
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
#endif
