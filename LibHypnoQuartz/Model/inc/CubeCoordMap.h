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
 * \file      CubeCoordMap.h
 * \brief     Translations for cube coords
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   Translates from/to the internal 3D 4x4x4 cube coordinate system
 *
 * 			Internally we base our 3D coords as:
 *
 * 				Y
 * 				^
 * 				|
 * 				|
 * 				|
 * 				+---------------> X
 * 			   /
 * 			  /
 * 			 /
 * 			<
 * 			Z
 *
 * 			CUBE FRONT
 *
 *
 * 			The cube gadget itself is mapped as:
 *
 * 				Z    > Y
 * 				^   /
 * 				|  /
 * 				| /
 * 				|/
 * 				+---------------> X
 *
 * 			CUBE FRONT
 *
 *
 *
 */


#ifndef LIBHYPNOQUARTZ_MODEL_INC_CUBECOORDMAP_H_
#define LIBHYPNOQUARTZ_MODEL_INC_CUBECOORDMAP_H_

#include <memory>

#include "Coord.h"
#include "CoordMap.h"

namespace HypnoQuartz {

/*!
 * \class CubeCoordMap
 */
class CubeCoordMap : public CoordMap {
public:
	virtual ~CubeCoordMap() ;

	/**
	 * Singleton: This converts from the raw cube coordinates into our base coords
	 */
	static std::shared_ptr<CoordMap> fromRawCoords() ;

	/**
	 * Singleton: This converts to the raw cube coordinates from our base coords
	 */
	static std::shared_ptr<CoordMap> toRawCoords() ;


protected:
	CubeCoordMap() ;
} ;

}

#endif /* LIBHYPNOQUARTZ_MODEL_INC_CUBECOORDMAP_H_ */
