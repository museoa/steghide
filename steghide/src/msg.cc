/*
 * steghide 0.4.5 - a steganography program
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

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <iostream>

#include <termios.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "arguments.h"
#include "main.h"
#include "msg.h"
#include "support.h"

//
// class MessageBase
//
MessageBase::MessageBase ()
{
	setMessage (_("no message defined")) ;
}

MessageBase::MessageBase (string msg)
{
	setMessage (msg) ;
}

MessageBase::MessageBase (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (msgfmt, ap) ;
	va_end (ap) ;
}

MessageBase::~MessageBase ()
{
}

string MessageBase::getMessage ()
{
	return message ;
}

void MessageBase::setMessage (string msg)
{
	message = msg ;
}

void MessageBase::setMessage (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

string MessageBase::compose (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	string retval = vcompose (msgfmt, ap) ;
	va_end (ap) ;
	return retval ;
}

string MessageBase::vcompose (const char *msgfmt, va_list ap)
{
	char *str = new char[MsgMaxSize] ;
	vsnprintf (str, MsgMaxSize, msgfmt, ap) ;
	return string (str) ;
}

//
// class Message
//
Message::Message (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Message::printMessage ()
{
	if (args->verbosity.getValue() == NORMAL ||
		args->verbosity.getValue() == VERBOSE) {

		cerr << getMessage() << endl ;
	}
}

//
// class VerboseMessage
//
VerboseMessage::VerboseMessage (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void VerboseMessage::printMessage ()
{
	if (args->verbosity.getValue() == VERBOSE) {
		cerr << getMessage() << endl ;
	}
}

//
// class Warning
//
Warning::Warning (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Warning::printMessage ()
{
	if (args->verbosity.getValue() == NORMAL ||
		args->verbosity.getValue() == VERBOSE) {

		cerr << PROGNAME << _(": warning: ") << getMessage() << endl ;
	}
}

//
// class CriticalWarning
//
CriticalWarning::CriticalWarning (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void CriticalWarning::printMessage ()
{
	cerr << PROGNAME << _(": warning: ") << getMessage() << endl ;
}

//
// class Question
//
Question::Question (void)
	: MessageBase()
{
	yeschar = string (_("y")) ;
	nochar = string (_("n")) ;
}

Question::Question (string msg)
	: MessageBase (msg)
{
	yeschar = string (_("y")) ;
	nochar = string (_("n")) ;
}

Question::Question (const char *msgfmt, ...)
	: MessageBase()
{
	yeschar = string (_("y")) ;
	nochar = string (_("n")) ;

	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Question::printMessage ()
{
	assert (!stdin_isused()) ;

	cerr << getMessage() << " (" << yeschar << "/" << nochar << ") " ;
}

bool Question::getAnswer ()
{
	assert (!stdin_isused()) ;

	struct termios oldattr = termios_singlekey_on () ;
	char input[2] ;
	input[0] = cin.get() ;
	input[1] = '\0' ;
	bool retval = (string (input) == yeschar) ;
	termios_reset (oldattr) ;

	cerr << endl ;
	return retval ;
}
