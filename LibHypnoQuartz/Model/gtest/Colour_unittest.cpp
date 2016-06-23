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
 * \file      Colour_unittest.cpp
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

#include "gtest/gtest.h"
#include "Colour.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TEST(Colour, basic)
{
	Colour red1(15, 0, 0) ;
	Colour green1(0, 15, 0) ;
	Colour blue1(0, 0, 15) ;
	Colour c1(1, 2, 3) ;

	Colour red2(255, 0, 0) ;
	Colour green2(0, 255, 0) ;
	Colour blue2(0, 0, 255) ;

	EXPECT_EQ(15, red1.red()) ;
	EXPECT_EQ(0, red1.green()) ;
	EXPECT_EQ(0, red1.blue()) ;

	EXPECT_EQ(15, red2.red()) ;
	EXPECT_EQ(0, red2.green()) ;
	EXPECT_EQ(0, red2.blue()) ;

	EXPECT_EQ(0, green1.red()) ;
	EXPECT_EQ(15, green1.green()) ;
	EXPECT_EQ(0, green1.blue()) ;

	EXPECT_EQ(0, green2.red()) ;
	EXPECT_EQ(15, green2.green()) ;
	EXPECT_EQ(0, green2.blue()) ;

	EXPECT_EQ(0, blue1.red()) ;
	EXPECT_EQ(0, blue1.green()) ;
	EXPECT_EQ(15, blue1.blue()) ;

	EXPECT_EQ(0, blue2.red()) ;
	EXPECT_EQ(0, blue2.green()) ;
	EXPECT_EQ(15, blue2.blue()) ;

	EXPECT_EQ(1, c1.red()) ;
	EXPECT_EQ(2, c1.green()) ;
	EXPECT_EQ(3, c1.blue()) ;

}

//-------------------------------------------------------------------------------------------------------------
TEST(Colour, Constants)
{

	EXPECT_EQ(0, Colour::BLACK.red()) ;
	EXPECT_EQ(0, Colour::BLACK.green()) ;
	EXPECT_EQ(0, Colour::BLACK.blue()) ;

	EXPECT_EQ(15, Colour::WHITE.red()) ;
	EXPECT_EQ(15, Colour::WHITE.green()) ;
	EXPECT_EQ(15, Colour::WHITE.blue()) ;

	EXPECT_EQ(15, Colour::RED.red()) ;
	EXPECT_EQ(0, Colour::RED.green()) ;
	EXPECT_EQ(0, Colour::RED.blue()) ;

	EXPECT_EQ(0, Colour::GREEN.red()) ;
	EXPECT_EQ(15, Colour::GREEN.green()) ;
	EXPECT_EQ(0, Colour::GREEN.blue()) ;

	EXPECT_EQ(0, Colour::BLUE.red()) ;
	EXPECT_EQ(0, Colour::BLUE.green()) ;
	EXPECT_EQ(15, Colour::BLUE.blue()) ;


	EXPECT_EQ(0, Colour::CYAN.red()) ;
	EXPECT_EQ(15, Colour::CYAN.green()) ;
	EXPECT_EQ(15, Colour::CYAN.blue()) ;

	EXPECT_EQ(15, Colour::MAGENTA.red()) ;
	EXPECT_EQ(0, Colour::MAGENTA.green()) ;
	EXPECT_EQ(15, Colour::MAGENTA.blue()) ;

	EXPECT_EQ(15, Colour::YELLOW.red()) ;
	EXPECT_EQ(15, Colour::YELLOW.green()) ;
	EXPECT_EQ(0, Colour::YELLOW.blue()) ;

	EXPECT_EQ(8, Colour::GREY.red()) ;
	EXPECT_EQ(8, Colour::GREY.green()) ;
	EXPECT_EQ(8, Colour::GREY.blue()) ;

	Colour c1(Colour::RED) ;
	EXPECT_EQ(15, c1.red()) ;
	EXPECT_EQ(0, c1.green()) ;
	EXPECT_EQ(0, c1.blue()) ;

}
