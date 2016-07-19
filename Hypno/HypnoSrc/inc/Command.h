// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// commands to send to/get from the gadget
#ifndef COMMAND_H
#define COMMAND_H

#include <string>

#include "defines.h"

namespace HypnoGadget {

class Command {
public:
	// commands
	typedef enum {
		// version 0.3 of protocol
		Login        =  0,
		Logout       =  1,
		Version      = 12,
		Ack          = 25,
		Ping         = 60,
		Error        = 20,
		// version 0.4
		SetFrame     = 81,
		FlipFrame    = 80,

		// version 0.5
		Reset        = 10,
		Options      = 15,
		GetError     = 21,
		Info         = 11,

		// version 0.6
		MaxVisIndex  = 40,
		SelectVis    = 41,
		MaxTranIndex = 42,
		SelectTran   = 43,
		GetFrame     = 82,

		// version 0.7
		SetPixel     = 84,
		GetPixel     = 85,
		CurrentItem  = 30,

		// version 0.8
		SetRate      = 50,
		DrawLine     = 86,
		DrawBox      = 87,
		FillImage    = 88,

		// version 0.9
		SetPFrame    = 83,
		ScrollText   = 89,
		LoadAnim     = 90,

		Unknown      = 0xFF // used to mark an unknown command
		} CommandType ;

	/**
	 * Convert type enum nto a string
	 */
	static std::string commandStr(CommandType type) ;
};

}

#endif // COMMAND_H
// end - Command.h
