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
#include "hypno/CommsServer.h"

#include <functional>
#include <algorithm>

// for developer debug
#include "hypno/CommsDebug.h"

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
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] NEW" << std::endl ;
	}

	virtual ~CommsHandler()
	{
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] DEL" << std::endl ;
		if (mComms)
			mComms->close() ;
		this->Thread::exit() ;
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] DEL - END" << std::endl ;
	}

	virtual bool commsStart(std::shared_ptr<IComms> comms)
	{
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] START" << std::endl ;
		mComms = comms ;
		this->Thread::start() ;
		return true ;
	}

	virtual bool commsStop()
	{
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] STOP" << std::endl ;
		if (!mComms)
			return true ;

		// all done so close connection
		mComms->close() ;

		// stop this thread ready for next connection
		stopRequest() ;

		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] STOP - END" << std::endl ;
		return true ;
	}

	virtual bool commsExit()
	{
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] EXIT" << std::endl ;
		if (!mComms)
			return true ;

		// all done so exit connection
		mComms->exit() ;

		// exit this thread
		exitRequest() ;

		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] EXIT - END" << std::endl ;
		return true ;
	}

	virtual bool isAvailable()
	{
		return !isRunning() ;
	}

protected:
	virtual bool run() override
	{
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] RUN" << std::endl ;
		// run the handler
		mHandler(mComms) ;

		commsStop() ;
		DEBUG_COMMS_COUT << "CommsHandler[" << getName() << "] RUN - END" << std::endl ;
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
	mExiting(false),
	mMutex(),
	mHandlers()
{
}

//-------------------------------------------------------------------------------------------------------------
CommsServer::~CommsServer()
{
	mExiting = true ;
	DEBUG_COMMS_COUT << "CommsServer DEL" << std::endl ;

	// stop comms
	DEBUG_COMMS_COUT << "CommsServer DEL - kill comms..." << std::endl ;
	if (mComms)
		mComms->close() ;

	// Kill all of the handlers
	DEBUG_COMMS_COUT << "CommsServer DEL - kill handlers..." << std::endl ;
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		for (auto handler : mHandlers)
		{
			handler->commsExit() ;
		}
		mHandlers.clear() ;
	}

	// now kill this thread
	DEBUG_COMMS_COUT << "CommsServer DEL - kill this thread..." << std::endl ;
	this->Thread::exit() ;

	DEBUG_COMMS_COUT << "CommsServer DEL - END" << std::endl ;
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

//	// put comms into non-blocking mode
//	mComms->setNonBlocking(true) ;

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
DEBUG_COMMS_COUT << "CommsServer::run() - START" << std::endl ;

	mConnected = true ;
	while (okToRun())
	{
		// Put sleep at start of loop so we can use "continue" in the if-guards to bring us back here and always perform a sleep
		TimeUtils::msSleep(TICK_MS) ;
		if (!okToRun())
		{
			DEBUG_COMMS_COUT << "CommsServer::run() - not ok to run" << std::endl ;
			break ;
		}

		try
		{
			DEBUG_COMMS_COUT << "CommsServer::run() - wait for accept...." << std::endl ;

			// NOTE: Because I've used a smart pointer, if it doesn't get passed to a handler (and therefore copied) then
			// it will go out of scope and automatically be closed
			std::shared_ptr<IComms> client(mComms->accept()) ;
			if (!client)
				continue ;

			DEBUG_COMMS_COUT << "CommsServer::run() - accepted client @ " << client.get() << std::endl ;
			if (!okToRun())
			{
				DEBUG_COMMS_COUT << "CommsServer::run() - not ok to run" << std::endl ;
				break ;
			}

#ifdef DEBUG_COMMS
			DEBUG_COMMS_COUT << "-- Handlers ----" << std::endl ;
			for (auto hndl : mHandlers)
			{
				DEBUG_COMMS_COUT << hndl->getName() << " : running=" << hndl->isRunning() << " stop=" << hndl->isStop() << std::endl ;
			}
			DEBUG_COMMS_COUT << "----------------" << std::endl ;
#endif

			// Find an available handler
			auto handler(mHandlers.end()) ;
			{
				std::unique_lock<std::mutex> lock(mMutex) ;

				handler = (std::find_if(mHandlers.begin(), mHandlers.end(), [this](const std::shared_ptr<CommsHandler>& hdl)->bool{
					return hdl->isAvailable() ;
				})) ;
				if (handler == mHandlers.end())
					continue ;
			}

			DEBUG_COMMS_COUT << "CommsServer::run() - Using handler " << (*handler)->getName() << std::endl ;
			DEBUG_COMMS_COUT << "CommsServer::run() - start client @ " << client.get() << std::endl ;

			// Pass client connection to this handler
			(*handler)->commsStart(client) ;

#ifdef DEBUG_COMMS
			DEBUG_COMMS_COUT << "-- Handlers now ----" << std::endl ;
			for (auto hndl : mHandlers)
			{
				DEBUG_COMMS_COUT << hndl->getName() << " : running=" << hndl->isRunning() << " stop=" << hndl->isStop() << std::endl ;
			}
			DEBUG_COMMS_COUT << "----------------" << std::endl ;
#endif
		}
		catch (...)
		{
			mConnected = false ;
		}
	}

	DEBUG_COMMS_COUT << "CommsServer::run() - END" << std::endl ;
	mComms->close() ;

	// If we're exiting then tell thread to exit
	if (mExiting)
		this->exitRequest() ;

	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool CommsServer::okToRun()
{
	if (mExiting)
		return false ;

	if (mComms->isExit())
		return false ;

	if (isStop())
		return false ;

	if (!mConnected)
		return false ;

	return true ;
}
