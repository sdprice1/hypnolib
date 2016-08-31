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
 * \file      AnimCallback.cpp
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

#include "hypno/AnimCallback.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
AnimCallback::AnimCallback(std::shared_ptr<GadgetQuartz> gadget, std::shared_ptr<Model> model) :
	mGadget(gadget),
	mModel(model)
{
}

//-------------------------------------------------------------------------------------------------------------
AnimCallback::~AnimCallback()
{
}

//-------------------------------------------------------------------------------------------------------------
bool AnimCallback::newFrame()
{
	// Must override this
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
std::vector<uint8_t> AnimCallback::getData()
{
	return mModel->toPacked() ;
}

//-------------------------------------------------------------------------------------------------------------
bool AnimCallback::paint(const std::vector<uint8_t>& data)
{
	return mGadget->writeFrame(data) ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<Model> AnimCallback::model()
{
	return mModel ;
}
