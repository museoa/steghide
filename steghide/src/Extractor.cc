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
	Globs.TheCvrStgFile = CvrStgFile::readFile (StegoFileName) ;
	EmbData embdata (EmbData::EXTRACT, Args.Passphrase.getValue()) ;

	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Args.Passphrase.getValue()) ;

	unsigned int sam_ebit = Globs.TheCvrStgFile->getSamplesPerEBit() ;
	unsigned long ebit_idx = 0 ;
	while (!embdata.finished()) {
		unsigned long bitsneeded = embdata.getNumBitsNeeded() ;
		if (ebit_idx + bitsneeded > Globs.TheCvrStgFile->getNumSamples()) {
			if (Globs.TheCvrStgFile->is_std()) {
				throw SteghideError (_("the stego data from standard input is too short to contain the embedded data (file corruption ?).")) ;
			}
			else {
				throw SteghideError (_("the stego file \"%s\" is too short to contain the embedded data (file corruption ?)."), Globs.TheCvrStgFile->getName().c_str()) ;
			}
		}
		BitString bits ;
		for (unsigned long i = 0 ; i < bitsneeded ; i++, ebit_idx++) {
			BIT xorresult = 0 ;
			for (unsigned int j = 0 ; j < sam_ebit ; j++) {
				xorresult ^= Globs.TheCvrStgFile->getSampleBit (sel[(ebit_idx * sam_ebit) + j]) ;
			}
			bits.append (xorresult) ;
		}
		embdata.addBits (bits) ;
	}

	// write data
	std::string fn ;
	if (Args.ExtFn.is_set()) {
		if (Args.ExtFn.getValue() == "") {
			// write extracted data to stdout
			fn = "" ;
		}
		else {
			// file name given by extracting user overrides embedded file name
			fn = Args.ExtFn.getValue() ;
		}
	}
	else {
		// write extracted data to file with embedded file name
		myassert (Args.ExtFn.getValue() == "") ;
		fn = embdata.getFileName() ;
		if (fn.length() == 0) {
			throw SteghideError (_("please specify a file name for the extracted data (there is no name embedded in the stego file).")) ;
		}
	}

	BinaryIO io (fn, BinaryIO::WRITE) ;
	std::vector<BYTE> data = embdata.getData() ;
	for (std::vector<BYTE>::iterator i = data.begin() ; i != data.end() ; i++) {
		io.write8 (*i) ;
	}
	io.close() ;
}
