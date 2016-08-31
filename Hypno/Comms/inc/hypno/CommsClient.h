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
 * \file      CommsClient.h
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_COMMSCLIENT_H_
#define LIBHYPNOQUARTZ_COMMON_INC_COMMSCLIENT_H_

#include "hypno/Thread.h"
#include "hypno/IComms.h"

namespace HypnoQuartz {

/*!
 * \class CommsClient
 * \brief     Thread for client comms connection
 * \details   Creates a client thread which will connect to the server
 */
class CommsClient : public Thread {
public:
	explicit CommsClient(std::shared_ptr<IComms> comms) ;

	virtual ~CommsClient() ;

//    /**
//     * Set the blocking mode of the receiveData() method. Default is non-blocking
//     * @param true sets connection as non-blocking, false set blocking
//     */
//    virtual void setBlocking(bool blocking) ;

    /**
     * Method used to set the connection in or out of binary mode (as opposed to text mode).
     * @param true sets connection as binary which handles binary messages (including NUL chars); false
     * treats messages as text terminating on NUL
     */
    virtual void setBinary(bool binary) ;

	/**
	 * Starts the connection. Returns true if connection made OK; false on any error
	 */
	virtual bool start(const std::string& connection) ;

	/**
	 * Send data - non blocking
	 * Returns false on error
	 */
	virtual bool sendData(const std::string& data) ;
    virtual bool sendData(const std::vector<uint8_t>& data) ;

	/**
	 * Receive data - non blocking
	 * Returns false on error (returns true if no error, even if no data)
	 */
    virtual bool receiveData(std::string& data, unsigned timeoutMs = 0) ;
    virtual bool receiveData(std::vector<uint8_t>& data, unsigned timeoutMs = 0) ;



protected:
	/**
	 * Hook method which can be overriden by derived class to handle receive data - will be called whenever data is received
	 * Return false to abort the client
	 */
	virtual bool handler() ;

protected:
	/**
	 * Get the comms object for I/O
	 */
	std::shared_ptr<IComms> getComms() ;

private:
	// Thread handler
	virtual bool run() override ;

private:
	std::shared_ptr<IComms> mComms ;
	std::mutex mMutex ;
	std::condition_variable mCondRx ;

	std::vector<uint8_t> mTxBuffer ;
	std::vector<uint8_t> mRxBuffer ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_SOCKETCLIENT_H_ */
