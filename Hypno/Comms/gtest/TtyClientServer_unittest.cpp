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
 * \file      Tty_unittest.cpp
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
#include "test.h"
#include "hypno/Gtesting.h"
#include "hypno/CommsClient.h"
#include "hypno/CommsServer.h"
#include "hypno/Tty.h"

using namespace HypnoQuartz ;


//=============================================================================================================
// PUBLIC
//=============================================================================================================


//-------------------------------------------------------------------------------------------------------------------
class TtyClient : public CommsClient
{
public:
	TtyClient() : CommsClient(std::shared_ptr<IComms>(new Tty)) {}
	virtual ~TtyClient() {}
};

//-------------------------------------------------------------------------------------------------------------------
class TtyServer : public CommsServer
{
public:
	TtyServer() : CommsServer(std::shared_ptr<IComms>(new Tty)) {}
	virtual ~TtyServer() {}

	// Implement echo
	virtual bool handler(std::shared_ptr<IComms> comms)
	{
std::cerr << "<TEST> handler - START" << std::endl;

		while (comms->isOpen())
		{
			TimeUtils::msSleep(200) ;
			if (!comms->isOpen())
				break ;

			std::string rx ;
			std::cerr << "<TEST> handler - Receive..." << std::endl;
			if (!comms->receive(rx))
				break ;

			// echo back
			if (!rx.empty())
			{
				std::cerr << "<TEST> handler - Echo" << std::endl;
				comms->send(rx) ;
			}
		}

std::cerr << "<TEST> handler - END" << std::endl;
		return false ;
	}

};


//===================================================================================================================
class TtyClientServerTest : public Gtesting {};

#ifdef TEST_TTY

//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TtyClientServerTest, Connect)
{
	// server
	TtyServer server ;
	EXPECT_TRUE(server.start(DEV0)) ;

	// client
	TtyClient client ;
	std::cerr << "== CLIENT CONNECT ===" << std::endl ;
	EXPECT_TRUE(client.start(DEV1)) ;

	std::cerr << "== TEST END ===" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TtyClientServerTest, Data)
{
	// server
	TtyServer server ;
	EXPECT_TRUE(server.start(DEV0)) ;

	// client
	TtyClient client ;
	EXPECT_TRUE(client.start(DEV1)) ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=client.sendData(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
//sleep(1);
	EXPECT_TRUE(ok=client.receiveData(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	std::cerr << "== TEST END ===" << std::endl ;
}


//===================================================================================================================
#endif
