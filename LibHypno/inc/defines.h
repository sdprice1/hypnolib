// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// basic types to make gadget code portable
#ifndef DEFINES_H
#define DEFINES_H

#include <cstdint>

// Linux
#ifndef WIN32
#include <unistd.h>

#include <cstdlib>
#include <cstddef>
#include <cstring>

#define rom
//#define Sleep(secs)	sleep(secs)
#endif // NOT WIN32

#endif // defines.h
// end - defines.h
