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

#ifndef SH_GRAPHACCESS_H
#define SH_GRAPHACCESS_H

// DEBUG
#include <iostream>

class Graph ;

class GraphAccess {
	public:
	GraphAccess (void) ;
	GraphAccess (Graph* g) ;

	protected:
	// FIXME - these two should be static
	Graph* TheGraph ;
	unsigned short SamplesPerVertex ;
} ;

#endif // ndef SH_GRAPHACCESS_H
