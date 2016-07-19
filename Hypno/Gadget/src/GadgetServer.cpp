/*!
 * Copyright (c) 2016 Steve Price.
 *
 * This file is part of Hypnolib
 *
 * Hypnolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hypnolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hypnolib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      GadgetServer.cpp
 * \brief     
 *
 * \date      11 May 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "GadgetServer.h"

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

#include "Path.h"
#include "TimeUtils.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned BUFF_SIZE{8192} ;

//=============================================================================================================
// CLASSES
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------------
namespace HypnoQuartz {
class GadgetServer::EmuServer : public CommsServer
{
public:
	EmuServer(std::shared_ptr<IComms> comms) :
		CommsServer(comms),
		mClient(),
		mHasReset(false)
	{}
	virtual ~EmuServer() {}

	virtual bool handler(std::shared_ptr<IComms> comms)
	{
		mClient = comms ;
//		comms->setNonBlocking(false) ;

std::cerr << "<TEST> handler - START" << std::endl;

		while (isConnected())
		{
			TimeUtils::msSleep(100) ;
			if (!isConnected())
				break ;

			std::vector<uint8_t> rx ;
//			std::cerr << "<TEST> handler - Receive..." << std::endl;
			if (!comms->receive(rx))
				break ;
			if (rx.empty())
				continue ;

			std::cerr << "<TEST> handler - Received " << rx.size() << std::endl;
			std::unique_lock<std::mutex> lock(mMutex) ;
			mRx.insert(mRx.end(), rx.begin(), rx.end()) ;

IDebug::dump("handler RX:", rx) ;
IDebug::dump("current mRX:", mRx) ;
		}

		std::unique_lock<std::mutex> lock(mMutex) ;
		mClient.reset() ;
		mHasReset = true ;

std::cerr << "<TEST> handler - END" << std::endl;
		return false ;
	}

	void getRx(std::vector<uint8_t>& rx, unsigned maxSize)
	{
		rx.clear() ;

		std::unique_lock<std::mutex> lock(mMutex) ;
		if (mRx.empty())
			return ;

std::cerr << "getRx(" << maxSize <<") mRx.size=" << mRx.size() <<std::endl ;

		if (maxSize > mRx.size())
			maxSize = mRx.size() ;

		rx.insert(rx.end(), mRx.begin(), mRx.begin()+maxSize) ;

		// If all bytes transferred then we can clear down the buffer
		if (maxSize >= mRx.size())
		{
			mRx.clear() ;
			IDebug::dump("now mRX:", mRx) ;
			return ;
		}

		// need to suffle the end of the buffer forward
		std::vector<uint8_t> rest(mRx.begin()+maxSize, mRx.end()) ;
		using std::swap ;
		swap(mRx, rest) ;

		IDebug::dump("now mRX:", mRx) ;
	}

	void sendData(std::vector<uint8_t>& data)
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		mClient->send(data) ;
	}

	bool isReset()
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		bool reset(mHasReset) ;
		mHasReset = false ;
		return reset ;
	}

private:
	std::shared_ptr<IComms> mClient ;
	bool mHasReset ;
	std::vector<uint8_t> mRx ;
	std::mutex mMutex ;
};
}


//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
GadgetServer::GadgetServer() :
	Debuggable("GadgetServer"),
	mServer(),
	mComms()
{
}

//-------------------------------------------------------------------------------------------------------------
GadgetServer::~GadgetServer()
{
	Close();
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetServer::Open(const std::string& portName)
{
	// Ensure any existing connection is closed first
	Close() ;

	// Create comms
	mComms = IComms::factory(portName) ;
	if (!mComms)
		return false ;

	// Create server
	mServer = std::make_shared<EmuServer>(mComms) ;

	// Now open
	return mServer->start(portName) ;
}

//-------------------------------------------------------------------------------------------------------------
bool HypnoQuartz::GadgetServer::isReset()
{
	return mServer->isReset() ;
}

//-------------------------------------------------------------------------------------------------------------
void GadgetServer::Close(void)
{
debugNormal << "GadgetServer::Close()" << std::endl ;
	mServer.reset() ;
	mComms.reset() ;
debugNormal << "GadgetServer::Close() - DONE" << std::endl ;
}

//=============================================================================================================
// HypnoGadget::GadgetIO interface
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
uint16_t  GadgetServer::ReadBytes(uint8_t  * buffer, uint16_t  length)
{
	std::vector<uint8_t> data ;
	mServer->getRx(data, length) ;

	if (length > data.size())
		length = data.size() ;

	::memcpy(buffer, &data[0], length) ;
	return length;
}

//-------------------------------------------------------------------------------------------------------------
void GadgetServer::WriteBytes(const uint8_t  * buffer, uint16_t  length)
{
	std::vector<uint8_t> data(buffer, buffer+length) ;
	mServer->sendData(data) ;
}

