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
#include "graph.h"
#include "graphaccess.h"

//DEBUG

GraphAccess::GraphAccess ()
{
	cerr << "uninitialized graph access constructor call" << endl ;
}

GraphAccess::GraphAccess (Graph *g)
{
#if 0
	if (TheGraph == NULL) {
//		cerr << "graph access first time" << endl ;
		TheGraph = g ;
		SamplesPerVertex = TheGraph->getSamplesPerVertex() ;
	}
	else {
		//cerr << "graph access with " << TheGraph->getNumVertices() << " vertices." << endl ;
		assert (TheGraph == g) ;
	}
#endif
	TheGraph = g ;
	SamplesPerVertex = TheGraph->getSamplesPerVertex() ;
}

#if 0
Graph* GraphAccess::TheGraph = NULL ;
unsigned short GraphAccess::SamplesPerVertex = 0 ;
#endif
