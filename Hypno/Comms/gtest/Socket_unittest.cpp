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
#include "Socket.h"

#include "gtest/gtest.h"


using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const std::string UNIX_SKTNAME{"/tmp/skt-test"} ;
const std::string TCP_SKTNAME{"localhost:3333"} ;


//===================================================================================================================
class SocketTest : public ::testing::Test
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

    //-------------------------------------------------------------------------------------------------------------
    bool receive(int fd, std::string& s)
    {
        int maxRecBuf = 512;
        char buf[maxRecBuf + 1];

        s = "";

        memset(buf, 0, maxRecBuf + 1);

        int status = ::recv(fd, buf, maxRecBuf, 0);

        if (status == -1)
        {
        	if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
        	{
        		// non-blocking
        		return 1 ;
        	}
        	else
        	{
    			return 0;
        	}
        }

        if (status == 0)
        {
            return 0;
        }

		// text copy
		s = buf ;

    	return status;
    }

};




//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, UnixConnect)
{
	Socket skts ;
	Socket sktc ;

	// client
	EXPECT_FALSE(sktc.clientConnect(UNIX_SKTNAME)) ;

	// server
	EXPECT_TRUE(skts.serverListen(UNIX_SKTNAME)) ;

	// client
	EXPECT_TRUE(sktc.clientConnect(UNIX_SKTNAME)) ;

	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, UnixData)
{
	Socket skts ;
	Socket sktc ;

	// server
	EXPECT_TRUE(skts.serverListen(UNIX_SKTNAME)) ;

	// client
	EXPECT_TRUE(sktc.clientConnect(UNIX_SKTNAME)) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpConnect)
{
	Socket skts ;
	Socket sktc ;

	// client
	EXPECT_FALSE(sktc.clientConnect(TCP_SKTNAME)) ;

	// server
//	EXPECT_TRUE(skts.serverListen("*:3333")) ;
	EXPECT_TRUE(skts.serverListen(TCP_SKTNAME)) ;

	// client
	EXPECT_TRUE(sktc.clientConnect(TCP_SKTNAME)) ;

	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpData)
{
	Socket skts ;
	Socket sktc ;

	// server
	EXPECT_TRUE(skts.serverListen(TCP_SKTNAME)) ;

	// client
	EXPECT_TRUE(sktc.clientConnect(TCP_SKTNAME)) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpAnyHost)
{
	Socket skts ;
	Socket sktc ;

	// server
	EXPECT_TRUE(skts.serverListen("*:5555")) ;

	// client
	EXPECT_TRUE(sktc.clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpPortRange)
{
	Socket skts ;
	Socket sktc ;

	// server
	EXPECT_TRUE(skts.serverListen("localhost:5555-6000")) ;
	unsigned port(skts.getPort()) ;
	EXPECT_EQ(5555u, port) ;

	// client
	EXPECT_TRUE(sktc.clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	sktc.close() ;
	skts.close() ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpAnyPort)
{
	Socket skts ;
	Socket sktc ;

	// server
	EXPECT_TRUE(skts.serverListen("localhost:*")) ;
	unsigned port(skts.getPort()) ;
	EXPECT_TRUE(port >= 1024) ;
	std::cerr << "Connected to port " << port << std::endl ;

	// client
	EXPECT_TRUE(sktc.clientConnect(port, "localhost")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	sktc.close() ;
	skts.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(SocketTest, TcpIcomms)
{
	std::shared_ptr<IComms> skts(std::make_shared<Socket>()) ;
	std::shared_ptr<IComms> sktc(std::make_shared<Socket>()) ;

	// server
	EXPECT_TRUE(skts->serverListen("*:5555")) ;

	// client
	EXPECT_TRUE(sktc->clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(skts->accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=sktc->send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

}


//===================================================================================================================

