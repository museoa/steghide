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
#include "error.h"

EmbData* Extractor::extract ()
{
	Globs.TheCvrStgFile = CvrStgFile::readFile (StegoFileName) ;
	EmbData* embdata = new EmbData (EmbData::EXTRACT, Passphrase) ;

	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Passphrase) ;

	unsigned int sam_ebit = Globs.TheCvrStgFile->getSamplesPerVertex() ;
	unsigned long ebit_idx = 0 ;
	while (!embdata->finished()) {
		unsigned long bitsneeded = embdata->getNumBitsNeeded() ;
		if (ebit_idx + bitsneeded > Globs.TheCvrStgFile->getNumSamples()) {
			if (Globs.TheCvrStgFile->is_std()) {
				throw CorruptDataError (_("the stego data from standard input is too short to contain the embedded data.")) ;
			}
			else {
				throw CorruptDataError (_("the stego file \"%s\" is too short to contain the embedded data."), Globs.TheCvrStgFile->getName().c_str()) ;
			}
		}
		BitString bits ;
		for (unsigned long i = 0 ; i < bitsneeded ; i++, ebit_idx++) {
			EmbValue ev = 0 ;
			for (unsigned int j = 0 ; j < sam_ebit ; j++) {
				ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (sel[(ebit_idx * sam_ebit) + j])) % Globs.TheCvrStgFile->getEmbValueModulus() ;
			}
			bits.appendNAry (Globs.TheCvrStgFile->getEmbValueModulus(), ev) ;
		}
		embdata->addBits (bits) ;
	}

	return embdata ;
}
