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
 **/
class CvrStgSample {
	public:
	CvrStgSample (void) : File(NULL), Label(0) {} ;
	CvrStgSample (CvrStgFile *f) : File(f), Label(0) {} ;

	/**
	 * get the bit that is embedded in this sample (usually the xor of the lsbs)
	 * \return the embedded bit
	 **/
	virtual Bit getBit (void) const = 0 ;

	/**
	 * is the sample s a neighbour of this sample ?
	 * \return true if this and s are neighbours
	 **/
	virtual bool isNeighbour (CvrStgSample *s) const = 0 ;

	/**
	 * get the neighbour-samples that are opposite to this one
	 * \return the (complete) list of CvrStgSamples which are opposite neighbours of this CvrStgSample.
	 *
	 * Two samples s1 and s2 are called opposite here if s1->getBit() != s2->getBit()
	 *
	 * This function does not care if the returned samples really occure in the
	 * CvrStgFile (it returns the _potential_ opposite neighbours).
	 **/
	virtual list<CvrStgSample*> *getOppositeNeighbours (void) const = 0 ;

	/**
	 * get the nearest (with the least distance) opposite neighbour
	 * \return the nearest opposite neighbour
	 *
	 * Two samples s1 and s2 are called opposite here if s1->getBit() != s2->getBit()
	 *
	 * If two or more opposite Neigbours have equal distance each of them should
	 * be returned with equal probability.
	 **/
	virtual CvrStgSample* getNearestOppositeNeighbour (void) const = 0 ;

	/**
	 * calculate the distance between the value of s and the value of this
	 * \param s a sample value of the same type as this
	 * \return the distance
	 *
	 * The distance of two samples is 0 iff the samples are equal.
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	virtual float calcDistance (CvrStgSample *s) const = 0 ;

	/**
	 * get the key for this sample
	 * \return a key which must(!) be different for two different samples.
	 *
	 * There must not be two samples s1, s2 with !(SamplesEqual(s1,s2)) && (s1->getKey() == s2->getKey()).
	 **/
	virtual unsigned long getKey (void) const = 0 ;

	CvrStgFile *getFile (void) const ;
	void setFile (CvrStgFile *f) ;

	unsigned long getLabel (void) const ;
	void setLabel (unsigned long l) ;

	private:
	CvrStgFile *File ;
	unsigned long Label ;
} ;

// FIXME - 8bit palette - if two distinct palette entries contain the same color, these two samples are equal - !?
struct SamplesEqual : binary_function<CvrStgSample*, CvrStgSample*, bool> {
	bool operator() (CvrStgSample* s1, CvrStgSample *s2) const
	{
		return (s1->calcDistance(s2) == 0.0) ;
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
