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
 * \file      Debuggable.cpp
 * \brief     
 *
 * \date      29 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "Debuggable.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Debuggable::Debuggable(const std::string& name) :
	debugNormal(this, IDebug::NORMAL),
	debugVerbose(this, IDebug::VERBOSE),
	mName(name)
{
	IDebug::addName(mName) ;
}

//-------------------------------------------------------------------------------------------------------------
Debuggable::~Debuggable()
{
}

//-------------------------------------------------------------------------------------------------------------
std::string Debuggable::getName() const
{
	return mName ;
}

//-------------------------------------------------------------------------------------------------------------
void Debuggable::debugLevel(IDebug::DebugLevel level)
{
	IDebug::setDebug(mName, level) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::isDebug(IDebug::DebugLevel level) const
{
	return IDebug::isDebug(mName, level) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::getOptionsTimestamp() const
{
	return IDebug::optionsTimestamp() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::getOptionsName() const
{
	return IDebug::optionsName() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::getOptionsColour() const
{
	return IDebug::optionsColour() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::setOptionsTimestamp(bool enable)
{
	return IDebug::optionsTimestamp(enable) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::setOptionsName(bool enable)
{
	return IDebug::optionsName(enable) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Debuggable::setOptionsColour(bool enable)
{
	return IDebug::optionsColour(enable) ;
}
