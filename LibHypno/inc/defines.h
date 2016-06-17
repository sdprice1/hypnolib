// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// basic types to make gadget code portable
#ifndef DEFINES_H
#define DEFINES_H

// Linux
#ifndef WIN32
#include <unistd.h>
#include <stdint.h>

#include <cstdlib>
#include <cstddef>
#include <cstring>

#define rom
//#define Sleep(secs)	sleep(secs)
#endif // NOT WIN32



namespace HypnoGadget {

#ifdef WIN32
typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;
#else
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;

#endif
}; // namespace

#endif // defines.h
// end - defines.h
