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
 * \file      Coord_unittest.cpp
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

#include "hypno/Coord.h"

#include "gtest/gtest.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TEST(Coord, basic)
{
	Coord c0(0, 0, 0) ;
	Coord c1(1, 2, 3) ;

	EXPECT_EQ(0, c0.x()) ;
	EXPECT_EQ(0, c0.y()) ;
	EXPECT_EQ(0, c0.z()) ;

	EXPECT_EQ(1, c1.x()) ;
	EXPECT_EQ(2, c1.y()) ;
	EXPECT_EQ(3, c1.z()) ;

}

//-------------------------------------------------------------------------------------------------------------
TEST(Coord, maths)
{
	Coord c0(0, 0, 0) ;
	Coord c1(1, 2, 3) ;
	Coord c2(3, 4, 5) ;
	Coord c3(-1, -2, -3) ;

	Coord c0mc1(c0 - c1) ;

	EXPECT_EQ(-1, c0mc1.x()) ;
	EXPECT_EQ(-2, c0mc1.y()) ;
	EXPECT_EQ(-3, c0mc1.z()) ;

	c0mc1 += c2 ;

	EXPECT_EQ(2, c0mc1.x()) ;
	EXPECT_EQ(2, c0mc1.y()) ;
	EXPECT_EQ(2, c0mc1.z()) ;

	c0mc1 -= c3 ;

	EXPECT_EQ(3, c0mc1.x()) ;
	EXPECT_EQ(4, c0mc1.y()) ;
	EXPECT_EQ(5, c0mc1.z()) ;

	c0mc1 = c2 + c1 ;

	EXPECT_EQ(4, c0mc1.x()) ;
	EXPECT_EQ(6, c0mc1.y()) ;
	EXPECT_EQ(8, c0mc1.z()) ;
}
