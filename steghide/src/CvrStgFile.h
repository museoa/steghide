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

#include <list>
#include <string>
#include <map>

#include "BinaryIO.h"
#include "CvrStgObject.h"

class SampleValue ;

/**
 * \class CvrStgFile
 * \brief a cover-/stego-file
 *
 * file-format specific constants are handled as follows:
 * CvrStgFile contains a protected set-function (e.g. setSamplesPerEBit), a public
 * get-function (e.g. getSamplesPerEBit() const) and a private variable. The public get
 * function does nothing else than returning the private variable, which must be set
 * as soon as possible (if it is not set, it will contain a null value set in CvrStgFile::CvrStgFile).
 **/
class CvrStgFile : public CvrStgObject {
	public:
	/**
	 * this function reads the file with name fn and returns
	 * a *File object of the correct type casted to CvrStgFile.
	 **/
	static CvrStgFile *readFile (const std::string& fn) ;

	class Property : private std::pair<std::string,std::string> {
		public:
		Property (std::string key, std::string value)
			: std::pair<std::string,std::string> (key, value) {} ;

		std::string getKey (void) const
			{ return first ; } ;

		std::string getValue (void) const
			{ return second ; } ;
	} ;

	CvrStgFile (void) ;
	virtual ~CvrStgFile (void) ;

	virtual void read (BinaryIO *io) ;
	virtual void write (void) ;
	void transform (const std::string& fn) ;

	virtual std::list<Property> getProperties (void) const = 0 ;

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

	/**
	 * get the capacity as a human-readable string
	 **/
	std::string getHRCapacity (void) const ;

	/**
	 * get the number of samples per embedded bit (this is a file-format specific constant)
	 **/
	unsigned short getSamplesPerEBit (void) const
		{ return SamplesPerEBit ; } ;
	/**
	 * get the neighbourhood radius (this is a file-format specific constant)
	 **/
	UWORD32 getRadius (void) const
		{ return Radius ; } ;

	/**
	 * get the bit that is embedded in the Sample pos
	 * \param pos the position of the sample
	 * \return the bit that is embedded in the sample corresponding to the given sample position
	 *
	 * This is equivalent to getSample(pos)->getBit().
	 **/
	BIT getSampleBit (const SamplePos pos) const ;

#ifdef DEBUG
	/**
	 * return a map that contains the absolute sample frequencies indexed by sample key
	 **/
	virtual std::map<SampleKey,unsigned long>* getFrequencies (void) ;
	/**
	 * print the frequency table freqs
	 *
	 * This method is meant to be overriden in *File to provide a file-format specific way
	 * of printing a frequency table.
	 **/
	virtual void printFrequencies (const std::map<SampleKey,unsigned long>& freqs) ;
#endif

	protected:
	void setSamplesPerEBit (unsigned short spebit)
		{ SamplesPerEBit = spebit ; } ;

	/**
	 * set Radius to r unless Args.Radius is set (set Radius to Args.Radius.getValue() then)
	 **/
	void setRadius (UWORD32 r) ;
	
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

	unsigned short SamplesPerEBit ;
	UWORD32 Radius ;
} ;

#endif /* ndef SH_CVRSTGFILE_H */
