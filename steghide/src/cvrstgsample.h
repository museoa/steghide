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
	CvrStgSample (void) : File(NULL) {} ;
	CvrStgSample (CvrStgFile *f) : File(f) {} ;

	/**
	 * is s a neighbour of this sample ?
	 * \param s a sample value of the same type as this
	 * \return true iff s is a neighbour of this sample (s->is_Neighbour(s) is true)
	 *
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	bool is_Neighbour (CvrStgSample *s) ;

	/**
	 * is the value of s equal to the value of this ?
	 * \param s a sample value of the same type as this
	 * \return true iff the values are equal
	 *
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	bool is_equal (CvrStgSample *s) ;

	/**
	 * get the bit that is embedded in this sample (usually the xor of the lsbs)
	 * \return the embedded bit
	 **/
	virtual Bit getBit (void) = 0 ;

	/**
	 * calculate the distance between the value of s and the value of this
	 * \param s a sample value of the same type as this
	 * \return the distance
	 *
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	virtual float calcDistance (CvrStgSample *s) = 0 ;

	/**
	 * get the nearest (with the least distance) opposite neighbour
	 * \return the nearest opposite neighbour
	 *
	 * Two samples s1 and s2 are called opposite here if the xor of all sbits in s1
	 * is not equal to the xor of all sbits in s2.
	 *
	 * If two or more opposite Neigbours have equal distance each of them should
	 * be returned with equal probability.
	 **/
	virtual CvrStgSample* getNearestOppositeNeighbour (void) = 0 ;

	CvrStgFile *getFile (void) ;
	void setFile (CvrStgFile *f) ;

	// FIXME - ev. zur Konstruktion der unique sample value list einen Baum verwenden - dann auch Ordnung auf samles nötig
	private:
	CvrStgFile *File ;
} ;

#endif // ndef SH_CVRSTGSAMPLE_H
