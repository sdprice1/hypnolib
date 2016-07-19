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

#include "CubeCoordMap.h"

#include "gtest/gtest.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TEST(CubeCoordMap, basic)
{

	Coord c0(0, 0, 0) ;
	Coord c1(1, 2, 3) ;

	EXPECT_EQ(0, c0.x()) ;
	EXPECT_EQ(0, c0.y()) ;
	EXPECT_EQ(0, c0.z()) ;

	EXPECT_EQ(1, c1.x()) ;
	EXPECT_EQ(2, c1.y()) ;
	EXPECT_EQ(3, c1.z()) ;

	std::shared_ptr<CoordMap> to(CubeCoordMap::toRawCoords()) ;
	Coord c0mapped(to->translate(c0)) ;
	Coord c1mapped(to->translate(c1)) ;

	EXPECT_EQ(0, c0mapped.x()) ;
	EXPECT_EQ(3, c0mapped.y()) ;
	EXPECT_EQ(0, c0mapped.z()) ;

	EXPECT_EQ(1, c1mapped.x()) ;
	EXPECT_EQ(0, c1mapped.y()) ;
	EXPECT_EQ(2, c1mapped.z()) ;

	std::shared_ptr<CoordMap> from(CubeCoordMap::fromRawCoords()) ;
	Coord c0unmapped(from->translate(c0mapped)) ;
	Coord c1unmapped(from->translate(c1mapped)) ;

	EXPECT_EQ(0, c0unmapped.x()) ;
	EXPECT_EQ(0, c0unmapped.y()) ;
	EXPECT_EQ(0, c0unmapped.z()) ;

	EXPECT_EQ(1, c1unmapped.x()) ;
	EXPECT_EQ(2, c1unmapped.y()) ;
	EXPECT_EQ(3, c1unmapped.z()) ;
}
