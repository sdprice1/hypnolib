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
 * \file      TTyComms_unittest.cpp
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
#include "hypno/Tty.h"

using namespace HypnoQuartz ;

#ifdef TEST_TTY
//=============================================================================================================
// PUBLIC
//=============================================================================================================

//===================================================================================================================
class TtyTest : public Gtesting {};


//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TtyTest, Connect)
{
	Tty tty0 ;
	Tty tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen(DEV0)) ;

	// client
	EXPECT_TRUE(tty1.clientConnect(DEV1)) ;

	tty0.close() ;
	tty1.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TtyTest, Transfer)
{
	Tty tty0 ;
	Tty tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen(DEV0)) ;

	// client
	EXPECT_TRUE(tty1.clientConnect(DEV1)) ;


	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;

	tty0.close() ;
	tty1.close() ;
}

#if 0
//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, UnixData)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen(UNIX_SKTNAME)) ;

	// client
	EXPECT_TRUE(tty1.clientConnect(UNIX_SKTNAME)) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	tty1.close() ;
	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpConnect)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// client
	EXPECT_FALSE(tty1.clientConnect(TCP_SKTNAME)) ;

	// server
//	EXPECT_TRUE(tty0.serverListen("*:3333")) ;
	EXPECT_TRUE(tty0.serverListen(TCP_SKTNAME)) ;

	// client
	EXPECT_TRUE(tty1.clientConnect(TCP_SKTNAME)) ;

	tty1.close() ;
	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpData)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen(TCP_SKTNAME)) ;

	// client
	EXPECT_TRUE(tty1.clientConnect(TCP_SKTNAME)) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	tty1.close() ;
	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpAnyHost)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen("*:5555")) ;

	// client
	EXPECT_TRUE(tty1.clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	tty1.close() ;
	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpPortRange)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen("localhost:5555-6000")) ;
	unsigned port(tty0.getPort()) ;
	EXPECT_EQ(5555u, port) ;

	// client
	EXPECT_TRUE(tty1.clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	tty1.close() ;
	tty0.close() ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpAnyPort)
{
	TTyComms tty0 ;
	TTyComms tty1 ;

	// server
	EXPECT_TRUE(tty0.serverListen("localhost:*")) ;
	unsigned port(tty0.getPort()) ;
	EXPECT_TRUE(port >= 1024) ;
	std::cerr << "Connected to port " << port << std::endl ;

	// client
	EXPECT_TRUE(tty1.clientConnect(port, "localhost")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1.send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

	cfd->close() ;
	tty1.close() ;
	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TTyCommsTest, TcpIcomms)
{
	std::shared_ptr<IComms> tty0(std::make_shared<TTyComms>()) ;
	std::shared_ptr<IComms> tty1(std::make_shared<TTyComms>()) ;

	// server
	EXPECT_TRUE(tty0->serverListen("*:5555")) ;

	// client
	EXPECT_TRUE(tty1->clientConnect("localhost:5555")) ;

	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0->accept()) ;
	EXPECT_TRUE(cfd.get() != nullptr);
	std::cerr << "Accepted" << std::endl ;

	std::string message("Great, it works!") ;
	std::string rx ;

	// Client -> Server
	std::cerr << "Sending..." << std::endl ;
	bool ok ;
	EXPECT_TRUE(ok=tty1->send(message));
	if (!ok) ::perror("send") ;
	std::cerr << "Sent" << std::endl ;

	std::cerr << "Receiving..." << std::endl ;
	EXPECT_TRUE(ok=cfd->receive(rx)) ;
	if (!ok) ::perror("receive") ;
	std::cerr << "Received" << std::endl ;
	EXPECT_EQ(message, rx) ;

}
#endif

//===================================================================================================================

#endif
