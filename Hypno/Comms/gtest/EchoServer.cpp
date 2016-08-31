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
 * \file      Socket_unittest.cpp
 * \brief     
 *
 * \date      9 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <unistd.h>

#include "hypno/CommsException.h"
#include "hypno/CommsClient.h"
#include "hypno/CommsServer.h"
#include "hypno/Socket.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const std::string TCP_SKTNAME{"localhost:3333"} ;


//-------------------------------------------------------------------------------------------------------------------
class SocketServer : public CommsServer
{
public:
	SocketServer() : CommsServer(std::shared_ptr<IComms>(new Socket)) {}
	virtual ~SocketServer() {}

	// Implement echo
	virtual bool handler(std::shared_ptr<IComms> comms)
	{
		while (isConnected())
		{
			std::string rx ;

			try {
				if (!comms->receive(rx))
					break ;
			} catch (CommsException& e) {
				std::cerr << "Got exception " << e.what() << std::endl ;
				break ;
			}

			// echo back
			comms->send(rx) ;
		}

		return false ;
	}

};


int main( int argc, char* argv[] )
{
	// server
	SocketServer server ;
	if (!server.start(TCP_SKTNAME))
	{
		std::cerr << "Unable to connect to " << TCP_SKTNAME << std::endl ;
		return 1 ;
	}

	while(true) sleep(10) ;
	return 0;
}


//===================================================================================================================

