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
 * \file      CoordMap.h
 * \brief     Translate from one corrd system to another
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   Converts the coordinates from one space to another.
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
 * 			Z
 *
 * 			CUBE FRONT
 *
 */


#ifndef LIBHYPNOQUARTZ_MODEL_INC_COORDMAP_H_
#define LIBHYPNOQUARTZ_MODEL_INC_COORDMAP_H_

#include <memory>
#include "Coord.h"

namespace HypnoQuartz {

/*!
 * \class CoordMap
 */
class CoordMap {
public:
	virtual ~CoordMap() ;

	/**
	 * The operator converts the given coordinate into the alternate space
	 */
	virtual Coord translate(const Coord& coord) ;

protected:
	CoordMap() ;
} ;

}

#endif /* LIBHYPNOQUARTZ_MODEL_INC_COORDMAP_H_ */
