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

// FIXME - check name of these ifdefs in bmpsamplevalue.h,etc.
#ifndef SH_SAMPLEVALUE_H
#define SH_SAMPLEVALUE_H

#include <functional>
#include <hash_set>
#include <list>

#include "common.h"
// declared here to prevent circulating #includes
class CvrStgFile ;

/**
 * \class SampleValue
 * \brief the value of a sample in a CvrStgFile
 *
 * This is the abstract base class for all AuSampleValueValue, BmpSampleValueValue, etc. classes
 *
 * For two sample values s1 and s2:
 *
 * s1->calcDistance(s2) == s2->calcDistance(s1) is always true.
 *
 * s1->isNeighbour(s2) == s2->isNeighbour(s1) is always true.
 *
 * s1 and s2 are called opposite if s1->getBit() != s2->getBit()
 *
 * s1 and s2 are called neighbours if s1->isNeighbour(s2) is true
 *
 * s1->getKey() == s2->getKey() iff s1 == s2 
 *
 * s1 == s2 implies s1->getDistance(s2) == 0
 * BUT: s1->getDistance(s2) == 0 does not imply s1 == s2
 *      example: 8-bit bmp palette image - same color value for two different indices
 *
 * s1 == s2 implies s1->getBit() == s2->getBit()
 *
 * s1->getDistance(s2) == 0 implies s1->getBit() == s2->getBit()
 **/
class SampleValue {
	public:
	SampleValue (void) {} ;
	SampleValue (CvrStgFile *f)
		: File(f) {} ;

	/**
	 * is the sample value s a neighbour of this sample value ?
	 * \return true iff this and s are neighbours
	 **/
	virtual bool isNeighbour (const SampleValue *s) const = 0 ;

	/**
	 * get the nearest (with the least distance) opposite sample value that can be used in this file
	 * \return the nearest opposite sample value
	 *
	 * If two or more opposite samples values have equal distance each of them should
	 * be returned with equal probability.
	 *
	 * The returned SampleValue object should be deleted by the caller.
	 *
	 * If an opposite neighbour exists then getNearestOppositeSample() will return a neighbour.
	 **/
	virtual SampleValue* getNearestOppositeSampleValue (void) const = 0 ;

	/**
	 * calculate the distance between the sample value s and this
	 * \param s a sample value of the same type as this
	 * \return the distance
	 **/
	virtual float calcDistance (const SampleValue *s) const = 0 ;

	/**
	 * get the bit that is embedded in this sample value
	 * \return the embedded bit
	 **/
	Bit getBit (void) const
		{ return SBit ; } ;

	/**
	 * get the key for this sample
	 * \return a key which must be different for two different samples values.
	 **/
	unsigned long getKey (void) const
		{ return Key ; } ;

	/**
	 * two sample values are equal iff their keys are equal
	 **/
	bool operator== (const SampleValue& sv) const
		{ return (getKey() == sv.getKey()) ; } ;

	bool operator!= (const SampleValue& sv) const
		{ return (getKey() != sv.getKey()) ; } ;

	virtual float getRadius (void) const = 0 ;

	/**
	 * get the number of edges that this sample value contributes to a vertex
	 **/
	unsigned long getNumEdges (void) const
		{ return NumEdges ; } ;

	void setNumEdges (unsigned long ne)
		{ NumEdges = ne ; } ;

	void decNumEdges (void) ;

	void incNumEdges (void)
		{ NumEdges++ ; } ;

	CvrStgFile *getFile (void) const
		{ return File ; } ;

	void setFile (CvrStgFile *f)
		{ File = f ; } ;

	unsigned long getLabel (void) const
		{ return Label ; } ;

	void setLabel (unsigned long l)
		{ Label = l ; } ;

	protected:
	/// the bit that is embedded in this sample value - must be set in constructor of derived class
	Bit SBit ;

	/// the key of this sample value - must be different for two different sample values - must be set in constructor of derived class
	unsigned long Key ;

	private:
	CvrStgFile *File ;
	unsigned long Label ;

	/// the number of edges that are added to a vertex if this sample value is added to it
	unsigned long NumEdges ;
} ;

struct SampleValuesEqual : std::binary_function<SampleValue*, SampleValue*, bool> {
	bool operator() (const SampleValue* s1, const SampleValue *s2) const
	{
		return (*s1 == *s2) ;
	}
} ;

struct hash<SampleValue*> {
	size_t operator() (const SampleValue *s) const
	{
		hash<unsigned long> h ;
		return h (s->getKey()) ;
	}
} ;

#endif // ndef SH_CVRSTGSAMPLE_H
