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
#include "graphaccess.h"
#include "samplevalue.h"

class SampleValueOppositeNeighbourhood : private GraphAccess {
	public:
	SampleValueOppositeNeighbourhood (void) {} ;
	SampleValueOppositeNeighbourhood (Graph *g, const vector<SampleValue*> &svalues) ;

	const vector<SampleValue*>& operator[] (const SampleValueLabel lbl) const
		{ return OppNeighs[lbl] ; } ;

	const vector<SampleValue*>& operator[] (const SampleValue* sv) const
		{ return OppNeighs[sv->getLabel()] ; } ;

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

	vector<vector<SampleValue*> > OppNeighs ;

	void calcOppNeighs_generic (const vector<SampleValue*> &svalues) ;
	void calcOppNeighs_rgb (const vector<SampleValue*> &svalues) ;
	void calcOppNeighs_wav (const vector<SampleValue*> &svalues) ;

	/**
	 * return the smallest integer that is >= x
	 **/
	int roundup (float x) ;
} ;

#endif // ndef SH_SVALUEOPPNEIGHS_H
