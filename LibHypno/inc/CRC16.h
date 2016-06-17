// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// header for computing CRC16 over a fixed block of memory
#ifndef CRC16_H
#define CRC16_H

#include "defines.h"

namespace HypnoGadget {

uint16 CRC16(const uint8 * data, uint16 bytes);

}; // namespace


#endif
// end - CRC16.h
