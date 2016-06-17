/*!
 * Copyright (c) 2016 Steve Price.
 *
 * This file is part of Hypnolib
 *
 * Hypnolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hypnolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hypnolib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      Command.cpp
 * \brief     
 *
 * \date      13 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <map>
#include "Command.h"

using namespace HypnoGadget ;

//=============================================================================================================
// CONSTANT
//=============================================================================================================
#define CMDSTR(nm) \
	{ Command::nm,	#nm }

//-------------------------------------------------------------------------------------------------------------
std::map<Command::CommandType, std::string> CMD_STR{
	CMDSTR(Login),
	CMDSTR(Logout),
	CMDSTR(Version),
	CMDSTR(Ack),
	CMDSTR(Ping),
	CMDSTR(Error),
	CMDSTR(SetFrame),
	CMDSTR(FlipFrame),
	CMDSTR(Reset),
	CMDSTR(Options),
	CMDSTR(GetError),
	CMDSTR(Info),
	CMDSTR(MaxVisIndex),
	CMDSTR(SelectVis),
	CMDSTR(MaxTranIndex),
	CMDSTR(SelectTran),
	CMDSTR(GetFrame),
	CMDSTR(SetPixel),
	CMDSTR(GetPixel),
	CMDSTR(CurrentItem),
	CMDSTR(SetRate),
	CMDSTR(DrawLine),
	CMDSTR(DrawBox),
	CMDSTR(FillImage),
	CMDSTR(SetPFrame),
	CMDSTR(ScrollText),
	CMDSTR(LoadAnim ),
	CMDSTR(Unknown),

} ;

//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
std::string Command::commandStr(CommandType type)
{
	auto entry(CMD_STR.find(type)) ;
	if (entry == CMD_STR.end())
		return "UNKNOWN" ;

	return entry->second ;
}



