// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// header for computing CRC16 over a fixed block of memory
#ifndef CRC16_H
#define CRC16_H

#include "hypno/defines.h"

namespace HypnoGadget {

uint16_t CRC16(const uint8_t * data, uint16_t bytes);

}; // namespace


#endif
// end - CRC16.h
