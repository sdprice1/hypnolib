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
#include "gtest/gtest.h"

#include "test.h"
#include "TtyComms.h"
#include "CommsClient.h"

using namespace HypnoQuartz ;

#ifdef TEST_TTY
//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------------
class TtyClient : public CommsClient
{
public:
	TtyClient() :
		CommsClient(std::shared_ptr<IComms>(new TtyComms))
	{}

	virtual ~TtyClient()
	{
	}
};


//===================================================================================================================
class TtyClientTest : public ::testing::Test
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
TEST_F(TtyClientTest, TtyConnect)
{
	// server
	TtyComms tty0 ;
	EXPECT_TRUE(tty0.serverListen(DEV0)) ;

	// client
	TtyClient client ;
	EXPECT_TRUE(client.start(DEV1)) ;

	tty0.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(TtyClientTest, TtyData)
{
	// server
	TtyComms tty0 ;
	EXPECT_TRUE(tty0.serverListen(DEV0)) ;

	// client
	TtyClient client ;
	EXPECT_TRUE(client.start(DEV1)) ;


	std::cerr << "Accept..." << std::endl ;
	std::shared_ptr<IComms> cfd(tty0.accept()) ;
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
	tty0.close() ;
}


//===================================================================================================================
#endif
