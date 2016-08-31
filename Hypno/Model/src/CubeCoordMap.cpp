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
 * \file      CubeCoordMap.cpp
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

#include "hypno/CubeCoordMap.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC SINGLETON
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
namespace HypnoQuartz {
class FromRaw : public CubeCoordMap {
public:
	FromRaw() : CubeCoordMap() {}
	virtual ~FromRaw() {}
	virtual Coord translate(const Coord& coord) override
	{
		Coord mapped(coord.x(), coord.z(), 3 - coord.y()) ;
		return mapped ;
	}
};
}
std::shared_ptr<CoordMap> CubeCoordMap::fromRawCoords()
{
	std::shared_ptr<CoordMap> instance(std::make_shared<FromRaw>()) ;
	return instance ;
}

//-------------------------------------------------------------------------------------------------------------
namespace HypnoQuartz {
class ToRaw : public CubeCoordMap {
public:
	ToRaw() : CubeCoordMap() {}
	virtual ~ToRaw() {}
	virtual Coord translate(const Coord& coord) override
	{
		Coord mapped(coord.x(), 3 - coord.z(), coord.y()) ;
		return mapped ;
	}
};
}
std::shared_ptr<CoordMap> CubeCoordMap::toRawCoords()
{
	std::shared_ptr<CoordMap> instance(std::make_shared<ToRaw>()) ;
	return instance ;
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
CubeCoordMap::CubeCoordMap()
{
}

//-------------------------------------------------------------------------------------------------------------
CubeCoordMap::~CubeCoordMap()
{
}


