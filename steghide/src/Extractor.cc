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

#include <string>

#include "common.h"
#include "CvrStgFile.h"
#include "EmbData.h"
#include "Extractor.h"
#include "SampleValue.h"
#include "Permutation.h"

Extractor::Extractor ()
{
	StegoFileName = Args.StgFn.getValue() ;
	ExtractFileName = Args.ExtFn.getValue() ;
}

void Extractor::extract ()
{
	CvrStgFile *stgfile = CvrStgFile::readFile (StegoFileName) ;
	EmbData embdata (EmbData::EXTRACT) ;

	Permutation perm (stgfile->getNumSamples(), Args.Passphrase.getValue()) ;

	unsigned int sam_ebit = stgfile->getSamplesPerEBit() ;
	while (!embdata.finished()) {
		unsigned long bitsneeded = embdata.getNumBitsNeeded() ;
		BitString bits ;
		for (unsigned long i = 0 ; i < bitsneeded ; i++) {
			BIT xorresult = 0 ;
			for (unsigned int j = 0 ; j < sam_ebit ; j++) {
				xorresult ^= stgfile->getSampleBit (*perm) ;
				++perm ;
			}
			bits.append (xorresult) ;
		}
		embdata.addBits (bits) ;
	}

	embdata.write() ;
}
