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
 * \file      Coord.h
 * \brief     3D coordinate
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   x,y,z coordinate object. If z value is not specified it will default to 0.
 *
 * 			The coordinate components may be negative (either after a mathematical operation or
 * 			after being directly set). The application must determine how to handle thee negative values.
 *
 */


#ifndef LIBHYPNOQUARTZ_MODEL_INC_COORD_H_
#define LIBHYPNOQUARTZ_MODEL_INC_COORD_H_


namespace HypnoQuartz {

/*!
 * \class Coord
 */
class Coord {
public:
	Coord() ;
	Coord(int x, int y) ;
	Coord(int x, int y, int z) ;
	virtual ~Coord() ;

	const Coord operator+(const Coord& rhs) const ;
	const Coord operator-(const Coord& rhs) const ;
	Coord& operator+=(const Coord& rhs) ;
	Coord& operator-=(const Coord& rhs) ;

	/**
	 * Accessors
	 */
	int x() const ;
	int y() const ;
	int z() const ;

private:
	int mX ;
	int mY ;
	int mZ ;
} ;

}

#endif /* LIBHYPNOQUARTZ_MODEL_INC_COORD_H_ */
