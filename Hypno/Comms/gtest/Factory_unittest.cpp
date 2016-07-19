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
 * \file      Factory_unittest.cpp
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
#include "IComms.h"
#include "Gtesting.h"
#include "test.h"
#include "CommsClient.h"
#include "CommsServer.h"
#include "IComms.h"
#include "Socket.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const std::string UNIX_SKTNAME{"/tmp/skt-test"} ;
const std::string TCP_SKTNAME{"localhost:3333"} ;


//-------------------------------------------------------------------------------------------------------------------
class FactoryClient : public CommsClient
{
public:
	FactoryClient(const std::string& deviceName) :
		CommsClient(IComms::factory(deviceName))
	{}
	virtual ~FactoryClient() {}
};

//-------------------------------------------------------------------------------------------------------------------
class FactoryServer : public CommsServer
{
public:
	FactoryServer(const std::string& deviceName) :
		CommsServer(IComms::factory(deviceName))
	{}
	virtual ~FactoryServer() {}

	// Implement echo
	virtual bool handler(std::shared_ptr<IComms> comms)
	{
//		comms->setNonBlocking(false) ;

std::cerr << "<TEST> handler - START" << std::endl;

		while (isConnected())
		{
			TimeUtils::msSleep(200) ;
			if (!isConnected())
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
class FactoryTest : public Gtesting {
public:
	void runTest(const std::string& serverName, const std::string& clientName)
	{
		// server
		FactoryServer server(serverName) ;
		EXPECT_TRUE(server.start(serverName)) ;

		// client
		FactoryClient client(clientName) ;
		client.setBlocking(true) ;
		EXPECT_TRUE(client.start(clientName)) ;

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

	}

};



//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FactoryTest, Unix)
{
	runTest(UNIX_SKTNAME, UNIX_SKTNAME) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FactoryTest, Tcp)
{
	runTest(TCP_SKTNAME, TCP_SKTNAME) ;
}

//===================================================================================================================

#ifdef TEST_TTY

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FactoryTest, TTY)
{
	runTest(DEV0, DEV1) ;
}



//===================================================================================================================
#endif

