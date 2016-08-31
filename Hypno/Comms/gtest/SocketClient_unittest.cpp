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

#include "hypno/CommsClient.h"
#include "hypno/Socket.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const std::string UNIX_SKTNAME{"/tmp/skt-test"} ;
const std::string TCP_SKTNAME{"localhost:3333"} ;


//-------------------------------------------------------------------------------------------------------------------
class SocketClient : public CommsClient
{
public:
	SocketClient() :
		CommsClient(std::shared_ptr<IComms>(new Socket))
	{}

	virtual ~SocketClient()
	{
	}
};


//===================================================================================================================
class SocketClientTest : public ::testing::Test
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
TEST_F(SocketClientTest, UnixConnect)
{
	// server
	Socket skts ;
	EXPECT_TRUE(skts.serverListen(UNIX_SKTNAME)) ;

	// client
	SocketClient client ;
	EXPECT_TRUE(client.start(UNIX_SKTNAME)) ;

	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketClientTest, UnixData)
{

	// server
	Socket skts ;
	EXPECT_TRUE(skts.serverListen(UNIX_SKTNAME)) ;

	// client
	SocketClient client ;
	EXPECT_TRUE(client.start(UNIX_SKTNAME)) ;

//skts.setNonBlocking(true);
	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=client.sendData(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	skts.close() ;

}


//===================================================================================================================

