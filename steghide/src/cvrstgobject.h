/*
 * steghide 0.4.6 - a steganography program
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

#ifndef SH_CVRSTGOBJECT_H
#define SH_CVRSTGOBJECT_H

/**
 * \class CvrStgObject
 * \brief an object that can hold embedded data
 *
 * This abstract base class provides an interface for every class
 * that is able to hold embedded data.
 **/
class CvrStgObject {
	public:
	/**
	 * return the capacity of this object
	 * \return the number of bits that can be embedded in this object (capacity)
	 **/
	virtual unsigned long getCapacity (void) const = 0 ;

	/**
	 * embed a bit
	 * \param pos the position where the bit should be embedded (must be in 0...getCapacity() - 1)
	 * \param bit the bit to embed (must be 0 or 1)
	 *
	 * The derived class should check the conditions given above in its embedBit Implementation.
	 **/
	virtual void embedBit (unsigned long pos, int bit) = 0 ;

	/**
	 * extract a bit
	 * \param pos the position from where the bit should be extracted (must be in 0...getCapacity() - 1)
	 * \return the extracted bit (0 or 1)
	 *
	 * The derived class should check the conditions given above in its extractBit Implementation.
	 **/
	virtual int extractBit (unsigned long pos) const = 0 ;
} ;

#endif //ndef SH_CVRSTGOBJECT_H
