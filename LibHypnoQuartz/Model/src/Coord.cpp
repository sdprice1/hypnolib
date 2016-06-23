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
 * \file      Coord.cpp
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

#include "Coord.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Coord::Coord() :
	mX(0),
	mY(0),
	mZ(0)
{
}

//-------------------------------------------------------------------------------------------------------------
Coord::Coord(unsigned x, unsigned y) :
	mX(x),
	mY(y),
	mZ(0)
{
}

//-------------------------------------------------------------------------------------------------------------
Coord::Coord(unsigned x, unsigned y, unsigned z) :
	mX(x),
	mY(y),
	mZ(z)
{
}

//-------------------------------------------------------------------------------------------------------------
Coord::~Coord()
{
}

//-------------------------------------------------------------------------------------------------------------
unsigned Coord::x() const
{
	return mX ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Coord::y() const
{
	return mY ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Coord::z() const
{
	return mZ ;
}
