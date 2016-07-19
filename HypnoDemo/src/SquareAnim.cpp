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
 * \file      SquareAnim.cpp
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
#include "SquareAnim.h"
#include "GadgetQuartz.h"
#include "Colour.h"
#include "Coord.h"
#include "SquareModel.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
SquareAnim::SquareAnim(std::shared_ptr<GadgetQuartz> gadget) :
	AnimCallback(gadget, std::make_shared<SquareModel>()),
	mPos(0)
{
}

//-------------------------------------------------------------------------------------------------------------
SquareAnim::~SquareAnim()
{
}

//-------------------------------------------------------------------------------------------------------------
bool SquareAnim::newFrame()
{
	model()->clear() ;

#if 1
	int x,y;    // coordinates
	x = mPos&7;  // convert mPos 0-63 to x,y in [0,7]x[0,7]
	y = mPos/8;

	Colour randColour(Colour::rand()) ;
	model()->setPixel( Coord(x,y), randColour);
#else
	// draw testing axis
	for (int x = 0; x < 8; ++x)
		model()->setPixel( Coord(x,0), Colour::RED); // red x axis
	for (int y = 0; y < 8; ++y)
		model()->setPixel( Coord(0,y), Colour::GREEN); // green y axis
#endif

	// next pixel for next frame of animation
	mPos = (mPos+1)&63; // count 0-63 and repeat

	return true ;
}

