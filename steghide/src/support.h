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

#ifndef SH_SUPPORT_H
#define SH_SUPPORT_H

#include <stdio.h>

#include <termios.h>

#define CHILD_ARGCMAX	20
#define CHILD_ARGLENMAX 512

/* function prototypes */
void srnd (unsigned long seed) ;
int rnd (unsigned long max) ;
char *stripdir (const char *filename) ;
void swap (unsigned long *x, unsigned long *y) ;
char *get_passphrase (bool doublecheck) ;
struct termios termios_echo_off (void) ;
struct termios termios_singlekey_on (void) ;
void termios_reset (struct termios attr) ;
int fileexists (char *filename) ;
unsigned long readnum (char *s) ;
void *s_malloc (size_t size) ;
void *s_calloc (size_t nmemb, size_t size) ;
void *s_realloc (void *ptr, size_t size) ;
void cp32ul2uc_be (unsigned char *dest, unsigned long src) ;
void cp32uc2ul_be (unsigned long *dest, unsigned char *src) ;
void cp32ul2uc_le (unsigned char *dest, unsigned long src) ;
void cp32uc2ul_le (unsigned long *dest, unsigned char *src) ;
unsigned int cp_bits_to_buf_le (void *buf, unsigned int bufbitpos, unsigned long val, unsigned int val_nbits) ;
unsigned int cp_bits_from_buf_le (void *buf, unsigned int bufbitpos, unsigned long *val, unsigned int val_nbits) ;
unsigned int nbits (unsigned long x) ;
bool stdin_isused (void) ;
void checkforce (const char *filename) ;

#endif // ndef SH_SUPPORT_H
