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
#include "gtest/gtest.h"

#include "hypno/CommsException.h"
#include "hypno/CommsClient.h"
#include "hypno/CommsServer.h"
#include "hypno/Socket.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const std::string UNIX_SKTNAME{"/tmp/skt-test"} ;
const std::string TCP_SKTNAME{"localhost:3333"} ;
const unsigned MAX_CLIENTS{3} ;

//-------------------------------------------------------------------------------------------------------------------
class SocketClient : public CommsClient
{
public:
	SocketClient() : CommsClient(std::shared_ptr<IComms>(new Socket)) {}
	virtual ~SocketClient() {}
};

//-------------------------------------------------------------------------------------------------------------------
class SocketServer : public CommsServer
{
public:
	SocketServer() :
		CommsServer(std::shared_ptr<IComms>(new Socket), MAX_CLIENTS),
		mStop(false),
		mHandlerRunning(false)
	{}
	virtual ~SocketServer() {}

	// Implement echo
	virtual bool handler(std::shared_ptr<IComms> comms)
	{
		mHandlerRunning = true ;

		std::string rx ;
		while (comms->isOpen())
		{
			try {
				if (!comms->receive(rx))
					break ;
			} catch (CommsException& e) {
				std::cerr << "Got exception " << e.what() << std::endl ;
				break ;
			}

			// echo back
			std::cerr << "ECHO: '" << rx << "'" << std::endl ;
			comms->send(rx) ;
		}

		std::cerr << "HANDLER STOPPED" << std::endl ;
		mHandlerRunning = false ;
		return false ;
	}


	bool mStop ;
	bool mHandlerRunning ;
};


//===================================================================================================================
class SocketClientServerTest : public ::testing::Test
{
public:

    /*! @brief Setup the test
     */
    virtual void SetUp()
    {
    	const ::testing::TestInfo* const test_info =
    	  ::testing::UnitTest::GetInstance()->current_test_info();

    	std::cout << std::endl << "*** " << test_info->test_case_name() << " Tests ***" <<  std::endl;
    	std::cout << test_info->name() << " - START" <<  std::endl;

    }

    /*! @brief Tear down the test
     */
    virtual void TearDown()
    {

    }

};




//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, UnixConnect)
{
	// server
	SocketServer server ;
	EXPECT_TRUE(server.start(UNIX_SKTNAME)) ;

	// client
	SocketClient client ;
	EXPECT_TRUE(client.start(UNIX_SKTNAME)) ;

}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, UnixData)
{
	// server
	std::cerr << "- Server -" << std::endl ;
	SocketServer server ;
	EXPECT_TRUE(server.start(UNIX_SKTNAME)) ;

	// client
	std::cerr << "- Client -" << std::endl ;
	SocketClient client ;
	EXPECT_TRUE(client.start(UNIX_SKTNAME)) ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=client.sendData(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=client.receiveData(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	std::cerr << "** TEST END **" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, TcpConnect)
{
	// server
	SocketServer server ;
	EXPECT_TRUE(server.start(TCP_SKTNAME)) ;

	// client
	SocketClient client ;
	EXPECT_TRUE(client.start(TCP_SKTNAME)) ;

	std::cerr << "** TEST END **" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, TcpData)
{
	// server
	SocketServer server ;
	EXPECT_TRUE(server.start(TCP_SKTNAME)) ;

	// client
	SocketClient client ;
	EXPECT_TRUE(client.start(TCP_SKTNAME)) ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=client.sendData(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=client.receiveData(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	std::cerr << "** TEST END **" << std::endl ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, TcpClients)
{
	// server
	SocketServer server ;
	EXPECT_TRUE(server.start(TCP_SKTNAME)) ;

	std::string message("Great, it works!") ;

	// Reduce scope of clients - when destroyed server should still be running
	{

	// clients - should be able to connect up to MAX_CLIENTS
	std::vector< std::shared_ptr<SocketClient> > clients ;
	for (unsigned id=0; id < MAX_CLIENTS; ++id)
	{
		std::cerr << "Create client #" << id << std::endl ;
		std::shared_ptr<SocketClient> client( std::make_shared<SocketClient>() ) ;
		clients.push_back(client) ;
		std::cerr << "Start client #" << id << std::endl ;
		EXPECT_TRUE(client->start(TCP_SKTNAME)) ;

		// should be able to send data
		EXPECT_TRUE(client->sendData(message)) ;

		// should be able to receive data
		std::string rx ;
		EXPECT_TRUE(client->receiveData(rx)) ;

		EXPECT_EQ(message, rx) ;
	}

//	// Should fail trying to connect 1 more
//	{
//		std::string message1("Does this work?") ;
//
//		std::cerr << "###################" << std::endl ;
//		std::cerr << "Create client #" << MAX_CLIENTS << std::endl ;
//		std::shared_ptr<SocketClient> client( std::make_shared<SocketClient>() ) ;
//		clients.push_back(client) ;
//		std::cerr << "Start client #" << MAX_CLIENTS << std::endl ;
//		EXPECT_TRUE(client->start(TCP_SKTNAME)) ;
//
//		// should NOT be able to send data
//		EXPECT_TRUE(client->sendData(message1)) ;
//
//		// should NOT be able to receive data
//		std::string rx ;
//		EXPECT_FALSE(client->receiveData(rx, 500)) ;
//
//		EXPECT_TRUE(rx.empty()) ;
//
//		std::cerr << "###################" << std::endl ;
//	}

	}


	// Server should still be up...

	std::cerr << "** TEST END **" << std::endl ;
}

#if 0
//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientServerTest, TcpServerDisconnect)
{
	std::string message("Great, it works!") ;

	std::shared_ptr<SocketClient> client( std::make_shared<SocketClient>() ) ;

	// Create server in disposable context
	{
		std::cerr << "## SERVER CREATE ##" << std::endl ;
		// server
		SocketServer server ;
		EXPECT_TRUE(server.start(TCP_SKTNAME)) ;

		// Connect client
		EXPECT_TRUE(client->start(TCP_SKTNAME)) ;

		// should be able to send data
		EXPECT_TRUE(client->sendData(message)) ;

		// should be able to receive data
		std::string rx ;
		EXPECT_TRUE(client->receiveData(rx)) ;

		EXPECT_EQ(message, rx) ;

		std::cerr << "## SERVER DESTROY ##" << std::endl ;
	}


	// Server killed - now try client...

	// should be able to send data
	EXPECT_TRUE(client->sendData(message)) ;

	// should be able to receive data
	std::string rx ;
	EXPECT_TRUE(client->receiveData(rx)) ;

	EXPECT_EQ(message, rx) ;


std::cerr << "### TEST END ###" << std::endl ;
}
#endif

//===================================================================================================================

