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

#include "Vertex.h"

class Edge ;
class EdgeIterator ;
class Graph ;
class Matching ;

/**
 * \class AugmentingPathHeuristic
 **/
class AugmentingPathHeuristic {
	public:
	/**
	 * construct an AugmentingPathHeuristic object
	 * \param g the graph on which this heuristic should run
	 * \param m the matching to start with
	 * \param mne the maximum number of edges that should be considered for every vertex
	 **/
	AugmentingPathHeuristic (Graph* g, Matching* m, UWORD32 mne = UWORD32_MAX) ;

	/**
	 * reset the state of this AugmentingPathHeuristic, esp. the EdgeIterators
	 * \param mne the maximum number of edges that should be considered for every vertex for now on
	 **/
	void reset (UWORD32 mne = UWORD32_MAX) ;

	void run (void) ;

	Matching* getMatching (void) const
		{ return TheMatching ; } ;

	private:
	/**
	 * \param v0 an exposed vertex
	 **/
	std::vector<Edge*>* searchAugmentingPath (Vertex* v0) ;

	Edge* getNextEdge (Vertex *v) ;

	void pushOnPath (std::vector<Edge*>* path, Edge* e) ;

	void markVisited (Vertex *v)
		{ TimeCounters[v->getLabel()] = TimeCounter ; } ;

	/**
	 * returns true iff v has already been visited in this
	 * iteration, i.e. in the current call of searchAugmentingPath
	 **/
	bool isVisited (Vertex *v) const
		{ return isVisited(v->getLabel()) ; } ;

	bool isVisited (VertexLabel vlbl) const
		{ return (TimeCounters[vlbl] == TimeCounter) ; } ;

	Graph* TheGraph ;
	Matching* TheMatching ;

	UWORD32 TimeCounter ;
	std::vector<UWORD32> TimeCounters ;
	std::vector<bool> VertexOnPath ;
	std::vector<EdgeIterator> EdgeIterators ;
} ;

#endif // ndef SH_AUGHEUR_H
