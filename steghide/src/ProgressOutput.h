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

#ifndef SH_PROGRESSOUTPUT_H
#define SH_PROGRESSOUTPUT_H

#include <string>

/**
 * \class ProgressOutput
 * \brief prints the progress to stdout
 **/
class ProgressOutput {
	public:
	/**
	 * create a ProgressOutput object
	 * \param uf the UpdateFrequency
	 **/
	ProgressOutput (unsigned short uf) ;

	/**
	 * update the output (taking UpdateFrequency into account) with rate as percentage
	 **/
	void update (float rate) ;

	/**
	 * print "done" instead of the percentage and do a carriage return
	 **/
	void done (void) ;

	/**
	 * set the UpdateFrequency
	 **/
	void setUpdateFrequency (unsigned short uf)
		{ UpdateFrequency = uf ; } ;

	void setPrecision (unsigned short p) ;

	private:
	/// if UpdateFrequency is k, then the output will be done with every k-th call of update
	unsigned short UpdateFrequency ;
	unsigned short Counter ;
	std::string EmbString ;
	std::string CvrString ;

	unsigned short makePercent (float r) ;
} ;

#endif // ndef SH_PROGRESSOUTPUT_H
