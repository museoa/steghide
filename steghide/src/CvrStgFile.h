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

#ifndef SH_CVRSTGFILE_H
#define SH_CVRSTGFILE_H

#include <string>

#include "BinaryIO.h"
#include "CvrStgObject.h"

class SampleValue ;

class CvrStgFile : public CvrStgObject {
	public:
	/**
	 * this function reads the file with name fn and returns
	 * a *File object of the correct type casted to CvrStgFile.
	 **/
	static CvrStgFile *readFile (const std::string& fn) ;

	CvrStgFile (void) ;
	CvrStgFile (BinaryIO *io) ;
	virtual ~CvrStgFile (void) ;

	virtual void read (BinaryIO *io) ;
	virtual void write (void) ;
	void transform (const std::string& fn) ;

	/**
	 * get the name of this cvrstgfile
	 **/
	const std::string& getName (void) const
		{ return getBinIO()->getName() ; } ;

	bool is_std (void) const
		{ return getBinIO()->is_std() ; } ;

	/**
	 * get the capacity of this cvrstgfile
	 * \return the capacity in bytes
	 **/
	unsigned long getCapacity (void) const
		{ return (getNumSamples() / getSamplesPerEBit()) / 8 ; } ;

	virtual unsigned int getSamplesPerEBit (void) const = 0 ;

	/**
	 * get the bit that is embedded in the Sample pos
	 * \param pos the position of the sample
	 * \return the bit that is embedded in the sample corresponding to the given sample position
	 *
	 * This is equivalent to getSample(pos)->getBit().
	 **/
	BIT getSampleBit (const SamplePos pos) const ;

	protected:
	void setBinIO (BinaryIO* io)
		{ BinIO = io ; } ;

	BinaryIO* getBinIO (void) const
		{ return BinIO ; } ;

	private:
	enum FILEFORMAT { UNKNOWN, BMP, WAV, AU, JPEG } ;

	/**
	 * guesses the file format by looking at the first few bytes
	 **/
	static FILEFORMAT guessff (BinaryIO *io) ;

	BinaryIO* BinIO ;
} ;

#endif /* ndef SH_CVRSTGFILE_H */
