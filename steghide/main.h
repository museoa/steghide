/*
 * steghide 0.4.2 - a steganography program
 * Copyright (C) 2001 Stefan Hetzl <shetzl@teleweb.at>
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

#ifndef SH_MAIN_H
#define SH_MAIN_H

#define ACTN_EMBED		1
#define ACTN_EXTRACT	2
#define ACTN_VERSION	3
#define ACTN_LICENSE	4
#define ACTN_HELP		5

#define INTERVAL_DEFAULT	10

#define PROGNAME	"steghide"

extern int args_action ;
extern char *args_passphrase ;
extern char *args_fn_cvr ;
extern char *args_fn_stg ;
extern char *args_fn_pln ;
extern int args_quiet ;
extern int args_verbose ;

#endif /* ndef SH_MAIN_H */
