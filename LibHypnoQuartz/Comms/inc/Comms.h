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
 * \file      Comms.h
 * \brief     Base IComms class
 *
 * \date      4 Jul 2016
 * \author    sdprice1
 *
 * \details   This is a base class that provides the IComms interface. It provides common methods that can be used by derived objects
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMS_INC_COMMS_H_
#define LIBHYPNOQUARTZ_COMMS_INC_COMMS_H_

#include "IComms.h"

namespace HypnoQuartz {

/*!
 * \class Comms
 */
class Comms : public virtual IComms {
public:
	Comms() ;
	virtual ~Comms() ;

	// General methods

	/**
	 * Close the connection
	 */
	virtual void close() override ;

    /**
     * Method used to change the blocking status of the connection.
     * @param true sets connection as non-blocking, false set blocking
     */
    virtual void setNonBlocking(bool nonBlocking) override ;

    /**
     * Method used to set the connection in or out of binary mode (as opposed to text mode).
     * @param true sets connection as binary which handles binary messages (including NUL chars); false
     * treats messages as text terminating on NUL
     */
    virtual void setBinary(bool binary) override ;

    /**
     * Do select on the file descriptor for this object. Defaults shown give blocking select until
     * read is available
     */
    virtual std::set<IComms::SelectMode> select(unsigned timeoutMs=0, bool read=true, bool write=false) override ;

    /**
     * send a string
     */
    virtual bool send(const std::string& data) override ;

    /**
     * send a vector of bytes (in binary mode)
     */
    virtual bool send(const std::vector<uint8_t>& data) override ;

    /**
     * receive a string
     */
    virtual bool receive(std::string& data) override ;

    /**
     * Receive binary data into the buffer. The buffer will have the new data appended
     */
    virtual bool receive(std::vector<uint8_t>& data) override ;


	// Client methods

	/**
	 * Connect to the server using "deviceName"
	 */
	virtual bool clientConnect(const std::string& deviceName) override ;


	// Server methods

	/**
	 * Bind and listen to the connection using "deviceName"
	 */
	virtual bool serverListen(const std::string& deviceName, unsigned maxConnections = DEFAULT_MAX_CONNECTIONS) override ;

	/**
	 * Accept a new connection. Returns pointer to new IComms or null
	 */
	virtual std::shared_ptr<IComms> accept() const override ;

    /**
     * Get the actual number of maximum connections. Comms type may restrict this number to be less than requested.
     */
    virtual unsigned getMaxConnections() const override ;

protected:
	/**
	 * Protected method for setting the file descriptor for this object
	 */
	void setFd(int fd) ;

	/**
	 * Protected method for getting the file descriptor for this object
	 */
	int getFd() const ;

	/**
	 * Is the object in non-blocking mode
	 */
	bool isNonBlocking() const ;

	/**
	 * Is the object in binary mode
	 */
	bool isBinary() const ;

	/**
	 * Set the maximum number of connections for the server
	 */
	void setMaxConnections(unsigned maxConnections) ;

private:
	int mFd ;
	bool mBinary ;
	bool mNonBlocking ;
	unsigned mMaxConnections ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMS_INC_COMMS_H_ */
