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

#ifndef SH_SVALUEOPPNEIGHS_H
#define SH_SVALUEOPPNEIGHS_H

#include <vector>

#include "common.h"
#include "GraphAccess.h"
#include "SampleValue.h"

class SampleValueOppositeNeighbourhood : private GraphAccess {
	public:
	SampleValueOppositeNeighbourhood (void) {} ;
	SampleValueOppositeNeighbourhood (Graph* g, const std::vector<SampleValue*>& svalues) ;

	const std::vector<SampleValue*>& operator[] (const SampleValueLabel lbl) const
		{ return OppNeighs[lbl] ; } ;

	const std::vector<SampleValue*>& operator[] (const SampleValue* sv) const
		{ return OppNeighs[sv->getLabel()] ; } ;

	/**
	 * check the integrity of this data structure (only used for debugging and testing)
	 **/
	bool check (bool verbose = false) const ;

	private:
	/**
	 * a strict weak ordering with sv1 < sv2 iff sv1 is nearer to sv (as given in constructor)
	 **/
	class SmallerDistance {
		public:
		SmallerDistance (SampleValue *sv)
			: SrcSampleValue(sv) {} ;

		bool operator() (const SampleValue* sv1, const SampleValue* sv2)
			{ return (SrcSampleValue->calcDistance(sv1) < SrcSampleValue->calcDistance(sv2)) ; } ;

		private:
		SampleValue* SrcSampleValue ;
	} ;

	std::vector<std::vector<SampleValue*> > OppNeighs ;

	void calcOppNeighs_generic (const std::vector<SampleValue*> &svalues) ;
	void calcOppNeighs_rgb (const std::vector<SampleValue*> &svalues) ;
	void calcOppNeighs_wav (const std::vector<SampleValue*> &svalues) ;

	void sortOppNeighs (void) ;
	void quicksort (std::vector<SampleValue*>& oppneighs, float* distances, unsigned int l, unsigned int r) ;

	/**
	 * partition oppneighs/distances into those with distances less than and those with distances greater than and equal to x
	 * \return the index in oppneighs/distances that separates the two
	 **/
	unsigned int partition (std::vector<SampleValue*>& oppneighs, float* distances, unsigned int l, unsigned int r, float x) ;
	void swap (std::vector<SampleValue*>& oppneighs, float* distances, unsigned int i, unsigned int j) ;

	/**
	 * return the smallest integer that is >= x
	 **/
	int roundup (float x) ;

	bool check_size (bool verbose = false) const ;
	bool check_soundness (bool verbose = false) const ;
	bool check_sorted (bool verbose = false) const ;
	bool check_uniqueness (bool verbose = false) const ;
	bool check_completeness (bool verbose = false) const ;
} ;

#endif // ndef SH_SVALUEOPPNEIGHS_H
