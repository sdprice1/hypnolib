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
 * \file      DebugMgr.cpp
 * \brief     Manages the debug
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
#include "DebugMgr.h"

#include <iostream>
#include <iomanip>
#include <sstream>


using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
DebugMgr::DebugMgr() :
	mGlobalLevel(IDebug::NONE),
	mLevels(),
	mOptionTimestamp(false),
	mOptionName(false),
	mOptionColour(false)

{
}

//-------------------------------------------------------------------------------------------------------------
DebugMgr::~DebugMgr()
{
}

//-------------------------------------------------------------------------------------------------------------
void DebugMgr::debugLevel(IDebug::DebugLevel level)
{
}

//-------------------------------------------------------------------------------------------------------------
std::string DebugMgr::getName() const
{
	return "" ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::isDebug(IDebug::DebugLevel level) const
{
	return false ;
}


//-------------------------------------------------------------------------------------------------------------
void DebugMgr::addName(const std::string& name)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	auto entry(mLevels.find(name)) ;
	if (entry != mLevels.end())
		return ;

	mLevels[name] = mGlobalLevel ;
}

//-------------------------------------------------------------------------------------------------------------
void DebugMgr::setDebug(DebugLevel level)
{
	std::unique_lock<std::mutex> lock(mMutex) ;

	// save global setting for objects added after this
	mGlobalLevel = level ;

	// set all current object levels
	for (auto& entry : mLevels)
	{
		entry.second = mGlobalLevel ;
	}
}

//-------------------------------------------------------------------------------------------------------------
void DebugMgr::setDebug(const std::string& name, DebugLevel level)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	mLevels[name] = level ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::isDebug(const std::string& name, DebugLevel level) const
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	auto entry(mLevels.find(name)) ;
	if (entry == mLevels.end())
		return false ;

	return ( static_cast<unsigned>(entry->second) >= static_cast<unsigned>(level) ) ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::getOptionsTimestamp() const
{
	return mOptionTimestamp ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::getOptionsName() const
{
	return mOptionName ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::getOptionsColour() const
{
	return mOptionColour ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::setOptionsTimestamp(bool enable)
{
	mOptionTimestamp = enable ;
	return mOptionTimestamp ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::setOptionsName(bool enable)
{
	mOptionName = enable ;
	return mOptionName ;
}

//-------------------------------------------------------------------------------------------------------------
bool DebugMgr::setOptionsColour(bool enable)
{
	mOptionColour = enable ;
	return mOptionColour ;
}

//=============================================================================================================
// Singleton
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IDebug> IDebug::getInstance()
{
	static std::shared_ptr<IDebug> instance(std::make_shared<DebugMgr>()) ;
	return instance ;
}

//=============================================================================================================
// IDebug
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void IDebug::dump(const std::string& msg, const uint8_t  * buffer, uint16_t  dataSize)
{
	std::cout << "=v========================================" << std::endl ;
	std::cout << "== " << msg << " == " << std::endl ;

	std::stringstream addSs ;
	std::stringstream dataSs ;
	std::string prevData ;
	std::string prevAscii ;
	bool abbrev{false} ;

	for (unsigned idx=0; idx < dataSize; )
	{
		addSs.str("") ;
		addSs << "0x" << std::setfill('0') << std::setw(4) << std::hex <<
				idx <<
				std::dec << "  " ;

		std::string ascii(16, ' ') ;
		unsigned byte(0) ;
		for (; byte < 16 && idx < dataSize; ++byte, ++idx, ++buffer)
		{
			dataSs << "0x" << std::setfill('0') << std::setw(2) << std::hex <<
					(unsigned)*buffer <<
					std::dec << " " ;

			if (byte == 7)
				dataSs << " " ;

			if (isprint(*buffer))
				ascii[byte] = (char)*buffer ;
			else
				ascii[byte] = '.' ;
		}
		for (; byte < 16; ++byte)
		{
			if (byte == 7)
				dataSs << " " ;
			dataSs << "     " ;
		}

		// check to see if this data is same as previous
		bool newAbbrev(false) ;
		if (dataSs.str() == prevData)
			newAbbrev = true ;

		// show we've been abbreviating if coming out of abbreviation
		if (abbrev && !newAbbrev)
			std::cout << "***                                                                                        ***" << std::endl ;

		// Don't output if currently abbreviating
		if (!newAbbrev)
			std::cout << addSs.str() << dataSs.str() << "  |" << ascii << "|" << std::endl ;

		prevData = dataSs.str() ;
		prevAscii = ascii ;
		abbrev = newAbbrev ;
		dataSs.str("") ;
	}

	// show we've been abbreviating if we still are
	if (abbrev)
	{
		std::cout << "***                                                                                        ***" << std::endl ;
		std::cout << addSs.str() << prevData << "  |" << prevAscii << "|" << std::endl ;
	}

	std::cout << "=^========================================" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void IDebug::dump(const std::string& msg, const std::vector<uint8_t>& buffer)
{
	dump(msg, &buffer[0], buffer.size()) ;
}


//-------------------------------------------------------------------------------------------------------------
void IDebug::addName(const std::string& name)
{
	std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->addName(name) ;
}


//-------------------------------------------------------------------------------------------------------------
void IDebug::setDebug(IDebug::DebugLevel level)
{
	std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->setDebug(level) ;
}

//-------------------------------------------------------------------------------------------------------------
void IDebug::setDebug(const std::string& name, IDebug::DebugLevel level)
{
	std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->setDebug(name, level) ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::isDebug(const std::string& name, IDebug::DebugLevel level)
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->isDebug(name, level) ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsTimestamp()
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->getOptionsTimestamp() ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsName()
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->getOptionsName() ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsColour()
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->getOptionsColour() ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsTimestamp(bool enable)
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->setOptionsTimestamp(enable) ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsName(bool enable)
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->setOptionsName(enable) ;
}

//-------------------------------------------------------------------------------------------------------------
bool IDebug::optionsColour(bool enable)
{
	return std::dynamic_pointer_cast<DebugMgr>(IDebug::getInstance())->setOptionsColour(enable) ;
}
