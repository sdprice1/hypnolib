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
 * \file      TtyComms.h
 * \brief     Comms class for serial tty
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 * \details   Socket-like interface for serial communications
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_
#define LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "Comms.h"

namespace HypnoQuartz {

/*!
 * \class TtyComms
 */
class TtyComms : public Comms {
public:
	TtyComms() ;
	virtual ~TtyComms() ;

	// General methods

	/**
	 * Close the socket
	 */
	virtual void close()  override ;

    /**
     * send a string
     */
    virtual bool send(const std::string& data)  override ;

    /**
     * send a vector of bytes (in binary mode)
     */
    virtual bool send(const std::vector<uint8_t>& data)  override ;

    /**
     * receive a string
     */
    virtual bool receive(std::string& data)  override ;

    /**
     * Receive binary data into the buffer. The buffer will have the new data appended
     */
    virtual bool receive(std::vector<uint8_t>& data)  override ;


	// Client methods

	/**
	 * Connect to the server socket using "deviceName" as described above
	 */
	virtual bool clientConnect(const std::string& deviceName)  override ;


	// Server methods

	/**
	 * Bind and listen to the socket using "deviceName" as described above
	 */
	virtual bool serverListen(const std::string& deviceName, unsigned maxConnections = DEFAULT_MAX_CONNECTIONS)  override ;

	/**
	 * Accept a new connection. Returns pointer to new IComms or null
	 */
	virtual std::shared_ptr<IComms> accept() const  override ;

protected:
	// Create a client object
	explicit TtyComms(int fd) ;

private:
	// tty open
	bool ttyOpen(const std::string& device) ;

	// low-level transmit
	bool send(const void* data, unsigned size) const ;

	// low-level receive
	bool receive(unsigned& numBytes) ;

private:
	enum class TtyType {
		NONE,
		SERVER,
		CLIENT,
		CLIENT_COPY
	};
	TtyType mType ;
    std::vector <uint8_t> mRxBuff ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_ */
