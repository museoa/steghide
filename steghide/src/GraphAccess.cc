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

#include "common.h"
#include "Graph.h"
#include "GraphAccess.h"

GraphAccess::GraphAccess (Graph* g)
{
	if (!is_set) {
		TheGraph = g ;
		SamplesPerVertex = g->getSamplesPerVertex() ;
		is_set = true ;
	}
	else {
		myassert (TheGraph == g) ;
	}
}

Graph* GraphAccess::TheGraph = NULL ;
unsigned short GraphAccess::SamplesPerVertex = 0 ;
bool GraphAccess::is_set = false ;
