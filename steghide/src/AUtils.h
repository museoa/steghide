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

#ifndef SH_AUTILS_H
#define SH_AUTILS_H

/**
 * \class AUtils
 * \brief provides some generic functions for non-standard arithmetic operations
 **/
template<class T>
class AUtils {
	public:
	/**
	 * return the maximum of a and b (needs >)
	 **/
	static T max (T a, T b) ;

	/**
	 * returns a divided through b rounded up to nearest "integer" (needs =, --, +, /)
	 **/
	static T div_roundup (T a, T b) ;

	/**
	 * substraction with the modification to return 0 (T()) for negative difference (needs >, -, T())
	 **/
	static T bminus (T a, T b) ;

	/**
	 * calculate the sum s[0]+...s[n-1] modulo m (needs =, +, %)
	 **/
	static T modsum (T* s, unsigned short n, T m) ;
} ;

template<class T>
T AUtils<T>::max (T a, T b)
{
	if (a > b) {
		return a ;
	}
	else {
		return b ;
	}
}

template<class T>
T AUtils<T>::div_roundup (T a, T b)
{
	T c = b-- ;
	return ((a + b) / c) ;
}

template<class T>
T AUtils<T>::bminus (T a, T b)
{
	if (a > b) {
		return (a - b) ;
	}
	else {
		return T() ;
	}
}

template<class T>
T AUtils<T>::modsum (T* s, unsigned short n, T m)
{
	T retval() ;
	for (unsigned short i = 0 ; i < n ; i++) {
		retval = (retval + s[i]) % m ;
	}
	return retval ;
}

#endif // ndef SH_AUTILS_H
