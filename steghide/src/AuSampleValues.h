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

#ifndef SH_AUSAMPLEVALUES_H
#define SH_AUSAMPLEVALUES_H

#include "AudioSampleValue.h"

// AuMuLawSampleValue
typedef AudioSampleValue<AuMuLaw,BYTE> MuLawSampleValue ;
const UWORD32 MuLawSampleValue::DefaultRadius = 1 ;
const BYTE MuLawSampleValue::MinValue = 0 ;
const BYTE MuLawSampleValue::MaxValue = BYTE_MAX ;

// AuPCM8SampleValue
typedef AudioSampleValue<AuPCM8,SBYTE> AuPCM8SampleValue ;
const UWORD32 AuPCM8SampleValue::DefaultRadius = 1 ;
const SBYTE AuPCM8SampleValue::MinValue = SBYTE_MIN ;
const SBYTE AuPCM8SampleValue::MaxValue = SBYTE_MAX ;

// AuPCM16SampleValue
typedef AudioSampleValue<AuPCM16,SWORD16> AuPCM16SampleValue ;
const UWORD32 AuPCM16SampleValue::DefaultRadius = 20 ;
const SWORD16 AuPCM16SampleValue::MinValue = SWORD16_MIN ;
const SWORD16 AuPCM16SampleValue::MaxValue = SWORD16_MAX ;

// AuPCM32SampleValue
typedef AudioSampleValue<AuPCM32,SWORD32> AuPCM32SampleValue ;
const UWORD32 AuPCM32SampleValue::DefaultRadius = 20 ; // FIXME - find good value!
const SWORD32 AuPCM32SampleValue::MinValue = SWORD32_MIN ;
const SWORD32 AuPCM32SampleValue::MaxValue = SWORD32_MAX ;

#endif // ndef SH_AUSAMPLEVALUES_H
