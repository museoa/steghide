/*
 * steghide 0.5.1 - a steganography program
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

#include "binaryio.h"
#include "common.h"
#include "jpegbase.h"

//
// class JpegObject
//
JpegObject::JpegObject ()
{
	setParent (NULL) ;
}

JpegObject::JpegObject (JpegObject *p)
{
	setParent (p) ;
}

JpegObject *JpegObject::getParent ()
{
	assert (parent) ;
	return parent ;
}

bool JpegObject::issetParent ()
{
	return (parent != NULL) ;
}

void JpegObject::setParent (JpegObject *p)
{
	parent = p ;
}

void JpegObject::splitByte (unsigned char byte, unsigned char *high, unsigned char *low)
{
	*high = (0xF0 & byte) >> 4 ;
	*low = (0x0F & byte) ;
}

unsigned char JpegObject::mergeByte (unsigned char high, unsigned char low)
{
	assert (high <= 15) ;
	assert (low <= 15) ;

	return ((high << 4) | low) ;
}

//
// class JpegElement
//
JpegElement::JpegElement ()
	: JpegObject()
{
	marker_isset = false ;
}

JpegElement::JpegElement (JpegMarker m)
	: JpegObject()
{
	setMarker (m) ;
}

JpegElement::JpegElement (JpegObject *p, JpegMarker m)
	: JpegObject (p)
{
	setMarker (m) ;
}

JpegElement::~JpegElement ()
{
}

void JpegElement::read (BinaryIO *io)
{
}

void JpegElement::write (BinaryIO *io)
{
	io->write8 (0xFF) ;
	io->write8 ((unsigned char) getMarker()) ;
}

JpegMarker JpegElement::getMarker ()
{
	assert (marker_isset) ;
	return marker ;
}

void JpegElement::setMarker (JpegMarker m)
{
	marker = m ;
	marker_isset = true ;
}

//
// class JpegSegment
//
JpegSegment::JpegSegment ()
	: JpegElement()
{
	length_isset = false ;
}

JpegSegment::JpegSegment (JpegMarker m)
	: JpegElement (m)
{
	length_isset = false ;
}

JpegSegment::JpegSegment (JpegObject *p, JpegMarker m)
	: JpegElement (p, m)
{
	length_isset = false ;
}

JpegSegment::~JpegSegment ()
{
}

void JpegSegment::read (BinaryIO *io)
{
	setLength (io->read16_be()) ;
}

void JpegSegment::write (BinaryIO *io)
{
	JpegElement::write (io) ;
	io->write16_be (getLength()) ;
}

unsigned int JpegSegment::getLength (void)
{
	assert (length_isset) ;
	return length ;
}

void JpegSegment::setLength (unsigned int l)
{
	length = l ;
	length_isset = true ;
}

//
// class JpegContainer
//
JpegContainer::JpegContainer ()
	: JpegObject(), CvrStgObject()
{
}

JpegContainer::JpegContainer (JpegObject *p)
	: JpegObject (p), CvrStgObject()
{
}

JpegContainer::~JpegContainer ()
{
	for (vector<JpegObject*>::iterator i = jpegobjs.begin() ; i != jpegobjs.end() ; i++) {
		delete (*i) ;
	}
}

vector<JpegObject*> JpegContainer::getJpegObjects ()
{
	return jpegobjs ;
}

vector<CvrStgObject*> JpegContainer::getCvrStgObjects ()
{
	return cvrstgobjs ;
}

void JpegContainer::appendObj(JpegObject *o)
{
	jpegobjs.push_back (o) ;
	if (CvrStgObject *cso = dynamic_cast<CvrStgObject*>(o)) {
		cvrstgobjs.push_back (cso) ;
	}
}

void JpegContainer::clearObjs()
{
	jpegobjs.clear() ;
	cvrstgobjs.clear() ;
}

void JpegContainer::read (BinaryIO *io)
{
	// reading is done in derived classes
}

void JpegContainer::write (BinaryIO *io)
{
	// writing is only done here, not in derived classes
	for (vector<JpegObject*>::iterator i = jpegobjs.begin() ; i != jpegobjs.end() ; i++) {
		(*i)->write (io) ;
	}
}

unsigned long JpegContainer::getNumSamples ()
{
	unsigned long sum = 0 ;
	for (vector<CvrStgObject*>::const_iterator i = cvrstgobjs.begin() ; i != cvrstgobjs.end() ; i++) {
		sum += (*i)->getNumSamples() ;
	}
	return sum ;
}

void JpegContainer::replaceSample (SamplePos pos, CvrStgSample *s)
{
	CvrStgObject *cso = calcCvrStgObject (&pos) ;
	cso->replaceSample (pos, s) ;
}

CvrStgSample *JpegContainer::getSample (SamplePos pos)
{
	CvrStgObject *cso = calcCvrStgObject (&pos) ;
	return cso->getSample (pos) ;
}

CvrStgObject *JpegContainer::calcCvrStgObject (SamplePos *pos) const
{
	vector<CvrStgObject*>::const_iterator i = cvrstgobjs.begin() ;
	unsigned long curNumSamples = (*i)->getNumSamples() ;
	while (*pos >= curNumSamples) {
		*pos -= curNumSamples ;
		i++ ;
		assert (i != cvrstgobjs.end()) ;
		curNumSamples = (*i)->getNumSamples() ;
	}
	return *i ;
}
