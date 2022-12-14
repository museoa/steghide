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

#include "CvrStgFile.h"

/**
 * \class MarkerRestricted
 * \brief a CvrStgFile restricted in length by embedded markers
 *
 * This class is a wrapper around CvrStgFile that restricts the originial file
 * in length by cutting off some space at the beginning and at the end. The amount
 * of cutted space is determined by embedding markers.
 *
 * The original file is partitioned in the following way:
 * original: |--------|-------|---------|--......--|----------|--------|---------|
 * name:      prespace presalt premarker embed area postmarker postsalt postspace
 *
 * A marker is calculated as the MD5 hash of the concatenation of all bits embedded
 * in the salt area with the passphrase. This MD5 hash is then xored to the desired length
 * of the marker (m_SizeMarker) and embedded in the marker area.
 **/
class MarkerRestricted : public CvrStgFile {
	public:
	/**
	 * construct from original file and passphrase and embed markers in original
	 * \param _orig the original file (that will be restricted)
	 * \param _pp the passphrase
	 * \param _prespace the amount of space to be skipped at begin (in samples)
	 * \param _postspace the amount of space to be skipped at end (in samples)
	 *
	 * This constructor is used for embedding.
	 **/
	MarkerRestricted (CvrStgFile* _orig, std::string _pp, UWORD32 _prespace, UWORD32 _postspace) ;

	/**
	 * construct from original and passphrase by searching for markers
	 * \param _orig the original file (that will be restricted)
	 * \param _pp the passphrase
	 *
	 * This constructor is used for extracting.
	 **/
	MarkerRestricted (CvrStgFile* _orig, std::string _pp) ;

	unsigned long getNumSamples (void) const ;
	SampleValue* getSampleValue (const SamplePos pos) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;

	virtual void read (BinaryIO *io) { m_Original->read(io) ; } ;
	virtual void write (void) { m_Original->write() ; } ;
	void transform (const std::string& fn) { m_Original->transform(fn) ; } ;

	virtual std::list<CvrStgFile::Property> getProperties (void) const { return m_Original->getProperties() ; } ;

	virtual std::vector<SampleValueAdjacencyList*> calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const
		{ return m_Original->calcSVAdjacencyLists(svs) ; } ;

	virtual std::vector<MatchingAlgorithm*> getMatchingAlgorithms (Graph* g, Matching* m) const
		{ return m_Original->getMatchingAlgorithms(g,m) ; } ;

	virtual const std::string& getName (void) const
		{ return m_Original->getName() ; } ;

	virtual bool is_std (void) const
		{ return m_Original->is_std() ; } ;

	// FIXME - this is not really correct because the samples that are
	// in the prespace and postspace are also counted, but it is not used
	// for MarkerRestricted anyway -> just to keep the compiler happy
	virtual std::map<SampleKey,unsigned long>* getFrequencies (void)
		{ return m_Original->getFrequencies() ; } ;
		
	private:
	static const unsigned int m_SizeSalt = 64 ;
	static const unsigned int m_SizeMarker = 64 ;
	static const UWORD32 m_SizeSearchSpace = 1000 ; // FIXME - find good value for this...

	CvrStgFile* m_Original ;
	/// the number of skipped samples at begin
	UWORD32 m_PreSpace ;
	/// the number of skipped samples at end
	UWORD32 m_PostSpace ;
	/// the number of samples at begin that are searched for the marker
	UWORD32 m_PreLimit ;
	/// the number of samples at end that are searched for the marker
	UWORD32 m_PostLimit ;
} ;
