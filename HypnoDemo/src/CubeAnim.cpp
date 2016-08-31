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
 * \file      CubeAnim.cpp
 * \brief     
 *
 * \date      23 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "CubeAnim.h"
#include "hypno/CubeModel.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================
const unsigned ON_STEPS{4} ;

//-------------------------------------------------------------------------------------------------------------
CubeAnim::CubeAnim(std::shared_ptr<GadgetQuartz> gadget) :
	AnimCallback(gadget, std::make_shared<CubeModel>()),
	mPos(0)
{
}

//-------------------------------------------------------------------------------------------------------------
CubeAnim::~CubeAnim()
{
}

//-------------------------------------------------------------------------------------------------------------
bool CubeAnim::newFrame()
{
	static std::vector<Colour> colours{
		Colour::RED,
		Colour::WHITE,
		Colour::BLUE,
		Colour::GREEN,
		Colour::CYAN,
		Colour::MAGENTA,
		Colour::YELLOW,
	} ;


	Colour colour(colours[mPos/ON_STEPS]) ;
	model()->fill(colour) ;

	mPos = (mPos+1) % (colours.size() * ON_STEPS) ;

	return true ;
}



//-------------------------------------------------------------------------------------------------------------
#if 0
bool CubeAnim::newFrame()
{
//	model()->clear() ;

#if 1
	int x,y,z;  // coordinates
	x = mPos/16; // convert mPos 0-63 to x,y,z in [0,3]x[0,3]x[0,3]
	y = (mPos/4)&3;
	z = mPos&3;
	model()->setPixel( Coord(x,y,z), Colour::rand() );
#else
	// draw testing axis
	for (int x = 0; x < 4; ++x)
		model()->setPixel( Coord(x,0,0), Colour::RED); // red x axis
	for (int y = 0; y < 4; ++y)
		model()->setPixel( Coord(0,y,0), Colour::GREEN); // green y axis
	for (int z = 0; z < 4; ++z)
		model()->setPixel( Coord(0,0,z), Colour::BLUE); // blue z axis
#endif

	// next pixel for next frame of animation
	mPos = (mPos+1)&63; // count 0-63 and repeat

	return true ;
}
#endif
