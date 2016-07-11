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
 * \file      Socket.cpp
 * \brief     
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <netinet/tcp.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include "Socket.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned MIN_PORT{1024} ;
const unsigned MAX_PORT{65535} ;

// Number of times to retry sending data before aborting
static const unsigned SEND_RETRY_COUNT{10};

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Socket::Socket() :
	Comms(),
	mType(SocketType::NONE),
	mPortNum(0),
	mAddrSize(0),
	mAddrPtr(nullptr),
	mAddrTcp(),
	mAddrUnix(),
	mUnixPath(),
	mRxBuff()
{
	std::cerr << "Socket NEW @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
Socket::~Socket()
{
	std::cerr << "Socket DEL @ " << this << " fd=" << getFd() << std::endl ;
	close() ;
	std::cerr << "Socket DEL - END @ " << this << " fd=" << getFd() << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void Socket::close()
{
	std::cerr << "Socket::close() @ " << this << " fd=" << getFd() << std::endl ;
	this->Comms::close() ;

	if (!mUnixPath.empty())
		::unlink(mUnixPath.c_str()) ;

	mType = SocketType::NONE ;
    mAddrSize = 0 ;
    mAddrPtr = nullptr ;
    mUnixPath.clear() ;

	std::cerr << "Socket::close() - END @ " << this << " fd=" << getFd() << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::send(const std::string& s)
{
	return send(s.c_str(), s.size()) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::send(const std::vector<uint8_t>& data)
{
	return send(&data[0], data.size()) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::receive(std::string& s)
{
	unsigned numBytes ;
	if (!receive(numBytes))
		return false ;

	if (isBinary())
	{
		// Use range constructor to ensure binary data (e.g. telnet control sequences) get copied across
		// properly (otherwise 0 will terminate the string prematurely)
		s = std::string(mRxBuff.begin(), mRxBuff.begin()+numBytes) ;
	}
	else
	{
		// text copy (to terminating NUL
		auto rxBegin(mRxBuff.begin()) ;
		auto rxEnd(mRxBuff.begin()+numBytes) ;
		auto pos(std::find(rxBegin, rxEnd, 0)) ;

		s = std::string(rxBegin, pos) ;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::receive(std::vector<uint8_t>& data)
{
	unsigned numBytes ;
	if (!receive(numBytes))
		return false ;

	data.insert(data.end(), mRxBuff.begin(), mRxBuff.begin()+numBytes) ;
	return true;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Socket::getPort() const
{
	return mPortNum ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::clientConnect(unsigned port, const std::string& host)
{
	return clientConnect(host + ":" + std::to_string(port)) ;
}


//-------------------------------------------------------------------------------------------------------------
bool Socket::clientConnect(const std::string& socketName)
{
	std::cerr << "Socket::clientConnect() @ " << this << std::endl ;
	// create the socket & set up address
	if (!socketFactory(socketName))
		return false ;

	int fd(getFd()) ;
    if (::connect(fd, mAddrPtr, mAddrSize) != -1)
    {
    	std::cerr << "Socket::clientConnect() @ " << this << " - connected fd=" << fd << std::endl ;
    	return true ;
    }

	::perror("connect") ;
	close();
	return false;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::serverListen(unsigned port, const std::string& host, unsigned maxConnections)
{
	return serverListen(host + ":" + std::to_string(port), maxConnections) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::serverListen(const std::string& socketName, unsigned maxConnections)
{
	std::cerr << "Socket::serverListen() @ " << this << std::endl ;

	setMaxConnections(maxConnections) ;

	// create the socket & set up address
	if (!socketFactory(socketName))
		return false ;

	int fd(getFd()) ;

	// For UNIX socket server needs to first remove the socket file
	if (mType == SocketType::UNIX )
	{
		if (::unlink(socketName.c_str()) == -1 && errno != ENOENT)
		{
			close() ;
			return false ;
		}

		mUnixPath = socketName ;
	}

	// Bind to the address
    if (::bind(fd, mAddrPtr, mAddrSize) == -1)
    {
    	::perror("listen") ;
    	close();
        return false;
    }

	// list for client connections
	if (::listen(fd, maxConnections ) == -1)
	{
		::perror("listen") ;
		close();
		return false;
	}

	std::cerr << "Socket::serverListen() @ " << this << " - listening fd=" << fd << std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> Socket::accept() const
{
	int fd(getFd()) ;

	if (fd < 0)
		return std::shared_ptr<IComms>() ;

    int client;
    socklen_t addrLen = mAddrSize ;
    if ((client = ::accept(fd, mAddrPtr, &addrLen)) >= 0)
        return std::shared_ptr<IComms>(new Socket(client)) ;

	return std::shared_ptr<IComms>() ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Socket::Socket(int fd) :
	Comms(),
	mType(SocketType::CLIENT),
	mPortNum(0),
	mAddrSize(0),
	mAddrPtr(nullptr),
	mAddrTcp(),
	mAddrUnix(),
	mUnixPath(),
	mRxBuff()
{
	setFd(fd) ;
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool Socket::socketFactory(const std::string& socketName)
{
	setFd(-1) ;
	mType = SocketType::NONE ;
    mAddrSize = 0 ;
    mAddrPtr = nullptr ;

	std::size_t pos(socketName.find(':')) ;

	// Check for a UNIX socket name
	if (pos == std::string::npos)
		return createUnix(socketName) ;

	// TCP
	std::string host(socketName.substr(0, pos)) ;
	std::string portStr(socketName.substr(pos+1)) ;

	// Look for a range
	unsigned portStart, portEnd ;
	if (portStr[0] == '*')
	{
		portStart = MIN_PORT ;
		portEnd = 65535 ;
	}
	else if ( (pos = portStr.find('-')) != std::string::npos )
	{
		portStart = static_cast<unsigned>( std::stoul(portStr.substr(0, pos)) ) ;
		portEnd = static_cast<unsigned>( std::stoul(portStr.substr(pos+1)) ) ;
		if (portStart < MIN_PORT)
			portStart = MIN_PORT ;
		if (portStart > MAX_PORT)
			portStart = MAX_PORT ;
		if (portEnd < MIN_PORT)
			portEnd = MIN_PORT ;
		if (portEnd > MAX_PORT)
			portEnd = MAX_PORT ;

		if (portStart > portEnd)
			portStart = portEnd ;
	}
	else
	{
		portStart = portEnd = static_cast<unsigned>(std::stoul(portStr)) ;
	}

	// now grab first valid port
	for (unsigned port = portStart; port <= portEnd; ++port)
	{
		if (createTcp(host, port))
			return true ;
	}

	return false ;
}


//-------------------------------------------------------------------------------------------------------------
bool Socket::createTcp(const std::string& host, unsigned port)
{
    ::memset(&mAddrTcp, 0, sizeof(mAddrTcp));

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    	return false ;

    // Set some common settings
    int on = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on)) == -1)
    {
    	::close(fd) ;
    	return false ;
    }
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*) &on, sizeof(on)) == -1)
    {
    	::close(fd) ;
    	return false ;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*) &on, sizeof(on)) == -1)
    {
    	::close(fd) ;
    	return false ;
    }

    mAddrTcp.sin_family = AF_INET;
    mAddrTcp.sin_port = htons(port);

    if (host == "*")
    {
    	// wildcard
    	mAddrTcp.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
    	// specific ip
		int result = ::inet_pton(AF_INET, host.c_str(), &mAddrTcp.sin_addr);
		if( (result == 0) && ( errno == EAFNOSUPPORT ) )
		{
			::close(fd) ;
			return false;
		}
    }

    // all good
    mType = SocketType::TCP ;
    setFd(fd) ;
    mPortNum = port ;
    mAddrSize = sizeof(mAddrTcp) ;
    mAddrPtr = reinterpret_cast<struct sockaddr*>(&mAddrTcp) ;
    return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::createUnix(const std::string& path)
{
    ::memset(&mAddrUnix, 0, sizeof(mAddrUnix));

    mAddrUnix.sun_family = AF_UNIX;
    if (path.size() >= sizeof(mAddrUnix.sun_path))
    	return false ;

    strncpy(mAddrUnix.sun_path, path.c_str(), sizeof(mAddrUnix.sun_path) - 1);
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        return false;

    setFd(fd) ;
    mType = SocketType::UNIX ;
    mAddrSize = sizeof(struct sockaddr_un) ;
    mAddrPtr = reinterpret_cast<struct sockaddr*>(&mAddrUnix) ;
    return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::send(const void* data, unsigned size) const
{
	int fd(getFd()) ;
	std::cerr << "Socket::send() @ " << this << " fd=" << fd << std::endl ;

	// Send the data - if we get the EAGAIN error then retry a few times before failing
	unsigned retryCount(SEND_RETRY_COUNT) ;
	while (retryCount > 0)
	{
		if (::send(fd, data, size, MSG_NOSIGNAL) != -1)
			return true;

::perror("send") ;

		if ( (errno != EAGAIN) && (errno != EWOULDBLOCK) )
			return false ;

		if (--retryCount == 0)
			return false ;

		// Pause
	    struct timespec ts{0, 1000};
	    nanosleep(&ts, (struct timespec *)0);
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::receive(unsigned& numBytes)
{
	// Allocate a receive buffer dynamically when it's first needed
    int maxRecBuf{8192};
	if (mRxBuff.empty())
	{
		mRxBuff.reserve(maxRecBuf) ;
		mRxBuff.resize(maxRecBuf, 0) ;
	}

	int fd(getFd()) ;
    int status = ::recv(fd, &mRxBuff[0], maxRecBuf, 0);

    if (status == -1)
    {
    	if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
    		// non-blocking
    		return true ;

		return false;
    }

    if (status == 0)
        return false;

    numBytes = (unsigned)status ;
	return true;
}
