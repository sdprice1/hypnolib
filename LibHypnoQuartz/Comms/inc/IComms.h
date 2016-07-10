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
 * \file      IComms.h
 * \brief     Virtual interface to comms class
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 * \details   This defines a connection-like interface so that communications can use this common interface for multiple types of transfer
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_ICOMMS_H_
#define LIBHYPNOQUARTZ_COMMON_INC_ICOMMS_H_

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <memory>

namespace HypnoQuartz {

/*!
 * \class IComms
 */
class IComms {
public:
	IComms() {}
	virtual ~IComms() {}

	static const unsigned DEFAULT_MAX_CONNECTIONS{4} ;

	// General methods

	/**
	 * Close the connection
	 */
	virtual void close()  =0 ;

    /**
     * Method used to change the blocking status of the connection.
     * @param true sets connection as non-blocking, false set blocking
     */
    virtual void setNonBlocking(bool nonBlocking) =0 ;

    /**
     * Method used to set the connection in or out of binary mode (as opposed to text mode).
     * @param true sets connection as binary which handles binary messages (including NUL chars); false
     * treats messages as text terminating on NUL
     */
    virtual void setBinary(bool binary) =0 ;

    /**
     * select() method will return a set containing one or more of these
     */
    enum class SelectMode {
    	READ,
		WRITE,
		ERROR,
		TIMEOUT
    };

    /**
     * Do select on the file descriptor for this object. Defaults shown give blocking select until
     * read is available
     */
    virtual std::set<SelectMode> select(unsigned timeoutMs=0, bool read=true, bool write=false) =0 ;

    /**
     * send a string
     */
    virtual bool send(const std::string& data)  =0 ;

    /**
     * send a vector of bytes (in binary mode)
     */
    virtual bool send(const std::vector<uint8_t>& data)  =0 ;

    /**
     * receive a string
     */
    virtual bool receive(std::string& data)  =0 ;

    /**
     * Receive binary data into the buffer. The buffer will have the new data appended
     */
    virtual bool receive(std::vector<uint8_t>& data)  =0 ;


	// Client methods

	/**
	 * Connect to the server using "deviceName"
	 */
	virtual bool clientConnect(const std::string& deviceName)  =0 ;


	// Server methods

	/**
	 * Bind and listen to the connection using "deviceName"
	 */
	virtual bool serverListen(const std::string& deviceName, unsigned maxConnections = DEFAULT_MAX_CONNECTIONS)  =0 ;

	/**
	 * Accept a new connection. Returns pointer to new IComms or null
	 */
	virtual std::shared_ptr<IComms> accept() const  =0 ;

    /**
     * Get the actual number of maximum connections. Comms type may restrict this number to be less than requested.
     */
    virtual unsigned getMaxConnections() const =0 ;

} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_ICOMMS_H_ */
