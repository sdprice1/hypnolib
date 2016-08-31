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
 * \file      CommsServer.h
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_COMMSSERVER_H_
#define LIBHYPNOQUARTZ_COMMON_INC_COMMSSERVER_H_

#include <memory>
#include <vector>
#include <atomic>

#include "hypno/Thread.h"
#include "hypno/IComms.h"

namespace HypnoQuartz {


/*!
 * \class CommsServer
 * \brief     Threaded comms server
 * \details   Creates N threads for serving connections to clients (preforks the required number of threads)
 */
class CommsServer : public Thread {
public:
	CommsServer(std::shared_ptr<IComms> comms, unsigned maxConnections = 4) ;

	virtual ~CommsServer() ;

	/**
	 * Starts the connection. Returns true if connection made OK; false on any error
	 */
	virtual bool start(const std::string& connection) ;

protected:
	/**
	 * Shows whether we're still connected
	 */
	bool isConnected() const ;

protected:
	/**
	 * Virtual method which needs to be overriden by derived class to handle socket data
	 */
	virtual bool handler(std::shared_ptr<IComms> comms) =0 ;

private:
	// Thread handler
	virtual bool run() override ;

	// check conditions and see if thread is still ok to run
	bool okToRun() ;

private:
	std::shared_ptr<IComms> mComms ;
	unsigned mMaxConnections ;
	std::atomic<bool> mConnected ;
	std::atomic<bool> mExiting ;
	mutable std::mutex mMutex ;

	class CommsHandler ;
	std::vector<std::shared_ptr<CommsHandler>> mHandlers ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_COMMSSERVER_H_ */
