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

#ifndef SH_SAMPLEVALUE_H
#define SH_SAMPLEVALUE_H

#include <functional>

#include "common.h"
#include "wrapper_hash_set.h"

class CvrStgFile ;

/**
 * \class SampleValue
 * \brief the value of a sample in a CvrStgFile
 *
 * This is the abstract base class for all AuSampleValue, BmpSampleValue, etc. classes
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
	SampleValue (const CvrStgFile* f) ;

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
	 * calculate the distance between the sample value s and this sample value
	 * \param s a sample value of the same type as this
	 * \return the distance
	 **/
	virtual UWORD32 calcDistance (const SampleValue *s) const = 0 ;

	/**
	 * is the sample value s a neighbour of this sample value ?
	 * \return true iff this and s are neighbours
	 *
	 * This is implemented as (calcDistance() <= Radius) but may be overridden by
	 * derived classes.
	 **/
	virtual bool isNeighbour (const SampleValue* s) const ;

	/**
	 * get the bit that is embedded in this sample value
	 * \return the embedded bit
	 **/
	BIT getBit (void) const
		{ return SBit ; } ;

	/**
	 * get the key for this sample
	 * \return a key which must be different for two different samples values.
	 **/
	UWORD32 getKey (void) const
		{ return Key ; } ;

	/**
	 * two sample values are equal iff their keys are equal
	 **/
	bool operator== (const SampleValue& sv) const { return (getKey() == sv.getKey()) ; } ;
	bool operator!= (const SampleValue& sv) const { return (getKey() != sv.getKey()) ; } ;

	/**
	 * get the number of edges that this sample value contributes to a vertex
	 **/
	unsigned long getNumEdges (void) const { return NumEdges ; } ;
	void setNumEdges (unsigned long ne) { NumEdges = ne ; } ;
	void incNumEdges (void) { NumEdges++ ; } ;
	void decNumEdges (void) ;

	UWORD32 getRadius (void) const { return Radius ; } ;

	void setFile (CvrStgFile* f) { File = f ; } ;
	const CvrStgFile* getFile (void) const { return File ; } ;

	void setLabel (unsigned long l) { Label = l ; } ;
	unsigned long getLabel (void) const { return Label ; } ;

	void print (unsigned short spc = 0) const ;

	protected:
	// FIXME - time/memory: use SBit, Key ?

	/// the bit that is embedded in this sample value - must be set in constructor of derived class
	BIT SBit ;

	/// the key of this sample value - must be different for two different sample values - must be set in constructor of derived class
	UWORD32 Key ;

	/// the radius of all sample values - must be set in constructor of derived class using the setRadius method
	static UWORD32 Radius ;

	/**
	 * set the static variable Radius either to the value specified by the user or to dr
	 * \param dr the default (if no value has been specified by the user)
	 *
	 * This method should be called from the constructor of the derived class
	 * with a class-specific constant for the default radius. The Radius variable
	 * is only set if it contains the value 0 (indicating that it has not been set yet).
	 **/
	void setRadius (UWORD32 dr) ;

	private:
	/// the CvrStgFile this sample value belongs to
	const CvrStgFile* File ; // FIXME - static ?

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


namespace NAMESPACE_SGI
{

struct sgi::hash<SampleValue*> {
	size_t operator() (const SampleValue *s) const
	{
		sgi::hash<unsigned long> h ;
		return h (s->getKey()) ;
	}
} ;

}

#endif // ndef SH_CVRSTGSAMPLE_H
