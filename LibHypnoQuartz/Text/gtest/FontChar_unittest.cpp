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
 * \file      FontChar_unittest.cpp
 * \brief     
 *
 * \date      10 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "gtest/gtest.h"
#include "FontChar.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TEST(FontChar, basic)
{
	FontChar fc1(6, 5, 'X', {
		{"X    X"},
		{" X  X "},
		{"  X   "},
		{" X X  "},
		{"X   X "},
	}) ;

	EXPECT_EQ('X', fc1.getChar()) ;
	EXPECT_EQ(6, fc1.getWidth()) ;
	EXPECT_EQ(5, fc1.getHeight()) ;
}


//-------------------------------------------------------------------------------------------------------------
TEST(FontChar, iterator)
{
	FontChar fc1(6, 5, 'Y', {
		{"XX   X"},
		{" X  X "},
		{"  X   "},
		{"  X   "},
		{"  XX  "},
	}) ;

	FontChar::const_iterator start(fc1.cbegin()) ;
	FontChar::const_iterator end(fc1.cend()) ;

	std::vector<bool> expected{
		false, false,  true,  true, false, false,
		false, false,  true, false, false, false,
		false, false,  true, false, false, false,
		false,  true, false, false,  true, false,
		 true,  true, false, false, false,  true,
	};

	unsigned count(0) ;
	for (auto pixel : fc1)
	{
		EXPECT_EQ(expected[count], pixel) ;
//		std::cout << "Pixel " << count << " = " << pixel << std::endl ;
		++count ;
	}
}

