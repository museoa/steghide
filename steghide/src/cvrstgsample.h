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

#ifndef SH_CVRSTGSAMPLE_H
#define SH_CVRSTGSAMPLE_H

#include <functional>
#include <hash_set>
#include <list>

#include "common.h"
// declared here to prevent circulating #includes
class CvrStgFile ;

/**
 * \class CvrStgSample
 * \brief a sample in a CvrStgFile
 *
 * This is the abstract base class for all AuSample, BmpSample, etc. classes
 *
 * For two samples s1 and s2:
 *
 * s1->calcDistance(s2) == s2->calcDistance(s1) is always true.
 *
 * s1->isNeighbour(s2) == s2->isNeighbour(s1) is always true.
 *
 * s1 and s2 are called opposite if s1->getBit() != s2->getBit()
 *
 * s1 and s2 are called neighbours if s1->isNeighbour(s2) is true
 *
 * s1->getKey() == s2->getKey() iff SamplesEqual (s1, s2) 
 *
 * SamplesEqual (s1, s2) implies s1->getDistance(s2) == 0
 * BUT: s1->getDistance(s2) == 0 does not imply SamplesEqual (s1, s2)
 *      example: 8-bit bmp palette image - same color value for two different indices
 *
 * SamplesEqual (s1, s2) implies s1->getBit() == s2->getBit()
 *
 * s1->getDistance(s2) == 0 implies s1->getBit() == s2->getBit()
 **/
class CvrStgSample {
	public:
	CvrStgSample (void) ;
	CvrStgSample (CvrStgFile *f) ;

	/**
	 * is the sample s a neighbour of this sample ?
	 * \return true if this and s are neighbours
	 **/
	virtual bool isNeighbour (CvrStgSample *s) const = 0 ;

	/**
	 * get the neighbour-samples that are opposite to this one
	 * \return the list of CvrStgSamples which are opposite neighbours of this CvrStgSample.
	 *
	 * getOppositeNeighbours returns those samples that are: 1. neighbours, 2. opposite and 3. can
	 * be used in this file. It does not matter if these samples really occure in this file.
	 * A superset of the opposite neighbours which occur in the file is returned.
	 **/
	virtual list<CvrStgSample*> *getOppositeNeighbours (void) const = 0 ;

	/**
	 * get the nearest (with the least distance) opposite sample that can be used in this file
	 * \return the nearest opposite sample
	 *
	 * If two or more opposite samples have equal distance each of them should
	 * be returned with equal probability.
	 *
	 * If an opposite neighbour exists then getNearestOppositeSample() will return a neighbour.
	 **/
	virtual CvrStgSample* getNearestOppositeSample (void) const = 0 ;

	/**
	 * calculate the distance between the value of s and the value of this
	 * \param s a sample value of the same type as this
	 * \return the distance
	 **/
	virtual float calcDistance (CvrStgSample *s) const = 0 ;

	/**
	 * get the bit that is embedded in this sample (usually the xor of the lsbs)
	 * \return the embedded bit
	 **/
	Bit getBit (void) const ;

	/**
	 * get the key for this sample
	 * \return a key which must be different for two different samples.
	 **/
	unsigned long getKey (void) const ; 

	CvrStgFile *getFile (void) const ;
	void setFile (CvrStgFile *f) ;

	unsigned long getLabel (void) const ;
	void setLabel (unsigned long l) ;

	protected:
	// the bit that is embedded in this sample - must be set in constructor of derived class
	Bit SBit ;
	// the key of this sample - must be different for two different samples - must be set in constructor of derived class
	unsigned long Key ;
#if 0
	/**
	 * get the default value for the neighbourhood radius
	 **/
	virtual float getDefaultRadius (void) const = 0 ;

	/**
	 * every pair of samples whose distance (as calculated by calcDistance)
	 * is smaller or equal than this value are neighbours
	 **/
	static float Radius ;

	private:
	void setRadius (void) ;
#endif
	private:
	CvrStgFile *File ;
	unsigned long Label ;
} ;

struct SamplesEqual : binary_function<CvrStgSample*, CvrStgSample*, bool> {
	bool operator() (CvrStgSample* s1, CvrStgSample *s2) const
	{
		return (s1->getKey() == s2->getKey()) ;
	}
} ;

struct hash<CvrStgSample*> {
	size_t operator() (const CvrStgSample *s) const
	{
		hash<unsigned long> h ;
		return h (s->getKey()) ;
	}
} ;

#endif // ndef SH_CVRSTGSAMPLE_H
