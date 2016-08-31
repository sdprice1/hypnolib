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
 * \file      Colour.cpp
 * \brief     Pixel/LED colour
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

#include "hypno/Colour.h"

using namespace HypnoQuartz ;


//=============================================================================================================
// PRIVATE STATIC DATA
//=============================================================================================================
const uint8_t MAX_COMPONENT{15} ;

//-------------------------------------------------------------------------------------------------------------
std::default_random_engine Colour::mRandGen ;
std::uniform_int_distribution<uint8_t> Colour::mRandDist(0, MAX_COMPONENT) ;


//=============================================================================================================
// PUBLIC STATIC DATA
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
const Colour Colour::BLACK	( 0,  0,  0) ;
const Colour Colour::WHITE	(15, 15, 15) ;
const Colour Colour::RED	(15,  0,  0) ;
const Colour Colour::GREEN 	( 0, 15,  0) ;
const Colour Colour::BLUE	( 0,  0, 15) ;
const Colour Colour::CYAN	( 0, 15, 15) ;
const Colour Colour::MAGENTA(15,  0, 15) ;
const Colour Colour::YELLOW	(15, 15,  0) ;
const Colour Colour::GREY	( 8,  8,  8) ;



//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
const Colour Colour::rand()
{
	return Colour(
		mRandDist(mRandGen),
		mRandDist(mRandGen),
		mRandDist(mRandGen)
	) ;
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Colour::Colour() :
	mRed(0),
	mGreen(0),
	mBlue(0)
{
}

//-------------------------------------------------------------------------------------------------------------
Colour::Colour(uint8_t  red, uint8_t  green, uint8_t  blue) :
	mRed(clamp(red)),
	mGreen(clamp(green)),
	mBlue(clamp(blue))
{
}

//-------------------------------------------------------------------------------------------------------------
Colour::Colour(uint16_t  rgb) :
	mRed(clamp((rgb>>8) & 0x0f)),
	mGreen(clamp((rgb>>4) & 0x0f)),
	mBlue(clamp(rgb & 0x0f))
{
}

//-------------------------------------------------------------------------------------------------------------
Colour::~Colour()
{
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::red() const
{
	return mRed ;
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::red(uint8_t  component)
{
	uint8_t  red1(mRed) ;
	mRed = clamp(component);
	return red1 ;
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::green() const
{
	return mGreen ;
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::green(uint8_t  component)
{
	uint8_t  green1(mGreen) ;
	mGreen = clamp(component);
	return green1 ;
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::blue() const
{
	return mBlue ;
}

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::blue(uint8_t  component)
{
	uint8_t  red1(mBlue) ;
	mBlue = clamp(component);
	return red1 ;
}

//=============================================================================================================
// PRIVATE STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
uint8_t  Colour::clamp(uint8_t  component)
{
	return component > 0xf ? 0xf : component ;
}

