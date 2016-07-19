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
 * \file      CommsServer.cpp
 * \brief     
 *
 * \date      6 Jul 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "CommsServer.h"

#include <functional>
#include <algorithm>


using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned TICK_MS{100} ;

//=============================================================================================================
// CLASSES
//=============================================================================================================
using HandlerFunction = std::function<bool(std::shared_ptr<IComms> comms)> ;

//-------------------------------------------------------------------------------------------------------------
class CommsServer::CommsHandler : public Thread {
public:
	CommsHandler(unsigned index, HandlerFunction handler) :
		Thread("CommsHandler" + std::to_string(index)),
		mIndex(index),
		mHandler(handler),
		mComms()
	{
	}

	virtual ~CommsHandler()
	{
		this->Thread::exit() ;
	}

	virtual bool start(std::shared_ptr<IComms> comms)
	{
		mComms = comms ;
		this->Thread::start() ;
		return true ;
	}

	virtual bool isAvailable()
	{
		return !isRunning() ;
	}

protected:
	virtual bool run() override
	{
		// run the handler
		mHandler(mComms) ;

		// all done so close connection
		mComms->close() ;

		// stop this thread ready for next connection
		stopRequest() ;
		return false ;
	}

private:
	unsigned mIndex ;
	HandlerFunction mHandler ;
	std::shared_ptr<IComms> mComms ;
} ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
CommsServer::CommsServer(std::shared_ptr<IComms> comms, unsigned maxConnections) :
	Thread("CommsServer"),
	mComms(comms),
	mMaxConnections(maxConnections),
	mConnected(false),
	mMutex(),
	mHandlers()
{
}

//-------------------------------------------------------------------------------------------------------------
CommsServer::~CommsServer()
{
	this->Thread::exit() ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsServer::start(const std::string& connection)
{
	// TODO: Better error handling!

	if (!mComms->serverListen(connection, mMaxConnections))
		return false ;

	// update with the actual number of connections allowed
	mMaxConnections = mComms->getMaxConnections() ;

	// Create handlers
	for (unsigned num=1; num <= mMaxConnections; ++num)
	{
		mHandlers.push_back(std::make_shared<CommsHandler>(num, [this](std::shared_ptr<IComms> comms)->bool{
			return handler(comms) ;
		})) ;
	}

	// put comms into non-blocking mode
	mComms->setNonBlocking(true) ;

	// now start thread
	this->Thread::start() ;

	// wait for tick
	TimeUtils::msSleep(2*TICK_MS) ;

	return true ;

}


//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool CommsServer::isConnected() const
{
	return mConnected ;
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool CommsServer::run()
{
std::cerr << "CommsServer::run() - START" << std::endl ;

	mConnected = true ;
	while (mConnected && !isStop())
	{
		// Put sleep at start of loop so we can use "continue" in the if-guards to bring us back here and always perform a sleep
		TimeUtils::msSleep(TICK_MS) ;
		if (isStop())
			break ;

		try
		{
//			std::cerr << "CommsServer::run() - wait for accept...." << std::endl ;

			// NOTE: Because I've used a smart pointer, if it doesn't get passed to a handler (and therefore copied) then
			// it will go out of scope and automatically be closed
			std::shared_ptr<IComms> client(mComms->accept()) ;
			if (!client)
				continue ;

			std::cerr << "CommsServer::run() - accepted client @ " << client.get() << std::endl ;

			std::cerr << "-- Handlers ----" << std::endl ;
			for (auto hndl : mHandlers)
			{
				std::cerr << hndl->getName() << " : running=" << hndl->isRunning() << " stop=" << hndl->isStop() << std::endl ;
			}
			std::cerr << "----------------" << std::endl ;

			// Find an available handler
			auto handler(std::find_if(mHandlers.begin(), mHandlers.end(), [this](const std::shared_ptr<CommsHandler>& hdl)->bool{
				return hdl->isAvailable() ;
			})) ;
			if (handler == mHandlers.end())
				continue ;

			std::cerr << "CommsServer::run() - Using handler " << (*handler)->getName() << std::endl ;
			std::cerr << "CommsServer::run() - start client @ " << client.get() << std::endl ;

			// Pass client connection to this handler
			(*handler)->start(client) ;
		}
		catch (...)
		{
			mConnected = false ;
		}
	}

	std::cerr << "CommsServer::run() - END" << std::endl ;
	mComms->close() ;
	return false ;
}

