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
 * \file      GadgetClient.cpp
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "hypno/GadgetClient.h"

#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

#include "hypno/Path.h"
#include "hypno/TimeUtils.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned BUFF_SIZE{8192} ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
GadgetClient::GadgetClient() :
	Debuggable("GadgetClient"),
	mClient(),
	mComms()
{
}

//-------------------------------------------------------------------------------------------------------------
GadgetClient::~GadgetClient()
{
	Close();
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetClient::Open(const std::string& portName)
{
	// Ensure any existing connection is closed first
	Close() ;

	// Create comms
	mComms = IComms::factory(portName) ;
	if (!mComms)
		return false ;

	// Create client
	mClient = std::make_shared<CommsClient>(mComms) ;

	// Now open
	return mClient->start(portName) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetClient::waitOpen(const std::string& portName)
{
	debugNormal << "GadgetClient::waitOpen(" << portName << ")" << std::endl ;

	// Ensure any existing connection is closed first
	Close() ;

	// Create comms
	mComms = IComms::factory(portName) ;
	if (!mComms)
		return false ;

	// Create client
	mClient = std::make_shared<CommsClient>(mComms) ;

	// Wait here indefinitely until device becomes available
	while (!mClient->start(portName))
	{
		debugNormal << " * GadgetClient waiting for " << portName << " ..." << std::endl ;
		sleep(1) ;
	}

	debugNormal << " * GadgetClient " << portName << " available, connected" << std::endl ;

	return true ;
}


//-------------------------------------------------------------------------------------------------------------
void GadgetClient::Close(void)
{
debugNormal << "GadgetClient::Close()" << std::endl ;
	mClient.reset() ;
	mComms.reset() ;
debugNormal << "GadgetClient::Close() - DONE" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetClient::isOpen() const
{
	if (!mComms)
		return false ;
	if (!mClient)
		return false ;
	return true ;
}


//=============================================================================================================
// HypnoGadget::GadgetIO interface
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
uint16_t  GadgetClient::ReadBytes(uint8_t  * buffer, uint16_t  length)
{
	std::vector<uint8_t> data ;

	// Timeout after 100ms and keep going
	if (!mClient->receiveData(data, 100))
		return 0 ;

	if (length > data.size())
		length = data.size() ;

	::memcpy(buffer, &data[0], length) ;
	return length;
}

//-------------------------------------------------------------------------------------------------------------
void GadgetClient::WriteBytes(const uint8_t  * buffer, uint16_t  length)
{
	std::vector<uint8_t> data(buffer, buffer+length) ;
	mClient->sendData(data) ;
}


