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

#include "bmpsamplevalue.h"
#include "common.h"
#include "samplevalue.h"
#include "svalueoppneigh.h"
#include "wavsamplevalue.h"

SampleValueOppositeNeighbourhood::SampleValueOppositeNeighbourhood (const vector<SampleValue*> &svalues)
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

void SampleValueOppositeNeighbourhood::calcOppNeighs_generic (const vector<SampleValue*> &svalues)
{
	// partition samples into those with getBit() == 0 and with getBit() == 1
	unsigned long n = svalues.size() ;

	vector<SampleValue*> svalues0 ;
	svalues0.reserve (n / 2) ;
	vector<SampleValue*> svalues1 ;
	svalues1.reserve (n / 2) ;

	for (unsigned long l = 0 ; l < n ; l++) {
		if (svalues[l]->getBit() == 0) {
			svalues0.push_back (svalues[l]) ;
		}
		else {
			svalues1.push_back (svalues[l]) ;
		}
	}

	// fill OppNeighs data structure
	OppNeighs = vector<vector<SampleValueLabel> > (n) ;
	unsigned long n0 = svalues0.size() ;
	unsigned long n1 = svalues1.size() ;
	for (unsigned long i0 = 0 ; i0 < n0 ; i0++) {
		for (unsigned long i1 = 0 ; i1 < n1 ; i1++) {
			if (svalues0[i0]->isNeighbour(svalues1[i1])) {
				unsigned long l0 = svalues0[i0]->getLabel() ;
				unsigned long l1 = svalues1[i1]->getLabel() ;
				OppNeighs[l0].push_back (l1) ;
				OppNeighs[l1].push_back (l0) ;
			}
		}
	}
}

void SampleValueOppositeNeighbourhood::calcOppNeighs_rgb (const vector<SampleValue*> &svalues)
{
	unsigned long numsvalues = svalues.size() ;

	vector<BmpRGBSampleValue*> svalues0 ;
	svalues0.reserve (numsvalues / 2) ;

	unsigned short numcubes = 0 ;
	short r = (short) svalues[0]->getRadius() ; // FIXME - was ist mit radius der nicht \in N
	if (256 % r == 0) {
		numcubes = 256 / r ;
	}
	else {
		numcubes = (256 / r) + 1 ;
	}

	// init cubes
	vector<BmpRGBSampleValue*> cubes[numcubes][numcubes][numcubes] ;

	// fill svalues0 and cubes
	for (unsigned long l = 0 ; l < numsvalues ; l++) {
		BmpRGBSampleValue *s = dynamic_cast<BmpRGBSampleValue*> (svalues[l]) ;
		assert (s != NULL) ;
		if (s->getBit() == 0) {
			svalues0.push_back (s) ;
		}
		else {
			unsigned short i_red = s->getRed() / r ;
			unsigned short i_green = s->getGreen() / r ;
			unsigned short i_blue = s->getBlue() / r ;
			cubes[i_red][i_green][i_blue].push_back (s) ;
		}
	}

	// fill OppNeighs data structure
	OppNeighs = vector<vector<SampleValueLabel> > (numsvalues) ;
	unsigned long numsv0 = svalues0.size() ;
	for (unsigned long i = 0 ; i < numsv0 ; i++) {
		// find all opposite neighbours of svalues0[i] (in the 3^3 cubes around samplesvalues0[i])
		short start_red = (svalues0[i]->getRed() / r) - 1, end_red = start_red + 2 ;
		short start_green = (svalues0[i]->getGreen() / r) - 1, end_green = start_green + 2 ;
		short start_blue = (svalues0[i]->getBlue() / r) - 1, end_blue = start_blue + 2 ;
		if (start_red < 0) start_red = 0 ;
		if (start_green < 0) start_green = 0 ;
		if (start_blue < 0) start_blue = 0 ;
		if (end_red >= numcubes) end_red = numcubes - 1 ;
		if (end_green >= numcubes) end_green = numcubes - 1 ;
		if (end_blue >= numcubes) end_blue = numcubes - 1 ;
		SampleValueLabel thissv_label = svalues0[i]->getLabel() ;

		for (short i_red = start_red ; i_red <= end_red ; i_red++) {
			for (short i_green = start_green ; i_green <= end_green ; i_green++) {
				for (short i_blue = start_blue ; i_blue <= end_blue ; i_blue++) {
					const vector<BmpRGBSampleValue*> &thiscube = cubes[i_red][i_green][i_blue] ;
					unsigned int thiscube_size = thiscube.size() ;
					for (unsigned int j = 0 ; j < thiscube_size ; j++) {
						if (svalues0[i]->isNeighbour (thiscube[j])) {
							OppNeighs[thissv_label].push_back (thiscube[j]->getLabel()) ;
							OppNeighs[thiscube[j]->getLabel()].push_back (thissv_label) ;
						}
					}

				}
			}
		}
	}
}

void SampleValueOppositeNeighbourhood::calcOppNeighs_wav (const vector<SampleValue*> &svalues)
{
	// partition samples into those with getBit() == 0 and with getBit() == 1
	unsigned long n = svalues.size() ;

	vector<WavPCMSampleValue*> svalues0 ;
	svalues0.reserve (n / 2) ;
	vector<WavPCMSampleValue*> svalues1 ;
	svalues1.reserve (n / 2) ;

	for (unsigned long l = 0 ; l < n ; l++) {
		WavPCMSampleValue *s = dynamic_cast<WavPCMSampleValue*> (svalues[l]) ;
		assert (s != NULL) ;
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

	int r_ub = (int) svalues[0]->getRadius() ; // FIXME - radius aufrunden

	OppNeighs = vector<vector<SampleValueLabel> > (n) ;
	unsigned long n0 = svalues0.size() ;
	unsigned long n1 = svalues1.size() ;
	unsigned long start1 = 0 ;
	for (unsigned long i0 = 0 ; i0 < n0 ; i0++) {
		while ((start1 < n1) && (svalues1[start1]->getValue() < svalues0[i0]->getValue() - r_ub)) {
			start1++ ;
		}
		unsigned long i1 = start1 ;
		while ((i1 < n1) && (svalues1[i1]->getValue() <= svalues0[i0]->getValue() + r_ub)) {
			OppNeighs[svalues0[i0]->getLabel()].push_back (svalues1[i1]->getLabel()) ;
			OppNeighs[svalues1[i1]->getLabel()].push_back (svalues0[i0]->getLabel()) ;
			i1++ ;
		}
	}
}
