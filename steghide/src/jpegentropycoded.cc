#include <assert.h>

#include <libintl.h>
#define _(S) gettext (S)

#include "binaryio.h"
#include "bufmanag.h"
#include "error.h"
#include "jpegbase.h"
#include "jpegentropycoded.h"
#include "jpegframe.h"
#include "jpegframehdr.h"
#include "jpegscan.h"
#include "jpegscanhdr.h"

JpegEntropyCoded::JpegEntropyCoded ()
	: JpegObject(), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;
}

JpegEntropyCoded::JpegEntropyCoded (BinaryIO *io)
	: JpegObject(), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;

	read (io) ;
}

JpegEntropyCoded::JpegEntropyCoded (JpegObject *p, BinaryIO *io)
	: JpegObject (p), CvrStgObject()
{
	readbyte = 0 ;
	readbytecontains = 0 ;
	termmarker = 0x00 ;
	termclean = false ;

	read (io) ;
}

JpegEntropyCoded::~JpegEntropyCoded ()
{
}

JpegMarker JpegEntropyCoded::getTerminatingMarker ()
{
	return termmarker ;
}

void JpegEntropyCoded::read (BinaryIO *io)
{
	assert (readbyte == 0) ;
	assert (readbytecontains == 0) ;
	assert (termmarker == 0x00) ;

	JpegScan *p_scan = (JpegScan *) getParent() ;
	JpegScanHeader *p_scanhdr = (JpegScanHeader *) p_scan->getScanHeader() ;
	JpegFrame *p_frame = (JpegFrame *) p_scan->getParent() ;
	JpegFrameHeader *p_framehdr = (JpegFrameHeader *) p_frame->getFrameHeader() ;

	unsigned long unitstart = 0 ;
	int prediction = 0 ; // FIXME - prediction für alle components ??

	while (termmarker == 0x00) {
		// read one MCU
		try {
			termclean = true ;	// set to false in first call to nextbit from decode

			for (unsigned char comp = 0 ; comp < p_framehdr->getNumComponents() ; comp++)  {
				unsigned char compdataunits = p_framehdr->getHorizSampling (comp) * p_framehdr->getVertSampling (comp) ;
				JpegHuffmanTable *DCTable = p_scan->getDCTable (p_scanhdr->getDCDestSpec (comp)) ;
				JpegHuffmanTable *ACTable = p_scan->getACTable (p_scanhdr->getACDestSpec (comp)) ;

				for (unsigned char du = 0 ; du < compdataunits ; du++) {
					// decode DC
					unsigned char t = decode (io, DCTable) ;
					int diff = receive (io, t) ;
					diff = extend (diff, t) ;

					for (unsigned char i = 0 ; i < 64 ; i++) {
						dctcoeffs.push_back (0) ;
					}
					dctcoeffs[unitstart] = prediction + diff ;
					prediction = dctcoeffs[unitstart] ;

					// decode ACs
					unsigned char k = 1 ;
					unsigned char ssss = 0 ;
					unsigned char rrrr = 0 ;
					bool eob = false ;

					while ((k <= 63) && !eob) {
						splitByte (decode (io, ACTable), &rrrr, &ssss) ;
						if (ssss == 0) {
							if (rrrr == 15) {
								k += 16 ; // goto start of loop
							}
							else {
								eob = true ;
							}
						}
						else {
							k += rrrr ;
							dctcoeffs[unitstart + k] = extend (receive (io, ssss), ssss) ;
							k++ ;	// next coeff
						}
					}

					unitstart += 64 ;
				}
			}
		}
		catch (JpegMarkerFound e) {
			if (!termclean) {
				if (io->is_std()) {
					throw SteghideError (_("corrupt jpeg file on standard input. Entropy coded data interrupted by marker.")) ;
				}
				else {
					throw SteghideError (_("corrupt jpeg file \"%s\". Entropy coded data interrupted by marker."), io->getName().c_str()) ;
				}
			}
			else {
				termmarker = e.getMarkerCode() ;
			}
		}
	}
}

// FIXME - 2er Komplement wird in jpeg datei verwendet - kann man voraussetzen, dass im Rechner auch zweierkomplement verwendet wird ?
// nein => explizit implementieren (ähnlich be/le) - d.h. negative Zahlen erzeugen

void JpegEntropyCoded::write (BinaryIO *io)
{
}

unsigned long JpegEntropyCoded::getCapacity ()
{
	return ((unsigned long) dctcoeffs.size()) ;
}

void JpegEntropyCoded::embedBit (unsigned long pos, int bit)
{
	assert (pos < dctcoeffs.size()) ;
	assert (bit == 0 || bit == 1) ;

	if (bit != extractBit (pos)) {
		if (dctcoeffs[pos] > 0) {
			dctcoeffs[pos]-- ;
		}
		else if (dctcoeffs[pos] < 0)  {
			dctcoeffs[pos]++ ;
		}
		else if (dctcoeffs[pos] == 0) {
			if ((rand() / (RAND_MAX + 1.0)) >= 0.5) {
				dctcoeffs[pos] = 1 ;
			}
			else {
				dctcoeffs[pos] = -1 ;
			}
		}
	}
}

int JpegEntropyCoded::extractBit (unsigned long pos)
{
	assert (pos < dctcoeffs.size()) ;
	int retval = 0 ;

	if (dctcoeffs[pos] >= 0) {
		retval = dctcoeffs[pos] % 2 ;
	}
	else {
		retval = (-dctcoeffs[pos] % 2) ;
	}

	return retval ;
}

//DEBUG
unsigned char JpegEntropyCoded::decode (BinaryIO *io, JpegHuffmanTable *ht)
{
	unsigned int len = 1 ;
	int code = nextbit (io) ;

	while (code > ht->getMaxCode(len)) {
		len++ ;
		code = (code << 1) | nextbit (io) ;
		assert (len <= 16) ;
	}

	unsigned int j = ht->getValPtr (len) ;
	int mc = ht->getMinCode (len) ;
	assert (code >= mc) ;
	j += code - mc ;
	return ht->getHuffVal(j) ;
}

int JpegEntropyCoded::receive (BinaryIO *io, unsigned char nbits)
{
	int retval = 0 ;

	while (nbits > 0) {
		retval = (retval << 1) | nextbit (io) ;
		nbits-- ;
	}

	return retval ;
}

int JpegEntropyCoded::extend (int val, unsigned char t)
{
	if (val < (1 << (t - 1))) {
		val += (-1 << t) + 1 ;
	}

	return val ;
}

int JpegEntropyCoded::nextbit (BinaryIO *io)
{
	int retval = 0 ;

	if (readbytecontains == 0) {
		readbyte = io->read8() ;
		readbytecontains = 8 ;
		if (readbyte == 0xFF) {
			unsigned char markerbyte = io->read8() ;
			if (markerbyte != 0x00) {
				throw JpegMarkerFound (markerbyte) ;
			}
		}
	}

	retval = readbyte >> 7 ;
	readbytecontains-- ;
	readbyte = readbyte << 1 ;

	/* FIXME - padding bits!?
	if (termclean) {
		termclean = false ;
	}
	*/

	return retval ;
}
