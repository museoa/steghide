/*
 * steghide 0.4.1 - a steganography program
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

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "main.h"

/* error messages */
ERRMSG errmsgs[] = {
 { ERR_NOACTION, 			"neither \"-r\" nor \"-w\" has been specified" },
 { ERR_NOMETHOD, 			"the distribution method has not been specified" },
 { ERR_STDINTWICE, 			"standard input cannot be used for plain file and cover file" },
 { ERR_ACTIONTWICE, 		"the action has been specified twice" }, 
 { ERR_METHODTWICE, 		"the space method has been specified twice" },
 { ERR_METHODUNKNOWN, 		"an unknown method has been specified" },
 { ERR_DATANOALGO,			"the data encryption algorithm has not been specified" },
 { ERR_DATANOMODE,			"the mode for the data encryption has not been specified" }, 
 { ERR_DATANOHASH,			"the hash function for the data encryption has not been specified" },
 { ERR_DATANOPASSPHRASE,	"the passphrase for the data encryption has not been specified" }, 
 { ERR_DATATWOALGOS,		"two names of algorithms for the data encryption were found" },
 { ERR_DATATWOMODES,		"two names of modes for the data encryption were found" },
 { ERR_DATATWOHASHES,		"two names of hash functions for the data encryption were found" },
 { ERR_DATATWOPASSPHRASES,	"two passphrases for the data encryption were found" },
 { ERR_DATALIBMCRYPTALGO,	"your version of libmcrypt does not know the algorithm you specified for data encryption" },
 { ERR_DATALIBMCRYPTMODE,	"your version of libmcrypt does not know the encryption mode you specified for data encryption" },
 { ERR_STHDRNOALGO,			"the stego header encryption algorithm has not been specified" },
 { ERR_STHDRNOMODE,			"the mode for the stego header encryption has not been specified" }, 
 { ERR_STHDRNOHASH,			"the hash function for the stego header encryption has not been specified" },
 { ERR_STHDRNOPASSPHRASE,	"the passphrase for the stego header encryption has not been specified" }, 
 { ERR_STHDRTWOALGOS,		"two names of algorithms for the stego header encryption were found" },
 { ERR_STHDRTWOMODES,		"two names of modes for the stego header encryption were found" },
 { ERR_STHDRTWOHASHES,		"two names of hash functions for the stego header encryption were found" },
 { ERR_STHDRTWOPASSPHRASES,	"two passphrases for the stego header encryption were found" },
 { ERR_STHDRLIBMCRYPTALGO,	"your version of libmcrypt does not know the algorithm you specified for stego header encryption" },
 { ERR_STHDRLIBMCRYPTMODE,	"your version of libmcrypt does not know the encryption mode you specified for stego header encryption" },
 { ERR_ARGUNKNOWN,			"an unknown argument has been encountered" },
 { ERR_PASSPHRASETOOLONG,	"the passphrase must not be longer than 512 characters" },
 { ERR_NOPASSPHRASE,        "no passphrase has been supplied" },
 { ERR_PLNFILENAMELEN,		"the plain file name is longer than 255 characters" },
 { ERR_NUMTOOBIG,			"a number you entered is too big" },
 { ERR_NOPLNFILENAME,		"please specify a name for the plain file (because there is none in the stego file)" },
 { ERR_NOMASK,				"no mask has been specified" },
 { ERR_NOPLNFILENAMECMDL,	"please specify the plain file name after the -pf argument" },
 { ERR_NOCVRFILENAME,		"please specify the cover file name after the -cf argument" },
 { ERR_NOSTGFILENAME,		"please specify the stego file name after the -sf argumnet" },
 { ERR_MASKZERO,			"the mask can not be zero" },

 { ERR_CVROPEN,				"can not open cover file" },
 { ERR_STGOPEN,				"can not open stego file" },
 { ERR_PLNOPEN,				"can not open plain file" },
 { ERR_CVRREAD,				"can not read from cover file" },
 { ERR_STGREAD,				"can not read from stego file" },
 { ERR_PLNREAD,				"can not read from plain file" },
 { ERR_STGWRITE,			"can not write to stego file" },
 { ERR_PLNWRITE,			"can not write to plain file" },
 { ERR_CVRCLOSE,			"can not close cover file" },
 { ERR_STGCLOSE,			"can not close stego file" },
 { ERR_PLNCLOSE,			"can not close plain file" },

 { ERR_STHDRVER,			"the version numver of the stego header is not known" },
 { ERR_CVRUNKNOWNFFORMAT,	"the cover file has an unknown file format" },
 { ERR_STGUNKNOWNFFORMAT,	"the stego file has an unknown file format" },
 { ERR_CVRUNSUPBMPFILE,		"the cover file is an unsupported bmp file" },
 { ERR_STGUNSUPBMPFILE,		"the stego file is an unsupported bmp file" },
 { ERR_CVRBMPWITHCOMPR,		"the cover file is a bmp file that uses compression" },
 { ERR_STGBMPWITHCOMPR,		"the stego file is a bmp file that uses compression" },
 { ERR_CVRUNSUPWAVFILE,		"the cover file is an unsupported wav file" },
 { ERR_STGUNSUPWAVFILE,		"the stego file is an unsupported wav file" },
 { ERR_CVRBPSNOTMULTOF8,	"the cover file is a wav file with a bits per sample rate that is not a multiple of eight" },
 { ERR_STGBPSNOTMULTOF8,	"the stego file is a wav file with a bits per sample rate that is not a multiple of eight" },
 { ERR_STHDRDMTDUNKNOWN,	"the distribution method saved in the stego header is not known (file corruption?)" },
 { ERR_COMPRNOTIMPL,		"plain data is compressed, but compression is not implemented in this version (file corruption?)" },
 { ERR_CKSUMNOTIMPL,		"the stego file contains a checksum of the plain data, but this feature is not implemented in this version (file corruption?)" },

 { ERR_MEMALLOC,			"can not allocate memory" },
 { ERR_CVRTOOSHORT,			"the cover file is too short to embed plain data" },
 { ERR_STGTOOSHORT,			"the stego file is too short to contain plain data of specified length (file corruption?)" },

 { ERR_MHASH,				"an error occured while calling a function from libmhash" },
 { ERR_LIBMCRYPT,			"an error occured while calling a function from libmcrypt" }
} ;

/* prints error message on stderr and exits */
void perr (int errn)
{
	int i = 0;

	while (errmsgs[i].n != errn) {
		i++ ;
	}
	fprintf (stderr, "%s: %s\n", PROGNAME, errmsgs[i].msg) ;
	exit (errn) ;
}
