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

#include "BitString.h"
#include "CvrStgFile.h"
#include "EmbData.h"
#include "Extractor.h"
#include "SampleValue.h"
#include "Selector.h"
#include "common.h"

Extractor::Extractor ()
{
	StegoFileName = Args.StgFn.getValue() ;
	ExtractFileName = Args.ExtFn.getValue() ;
}

void Extractor::extract ()
{
	CvrStgFile *stgfile = CvrStgFile::readFile (StegoFileName) ;
	EmbData embdata (EmbData::EXTRACT) ;

	Selector sel (stgfile->getNumSamples(), Args.Passphrase.getValue()) ;

	unsigned int sam_ebit = stgfile->getSamplesPerEBit() ;
	unsigned long ebit_idx = 0 ;
	while (!embdata.finished()) {
		unsigned long bitsneeded = embdata.getNumBitsNeeded() ;
		if (ebit_idx + bitsneeded > stgfile->getNumSamples()) {
			if (stgfile->is_std()) {
				throw SteghideError (_("the stego data from standard input is too short to contain the embedded data (file corruption ?).")) ;
			}
			else {
				throw SteghideError (_("the stego file \"%s\" is too short to contain the embedded data (file corruption ?)."), stgfile->getName().c_str()) ;
			}
		}
		BitString bits ;
		for (unsigned long i = 0 ; i < bitsneeded ; i++, ebit_idx++) {
			BIT xorresult = 0 ;
			for (unsigned int j = 0 ; j < sam_ebit ; j++) {
				xorresult ^= stgfile->getSampleBit (sel[(ebit_idx * sam_ebit) + j]) ;
			}
			bits.append (xorresult) ;
		}
		embdata.addBits (bits) ;
	}

	embdata.write() ;
}
