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
#include "jpegrestart.h"

JpegDefineRestartInterval::JpegDefineRestartInterval ()
	: JpegSegment (JpegElement::MarkerDRI)
{
	restartinterval = 0 ;
}

JpegDefineRestartInterval::JpegDefineRestartInterval (BinaryIO *io)
	: JpegSegment (JpegElement::MarkerDRI)
{
	restartinterval = 0 ;
	read (io) ;
}

JpegDefineRestartInterval::~JpegDefineRestartInterval ()
{
}

void JpegDefineRestartInterval::read (BinaryIO *io)
{
	JpegSegment::read (io) ;
	restartinterval = io->read16_be() ;
}

void JpegDefineRestartInterval::write (BinaryIO *io)
{
	JpegSegment::write (io) ;
	io->write16_be (restartinterval) ;
}
