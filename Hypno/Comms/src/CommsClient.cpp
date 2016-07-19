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
 * \file      CommsClient.cpp
 * \brief     
 *
 * \date      4 Jul 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "CommsClient.h"

#include <iostream>


using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
const unsigned TICK_TIME_MS{100} ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
CommsClient::CommsClient(std::shared_ptr<IComms> comms) :
	Thread("CommsClient"),
	mComms(comms),
	mMutex(),
	mCondRx(),
	mBlocking(false),
	mTxBuffer(),
	mRxBuffer()
{
	std::cerr << "CommsClient NEW @ " << this << " comms=" << mComms.get() << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
CommsClient::~CommsClient()
{
	std::cerr << "CommsClient DEL @ " << this << " comms=" << mComms.get() << std::endl ;
	this->Thread::exit() ;
}

//-------------------------------------------------------------------------------------------------------------
void CommsClient::setBlocking(bool blocking)
{
	mBlocking = blocking ;
}

//-------------------------------------------------------------------------------------------------------------
void CommsClient::setBinary(bool binary)
{
	mComms->setBinary(binary) ;
}


//-------------------------------------------------------------------------------------------------------------
bool CommsClient::start(const std::string& connection)
{
	if (!mComms->clientConnect(connection))
		return false ;

	// ensure blocking
	mComms->setNonBlocking(false) ;

	// now create thread
	this->Thread::start() ;

	return true ;
}


//-------------------------------------------------------------------------------------------------------------
bool CommsClient::sendData(const std::string& data)
{
	return mComms->send(std::vector<uint8_t>(data.begin(), data.end())) ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsClient::sendData(const std::vector<uint8_t>& data)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	mTxBuffer.insert(mTxBuffer.end(), data.begin(), data.end()) ;

IDebug::dump("CommsClient::sendData ", data) ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsClient::receiveData(std::string& str)
{
	std::vector<uint8_t> data ;
	if (!receiveData(data))
		return false ;

	str = std::string(data.begin(), data.end()) ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsClient::receiveData(std::vector<uint8_t>& data)
{
	data.clear() ;
	std::unique_lock<std::mutex> lock(mMutex) ;
	if (mBlocking)
	{
		// blocking - wait for data
		while (mRxBuffer.empty())
			mCondRx.wait(lock) ;
	}
	else
	{
		// non-blocking - no data so return immediate
		if (mRxBuffer.empty())
			return true ;
	}

	using std::swap ;
	swap(data, mRxBuffer) ;
	return true ;
}


//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> CommsClient::getComms()
{
	return mComms ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsClient::handler()
{
	// default does nothing
	return true ;
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool CommsClient::run()
{
	// Use select to wait until data is available, or we've reached a "tick"
	bool waitRead{true} ;
	bool waitWrite{false} ;
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		waitWrite = !mTxBuffer.empty() ;
	}
	std::set<IComms::SelectMode> selectSet( mComms->select(TICK_TIME_MS, waitRead, waitWrite) ) ;

	if (isStop())
		return true ;

	if (selectSet.find(IComms::SelectMode::TIMEOUT) != selectSet.end())
		return true ;

	// Read
	if (selectSet.find(IComms::SelectMode::READ) != selectSet.end())
	{
		std::vector<uint8_t> rx ;
		mComms->receive(rx) ;

		// buffer RX
		if (!rx.empty())
		{
			std::unique_lock<std::mutex> lock(mMutex) ;
			mRxBuffer.insert(mRxBuffer.end(), rx.begin(), rx.end()) ;
			mCondRx.notify_all() ;
		}

		// Get the child object to handle data - if returns false then stop
		if (!handler())
			return false ;
	}

	// Write
	if (selectSet.find(IComms::SelectMode::WRITE) != selectSet.end())
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		if (!mTxBuffer.empty())
		{
			mComms->send(mTxBuffer) ;
			mTxBuffer.clear() ;
		}
	}

	// Error
	if (selectSet.find(IComms::SelectMode::ERROR) != selectSet.end())
	{
		// TODO..
		std::cerr << "Error..." << std::endl ;
		return false ;
	}

	return true ;
}
