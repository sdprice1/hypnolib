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
 * \file      Socket.h
 * \brief     Linux base socket class
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 * \details   Provides for both client and server sockets. In both cases the socketName parameter can be either of these forms
 *
 * 			host:port			- TCP socket using host ip/name, and port number
 * 			*:port				- (server) TCP socket listening on any address, using port number
 * 			host:port1-port2	- TCP socket using host ip/name. Uses a port number picked between port1 up to port2
 * 			host:*				- TCP socket using host ip/name. Uses a port number picked between 1024 to 65535
 * 			path				- UNIX socket using path
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_SOCKET_H_
#define LIBHYPNOQUARTZ_COMMON_INC_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "IComms.h"
#include "Comms.h"

namespace HypnoQuartz {

/*!
 * \class Socket
 */
class Socket : public Comms {
public:
	Socket() ;
	virtual ~Socket() ;

	// -- Socket specific --

    /**
     * Returns the actual port number, or 0 if no port number available
     */
    virtual unsigned getPort() const ;

	/**
	 * Bind and listen to the socket using host and specified port number
	 */
	virtual bool serverListen(unsigned port, const std::string& host, unsigned maxConnections = IComms::DEFAULT_MAX_CONNECTIONS) ;

	/**
	 * Connect to server using host and specified port number
	 */
	virtual bool clientConnect(unsigned port, const std::string& host) ;



	// -- IComms interface --

	// General methods

	/**
	 * Close the socket
	 */
	virtual void close() override ;

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
	 * Connect to the server socket using "socketName" as described above
	 */
	virtual bool clientConnect(const std::string& socketName) override ;


	// Server methods

	/**
	 * Bind and listen to the socket using "socketName" as described above
	 */
	virtual bool serverListen(const std::string& socketName, unsigned maxConnections = IComms::DEFAULT_MAX_CONNECTIONS) override ;

	/**
	 * Accept a new connection. Returns pointer to new Socket or null
	 */
	virtual std::shared_ptr<IComms> accept() const override ;

protected:
	// Create a client Socket
	explicit Socket(int fd) ;

private:
	// Given the socket name, work out what type of socket to create and create it
	bool socketFactory(const std::string& socketName) ;

	// Create a new TCP socket
	bool createTcp(const std::string& host, unsigned port) ;

	// Create a new UNIX socket
	bool createUnix(const std::string& path) ;

	// low-level transmit
	bool send(const void* data, unsigned size) const ;

	// low-level receive
	bool receive(unsigned& numBytes) ;

private:
	enum class SocketType {
		NONE,
		TCP,
		UNIX,
		CLIENT
	};
	SocketType mType ;
	unsigned mPortNum ;

	unsigned mAddrSize ;
	struct sockaddr* mAddrPtr ;
	struct sockaddr_in mAddrTcp ;
	struct sockaddr_un mAddrUnix ;
	std::string mUnixPath ;
    std::vector <uint8_t> mRxBuff ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_SOCKET_H_ */
