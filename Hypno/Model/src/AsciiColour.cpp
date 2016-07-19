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
 * \file      AsciiColour.cpp
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "AsciiColour.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// LOCAL
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
namespace {

	const std::string ASCII_COLOURS = " BGCRMYW" ;

	uint8_t component2bit(uint8_t comp)
	{
		if (comp < 8)
			return 0 ;
		return 1 ;
	}
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
AsciiColour::AsciiColour() :
	Colour()
{
}

//-------------------------------------------------------------------------------------------------------------
AsciiColour::AsciiColour(const Colour& colour) :
	Colour(colour)
{
}

//-------------------------------------------------------------------------------------------------------------
AsciiColour::~AsciiColour()
{
}

//-------------------------------------------------------------------------------------------------------------
char AsciiColour::getChar() const
{
	// First convert the RGB values into a 3 bit code
	uint8_t code(
		component2bit(red()) << 2 |
		component2bit(green()) << 1 |
		component2bit(blue())
	) ;

	if (code > ASCII_COLOURS.size())
		return ' ' ;

	return ASCII_COLOURS.at(code) ;
}
