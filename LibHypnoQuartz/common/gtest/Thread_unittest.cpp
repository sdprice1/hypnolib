/*!
 * Copyright (c) 2016 Steve Price.
 * 
 * This file is part of Hypnocube
 * 
 * Hypnocube is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Hypnocube is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Hypnocube.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      Thread_unittest.cpp
 * \brief     
 *
 * \date      7 Jul 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "Gtesting.h"

#include "Thread.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//===================================================================================================================
class ThreadTest : public Gtesting {};

class ThreadUut : public Thread {
public:
	ThreadUut(const std::string& name) : Thread(name), mCount(0) {}
	virtual ~ThreadUut() {}

	virtual bool run()
	{
		++mCount ;
		TimeUtils::msSleep(200) ;
		return true ;
	}

	unsigned mCount ;
};

//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(ThreadTest, test)
{
	ThreadUut a("A") ;
	ThreadUut b("A") ;

	EXPECT_EQ(0u, a.mCount) ;
	EXPECT_EQ(0u, b.mCount) ;

	EXPECT_FALSE(a.isRunning()) ;
	EXPECT_FALSE(b.isRunning()) ;
	EXPECT_TRUE(a.isStop()) ;
	EXPECT_TRUE(b.isStop()) ;

	a.start() ;
//	TimeUtils::msSleep(100) ;

	EXPECT_TRUE(a.isRunning()) ;
	EXPECT_FALSE(b.isRunning()) ;
	EXPECT_FALSE(a.isStop()) ;
	EXPECT_TRUE(b.isStop()) ;

	TimeUtils::msSleep(800) ;
	a.stop() ;
//	TimeUtils::msSleep(400) ;

	EXPECT_FALSE(a.isRunning()) ;
	EXPECT_FALSE(b.isRunning()) ;
	EXPECT_TRUE(a.isStop()) ;
	EXPECT_TRUE(b.isStop()) ;

	EXPECT_TRUE(a.mCount >= 4) ;
	EXPECT_EQ(0u, b.mCount) ;
}


//===================================================================================================================

