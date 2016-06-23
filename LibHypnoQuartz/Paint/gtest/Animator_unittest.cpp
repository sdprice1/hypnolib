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
 * \file      Animator_unittest.cpp
 * \brief     
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */

//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <iostream>

#include "gtest/gtest.h"
#include "Animator.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
class TestAnimCallbacks : public virtual IAnimatorCallback {
public:
	TestAnimCallbacks() :
		tick(0),
		tickTimeMs(0)
	{}

	virtual ~TestAnimCallbacks() {}

	virtual bool newFrame() override
	{
		std::chrono::system_clock::time_point now(std::chrono::system_clock::now()) ;
std::cerr << "tick=" << tick << " tickTimeMs=" << tickTimeMs << std::endl ;
		if ( (tick > 0) && (tickTimeMs > 0) )
		{
			unsigned diffMs(  std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count() ) ;
			EXPECT_NEAR (diffMs, tickTimeMs, 2) ;
		}

		++tick ;
		lastTime = now ;
		return true ;
	}

	bool newFrameFail()
	{
		++tick ;
		return false ;
	}

	virtual std::vector<uint8_t> getData() override
	{
		std::vector<uint8_t> data ;
		data.push_back(0) ;
		data.push_back(1) ;
		data.push_back(2) ;
		data.push_back(3) ;
		return data ;
	}

	virtual bool paint(const std::vector<uint8_t>& data) override
	{
		return true ;
	}

	bool paintFalse(const std::vector<uint8_t>& data)
	{
		return false ;
	}


public:
	unsigned tick ;
	unsigned tickTimeMs ;
	std::chrono::system_clock::time_point lastTime ;
};


//-------------------------------------------------------------------------------------------------------------
TEST(Animator, basic)
{
	std::cerr << "Basic test start" << std::endl ;
	Animator anim ;
	anim.setFrameRate(1000) ;

	EXPECT_FALSE(anim.isRunning()) ;
	EXPECT_FALSE(anim.start()) ;
	EXPECT_FALSE(anim.isRunning()) ;
	anim.stop() ;
	EXPECT_FALSE(anim.isRunning()) ;

	std::cerr << "Basic test end" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
TEST(Animator, anim)
{
	std::cerr << "Anim test start" << std::endl ;
	Animator anim ;
	anim.setFrameRate(250) ;

	TestAnimCallbacks callbacks ;
	callbacks.tickTimeMs = 250 ;

	EXPECT_FALSE(anim.isRunning()) ;
	EXPECT_TRUE(anim.start(
		[&callbacks]()->bool { return callbacks.newFrame(); },
		[&callbacks]()->std::vector<uint8_t> { return callbacks.getData(); },
		[&callbacks](const std::vector<uint8_t>& data)->bool { return callbacks.paint(data); }
	)) ;
	EXPECT_TRUE(anim.isRunning()) ;

	sleep(1) ;

	anim.stop() ;
std::cerr << "Should be stopped!" << std::endl ;
	EXPECT_FALSE(anim.isRunning()) ;

	anim.setFrameRate(20) ;
std::cerr << "New rate.." << std::endl ;
	callbacks.tick = 0 ;
	callbacks.tickTimeMs = 20 ;

std::cerr << "Starting..." << std::endl ;
	EXPECT_TRUE(anim.start()) ;
	EXPECT_TRUE(anim.isRunning()) ;
	sleep(1) ;
	anim.stop() ;
	EXPECT_FALSE(anim.isRunning()) ;


	std::cerr << "Anim test end" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
TEST(Animator, animObj)
{
	std::cerr << "Anim test start" << std::endl ;
	Animator anim ;
	anim.setFrameRate(250) ;

	std::shared_ptr<TestAnimCallbacks> callbacks(std::make_shared<TestAnimCallbacks>()) ;
	callbacks->tickTimeMs = 250 ;

	EXPECT_FALSE(anim.isRunning()) ;
	EXPECT_TRUE(anim.start(callbacks)) ;
	EXPECT_TRUE(anim.isRunning()) ;

	sleep(1) ;

	anim.stop() ;
std::cerr << "Should be stopped!" << std::endl ;
	EXPECT_FALSE(anim.isRunning()) ;

	anim.setFrameRate(20) ;
std::cerr << "New rate.." << std::endl ;
	callbacks->tick = 0 ;
	callbacks->tickTimeMs = 20 ;

std::cerr << "Starting..." << std::endl ;
	EXPECT_TRUE(anim.start()) ;
	EXPECT_TRUE(anim.isRunning()) ;
	sleep(1) ;
	anim.stop() ;
	EXPECT_FALSE(anim.isRunning()) ;


	std::cerr << "Anim test end" << std::endl ;
}




//-------------------------------------------------------------------------------------------------------------
TEST(Animator, animErr)
{
	std::cerr << "AnimErr test start" << std::endl ;
	Animator anim ;
	anim.setFrameRate(250) ;

	TestAnimCallbacks callbacks ;
	callbacks.tickTimeMs = 0 ;

	EXPECT_FALSE(anim.isRunning()) ;

	//---------------------------------------------------
	// should start running then abort
	EXPECT_TRUE(anim.start(
		[&callbacks]()->bool { return callbacks.newFrameFail(); },
		[&callbacks]()->std::vector<uint8_t> { return callbacks.getData(); },
		[&callbacks](const std::vector<uint8_t>& data)->bool { return callbacks.paint(data); }
	)) ;
	EXPECT_TRUE(anim.isRunning()) ;
	sleep(1) ;
	EXPECT_FALSE(anim.isRunning()) ;


	//---------------------------------------------------
	// should start running then abort
	EXPECT_TRUE(anim.start(
		[&callbacks]()->bool { return callbacks.newFrame(); },
		[&callbacks]()->std::vector<uint8_t> { return callbacks.getData(); },
		[&callbacks](const std::vector<uint8_t>& data)->bool { return callbacks.paintFalse(data); }
	)) ;
	EXPECT_TRUE(anim.isRunning()) ;
	sleep(1) ;
	EXPECT_FALSE(anim.isRunning()) ;





	std::cerr << "AnimErr test end" << std::endl ;
}


