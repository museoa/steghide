/*
 * steghide 0.4.6b - a steganography program
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

#include "arg.h"

template<class T> Arg<T>::Arg (void)
{
	set = false ;
}

template<class T> Arg<T>::Arg (T v, bool setbyuser)
{
	Arg () ;
	setValue (v, setbyuser) ;
}

template<class T> T Arg<T>::getValue (void)
{
	return value ;
}

template<class T> void Arg<T>::setValue (T v, bool setbyuser)
{
	value = v ;
	if (setbyuser) {
		set = true ;
	}

	return ;
}

template<class T> bool Arg<T>::is_set (void)
{
	return set ;
}
