/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>
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

#include "BitString.h"
#include "MarkerRestricted.h"
#include "MHashPP.h"
#include "SampleValue.h"

MarkerRestricted::MarkerRestricted (CvrStgFile* _orig, std::string _pp, UWORD32 _prespace, UWORD32 _postspace)
	: m_Original(_orig), m_PreSpace(_prespace), m_PostSpace(_postspace)
{
	setSamplesPerVertex( m_Original->getSamplesPerVertex() ) ;
	setRadius( m_Original->getRadius() ) ;
	setEmbValueModulus( m_Original->getEmbValueModulus() ) ;
	setBinIO( NULL ) ;

	{ // create and embed premarker
		BitString salt ;
		for (unsigned int i = 0 ; i < m_SizeSalt ; i++) {
			salt.append( m_Original->getSampleValue( m_PreSpace + i )->getEmbeddedBit() ) ;
		}
		MHashPP hash( MHASH_MD5 ) ;
		hash << salt << _pp << MHashPP::endhash ;
		BitString marker = hash.getHashBits() ;
		marker.xorreduce( m_SizeMarker ) ;

		for (unsigned int i = 0 ; i < m_SizeMarker ; i++) {
			unsigned long pos = m_PreSpace + m_SizeSalt + i ;
			SampleValue* oldsv = m_Original->getSampleValue( pos ) ;
			m_Original->replaceSample( pos, oldsv->getNearestBitTargetSampleValue( marker[i] )) ;
		}
	}

	{ // create and embed postmarker
		BitString postsalt ;
		for (unsigned int i = 0 ; i < m_SizeSalt ; i++) {
			postsalt.append( m_Original->getSampleValue( m_Original->getNumSamples() - m_PostSpace - i )->getEmbeddedBit() ) ;
		}
		MHashPP hash( MHASH_MD5 ) ;
		hash << postsalt << _pp << MHashPP::endhash ;
		BitString marker = hash.getHashBits() ;
		marker.xorreduce( m_SizeMarker ) ;

		for (unsigned int i = 0 ; i < m_SizeMarker ; i++) {
			unsigned long pos = m_Original->getNumSamples() - m_PostSpace - m_SizeSalt - i ;
			SampleValue* oldsv = m_Original->getSampleValue( pos ) ;
			m_Original->replaceSample( pos, oldsv->getNearestBitTargetSampleValue( marker[i] )) ;
		}
	}
}

MarkerRestricted::MarkerRestricted (CvrStgFile* _orig, std::string _pp)
	: m_Original( _orig )
{
	setSamplesPerVertex( m_Original->getSamplesPerVertex() ) ;
	setRadius( m_Original->getRadius() ) ;
	setEmbValueModulus( m_Original->getEmbValueModulus() ) ;
	setBinIO( NULL ) ;

	m_PreLimit = m_SizeSearchSpace ;
	m_PostLimit = m_Original->getNumSamples() - m_SizeSearchSpace - 1 ;

	{ // search premarker
		BitString searchframe ;
		UWORD32 nextpos = 0 ;
		for ( ; nextpos < m_SizeSalt + m_SizeMarker ; nextpos++) {
			searchframe.append( m_Original->getSampleValue( nextpos )) ;
		}
		bool found = false ;
		while (!found) {
			if (nextpos >= m_PreLimit) {
				if (m_Original->is_std()) {
					throw SteghideError (_("could not find marker in the first %lu samples of standard input."), m_PreLimit) ;
				}
				else {
					throw SteghideError (_("could not find marker in the first %lu samples of \"%s\"."), m_PreLimit, m_Original->getName().c_str()) ;
				}
			}
			BitString salt = searchframe.getBits( 0, m_SizeSalt ) ;
			MHashPP hash( MHASH_MD5 ) ;
			hash << salt << _pp << MHashPP::endhash ;
			BitString shouldbemarker = hash.getHashBits().xorreduce( m_SizeMarker ) ;
			BitString embeddedmarker = searchframe.getBits( m_SizeSalt, m_SizeMarker ) ;

			if (embeddedmarker == shouldbemarker) {
				found = true ;
				m_PreSpace = nextpos - m_SizeSalt - m_SizeMarker ;
			}
			else { // move search frame one bit further
				searchframe.cutBits( 0, 1 ) ;
				searchframe.append( m_Original->getSampleValue( nextpos )->getEmbeddedBit() ) ;
				nextpos++ ;
			}
		}
	}

	{ // search postmarker
		BitString searchframe ;
		UWORD32 nextpos = m_Original->getNumSamples() ;
		for ( ; nextpos > m_Original->getNumSamples() - m_SizeSalt - m_SizeMarker ; nextpos--) {
			searchframe.append( m_Original->getSampleValue( nextpos )) ;
		}
		bool found = false ;
		while (!found) {
			if (nextpos <= m_PostLimit) {
				if (m_Original->is_std()) {
					throw SteghideError (_("could not find marker in the last %lu samples of standard input."), m_PostLimit) ;
				}
				else {
					throw SteghideError (_("could not find marker in the last %lu samples of \"%s\"."), m_PostLimit, m_Original->getName().c_str()) ;
				}
			}
			BitString salt = searchframe.getBits( 0, m_SizeSalt ) ;
			MHashPP hash( MHASH_MD5 ) ;
			hash << salt << _pp << MHashPP::endhash ;
			BitString shouldbemarker = hash.getHashBits().xorreduce( m_SizeMarker ) ;
			BitString embeddedmarker = searchframe.getBits( m_SizeSalt, m_SizeMarker ) ;

			if (embeddedmarker == shouldbemarker) {
				found = true ;
				m_PostSpace = (m_Original->getNumSamples() - nextpos) - m_SizeSalt - m_SizeMarker - 1 ;
			}
			else { // move search frame one bit further
				searchframe.cutBits( 0, 1 ) ;
				searchframe.append( m_Original->getSampleValue( nextpos )->getEmbeddedBit() ) ;
				nextpos-- ;
			}
		}
	}
}

unsigned long MarkerRestricted::getNumSamples() const
{
	return (m_Original->getNumSamples() - m_PreSpace - m_PostSpace - 2 * (m_SizeSalt + m_SizeMarker)) ;
}

SampleValue* MarkerRestricted::getSampleValue (const SamplePos pos) const
{
	return m_Original->getSampleValue( m_PreSpace + m_SizeSalt + m_SizeMarker + pos ) ;
}

void MarkerRestricted::replaceSample (const SamplePos pos, const SampleValue* s)
{
	m_Original->replaceSample( m_PreSpace + m_SizeSalt + m_SizeMarker + pos, s ) ;
}
