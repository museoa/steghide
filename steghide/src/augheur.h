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

#ifndef SH_AUGHEUR_H
#define SH_AUGHEUR_H

#include "edgeiterator.h"
#include "graph.h"
#include "matching.h"

/**
 * \class AugmentingPathHeuristic
 **/
class AugmentingPathHeuristic : private GraphAccess {
	public:
	AugmentingPathHeuristic (Graph *g, Matching *m) ;

	void run (void) ;

	Matching *getMatching (void) const
		{ return TheMatching ; } ;

	private:
	/**
	 * \param v0 an exposed vertex
	 **/
	vector<Edge*>* searchAugmentingPath (Vertex* v0) ;

	Edge* getNextEdge (Vertex *v) ;

	void markVisited (Vertex *v)
		{ TimeCounters[v->getLabel()] = TimeCounter ; } ;

	/**
	 * returns true iff v has already been visited in this
	 * iteration, i.e. in the current call of searchAugmentingPath
	 **/
	bool isVisited (Vertex *v)
		{ return (TimeCounters[v->getLabel()] == TimeCounter) ; } ;

	Matching *TheMatching ;

	UWORD32 TimeCounter ;
	vector<UWORD32> TimeCounters ;
	vector<bool> VertexOnPath ;
	vector<EdgeIterator> EdgeIterators ;
} ;

#endif // ndef SH_AUGHEUR_H
