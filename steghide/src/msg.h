/*
 * steghide 0.4.6 - a steganography program
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

#ifndef SH_MSG_H
#define SH_MSG_H

#include <cstdarg>
#include <string>

class MessageBase {
	public:
	MessageBase (void) ;
	MessageBase (string msg) ;
	MessageBase (const char *msgfmt, ...) ;
	virtual ~MessageBase (void) ;

	string getMessage (void) ;
	void setMessage (string msg) ;
	void setMessage (const char *msgfmt, ...) ;
	virtual void printMessage (void) = 0 ;

	protected:
	static const unsigned int MsgMaxSize = 512 ;

	string compose (const char *msgfmt, ...) ;
	string vcompose (const char *msgfmt, va_list ap) ;

	private:
	string message ;
} ;

class Message : public MessageBase {
	public:
	Message (void) : MessageBase() {} ;
	Message (string msg) : MessageBase (msg) {} ;
	Message (const char *msgfmt, ...) ;

	void printMessage (void) ;
} ;

class VerboseMessage : public MessageBase {
	public:
	VerboseMessage (void) : MessageBase() {} ;
	VerboseMessage (string msg) : MessageBase (msg) {} ;
	VerboseMessage (const char *msgfmt, ...) ;

	void printMessage (void) ;
} ;

class Warning : public MessageBase {
	public:
	Warning (void) : MessageBase() {} ;
	Warning (string msg) : MessageBase (msg) {} ;
	Warning (const char *msgfmt, ...) ;

	void printMessage (void) ;
} ;

class CriticalWarning : public MessageBase {
	public:
	CriticalWarning (void) : MessageBase() {} ;
	CriticalWarning (string msg) : MessageBase (msg) {} ;
	CriticalWarning (const char *msgfmt, ...) ;

	void printMessage (void) ;
} ;

class Question : public MessageBase {
	public:
	Question (void) ;
	Question (string msg) ;
	Question (const char *msgfmt, ...) ;

	void printMessage (void) ;
	bool getAnswer (void) ;

	private:
	string yeschar ;
	string nochar ;
} ;

#endif	/* ndef SH_MSG_H */
