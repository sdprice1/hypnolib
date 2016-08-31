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
 * \file      Debug_unittest.cpp
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

#include "hypno/Debuggable.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//===================================================================================================================
class DebugTest : public ::testing::Test
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
class A : public Debuggable {
public:
	A() : Debuggable("A"), count(0) {}
	~A() {}

	void doDebugNormal()
	{
		++count ;
		debugNormal << "Normal for A" << count << " with a bit " << count << std::endl;
	}

	void doDebugVerbose()
	{
		++count ;
		debugVerbose << "Verbose for A" << count << " with a bit " << count << std::endl;
	}

	unsigned count ;
};

#define CLASSGEN(nm) \
	class nm : public Debuggable { \
	public: \
		nm() : Debuggable(#nm) {} \
		~nm() {} \
\
		void doDebugNormal() \
		{ \
			debugNormal << "Normal for "#nm << std::endl ; \
		} \
\
		void doDebugVerbose() \
		{ \
			debugVerbose << "Verbose for "#nm << std::endl ; \
		} \
	};\

CLASSGEN(B)
CLASSGEN(C)
CLASSGEN(D)



//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, level)
{
	IDebug::setDebug(IDebug::NONE) ;

	A a ;
	B b ;

	EXPECT_TRUE(a.isDebug(IDebug::NONE)) ;
	EXPECT_FALSE(a.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(a.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(b.isDebug(IDebug::NONE)) ;
	EXPECT_FALSE(b.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(b.isDebug(IDebug::VERBOSE)) ;

	a.debugLevel(IDebug::NORMAL) ;

	EXPECT_TRUE(a.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(a.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(a.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(b.isDebug(IDebug::NONE)) ;
	EXPECT_FALSE(b.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(b.isDebug(IDebug::VERBOSE)) ;

	IDebug::setDebug(IDebug::VERBOSE) ;

	EXPECT_TRUE(a.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(a.isDebug(IDebug::NORMAL)) ;
	EXPECT_TRUE(a.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(b.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(b.isDebug(IDebug::NORMAL)) ;
	EXPECT_TRUE(b.isDebug(IDebug::VERBOSE)) ;

	IDebug::setDebug("B", IDebug::NONE) ;

	EXPECT_TRUE(a.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(a.isDebug(IDebug::NORMAL)) ;
	EXPECT_TRUE(a.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(b.isDebug(IDebug::NONE)) ;
	EXPECT_FALSE(b.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(b.isDebug(IDebug::VERBOSE)) ;

	// Global
	C c ;

	EXPECT_TRUE(a.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(a.isDebug(IDebug::NORMAL)) ;
	EXPECT_TRUE(a.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(b.isDebug(IDebug::NONE)) ;
	EXPECT_FALSE(b.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(b.isDebug(IDebug::VERBOSE)) ;

	EXPECT_TRUE(c.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(c.isDebug(IDebug::NORMAL)) ;
	EXPECT_TRUE(c.isDebug(IDebug::VERBOSE)) ;


	// Future
	IDebug::setDebug("D", IDebug::NORMAL) ;
	D d ;

	EXPECT_TRUE(d.isDebug(IDebug::NONE)) ;
	EXPECT_TRUE(d.isDebug(IDebug::NORMAL)) ;
	EXPECT_FALSE(d.isDebug(IDebug::VERBOSE)) ;

}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, output)
{
	IDebug::setDebug(IDebug::NONE) ;

	std::stringstream ss ;
	std::streambuf* coutbuf(std::cout.rdbuf(ss.rdbuf())) ;

	A a ;
	B b ;

	std::cerr << "No output" << std::endl ;
	a.doDebugNormal() ;
	b.doDebugNormal() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_TRUE(ss.str().empty()) ;
	ss.str("") ;

	std::cerr << "A out" << std::endl ;
	IDebug::setDebug("A", IDebug::NORMAL) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for ") == std::string::npos) ;
	ss.str("") ;

	std::cerr << "A+B out" << std::endl ;
	IDebug::setDebug("B", IDebug::VERBOSE) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Normal for B") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for B") != std::string::npos) ;
	ss.str("") ;



	std::cout.rdbuf(coutbuf) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, options)
{
	IDebug::setDebug(IDebug::NONE) ;

	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;

	EXPECT_FALSE( IDebug::optionsTimestamp() ) ;
	EXPECT_FALSE( IDebug::optionsName() ) ;
	EXPECT_FALSE( IDebug::optionsColour() ) ;

	A a ;
	B b ;

	a.setOptionsColour(true) ;

	EXPECT_FALSE( IDebug::optionsTimestamp() ) ;
	EXPECT_FALSE( IDebug::optionsName() ) ;
	EXPECT_TRUE( IDebug::optionsColour() ) ;

	b.setOptionsName(true) ;

	EXPECT_FALSE( IDebug::optionsTimestamp() ) ;
	EXPECT_TRUE( IDebug::optionsName() ) ;
	EXPECT_TRUE( IDebug::optionsColour() ) ;

	a.setOptionsTimestamp(true) ;

	EXPECT_TRUE( IDebug::optionsTimestamp() ) ;
	EXPECT_TRUE( IDebug::optionsName() ) ;
	EXPECT_TRUE( IDebug::optionsColour() ) ;

	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, outputName)
{
	IDebug::setDebug(IDebug::NONE) ;
	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(true) ;
	IDebug::optionsColour(false) ;

	std::stringstream ss ;
	std::streambuf* coutbuf(std::cout.rdbuf(ss.rdbuf())) ;

	A a ;
	B b ;

	std::cerr << "A out" << std::endl ;
	IDebug::setDebug("A", IDebug::NORMAL) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for ") == std::string::npos) ;
	ss.str("") ;

	std::cerr << "A+B out" << std::endl ;
	IDebug::setDebug("B", IDebug::VERBOSE) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[A] Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Verbose for B") != std::string::npos) ;
	ss.str("") ;



	std::cout.rdbuf(coutbuf) ;
	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, outputTimestamp)
{
	IDebug::setDebug(IDebug::NONE) ;
	IDebug::optionsTimestamp(true) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;

	std::stringstream ss ;
	std::streambuf* coutbuf(std::cout.rdbuf(ss.rdbuf())) ;

	A a ;
	B b ;

	std::cerr << "A out" << std::endl ;
	IDebug::setDebug("A", IDebug::NORMAL) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for ") == std::string::npos) ;
	ss.str("") ;

	std::cerr << "A+B out" << std::endl ;
	IDebug::setDebug("B", IDebug::VERBOSE) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Normal for B") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[A] Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Verbose for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("] Verbose for B") != std::string::npos) ;
	ss.str("") ;



	std::cout.rdbuf(coutbuf) ;
	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(DebugTest, outputTimestampName)
{
	IDebug::setDebug(IDebug::NONE) ;
	IDebug::optionsTimestamp(true) ;
	IDebug::optionsName(true) ;
	IDebug::optionsColour(false) ;

	std::stringstream ss ;
	std::streambuf* coutbuf(std::cout.rdbuf(ss.rdbuf())) ;

	A a ;
	B b ;

	std::cerr << "A out" << std::endl ;
	IDebug::setDebug("A", IDebug::NORMAL) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" A] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("Verbose for ") == std::string::npos) ;
	ss.str("") ;

	std::cerr << "A+B out" << std::endl ;
	IDebug::setDebug("B", IDebug::VERBOSE) ;
	a.doDebugNormal() ;
	a.doDebugVerbose() ;
	b.doDebugNormal() ;
	b.doDebugVerbose() ;
	std::cerr << ss.str() << std::endl ;
	EXPECT_FALSE(ss.str().empty()) ;
	EXPECT_TRUE(ss.str().find("[A] Normal for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" A] Normal for A") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Normal for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" B] Normal for B") != std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[A] Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" A] Verbose for A") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find("[B] Verbose for B") == std::string::npos) ;
	EXPECT_TRUE(ss.str().find(" B] Verbose for B") != std::string::npos) ;
	ss.str("") ;



	std::cout.rdbuf(coutbuf) ;
	IDebug::optionsTimestamp(false) ;
	IDebug::optionsName(false) ;
	IDebug::optionsColour(false) ;
}

//===================================================================================================================

