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
 * \file      Painter.cpp
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "hypno/Painter.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Painter::Painter() :
	mModel()
{
}

//-------------------------------------------------------------------------------------------------------------
Painter::~Painter()
{
}

//-------------------------------------------------------------------------------------------------------------
void Painter::setModel(std::shared_ptr<Model> newModel)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	mModel = newModel ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<Model> Painter::getModel() const
{
	return mModel ;
}

//-------------------------------------------------------------------------------------------------------------
void Painter::paint(std::shared_ptr<Model> newModel)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	mModel = newModel ;
	doPaint() ;
}
