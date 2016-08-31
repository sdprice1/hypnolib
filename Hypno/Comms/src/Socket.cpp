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
#include "hypno/Socket.h"

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

// for developer debug
#include "hypno/CommsDebug.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned MIN_PORT{1024} ;
const unsigned MAX_PORT{65535} ;

// Number of times to retry sending data before aborting
static const unsigned SEND_RETRY_COUNT{10};


//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool Socket::isSocketName(const std::string& deviceName)
{
	unsigned portStart, portEnd ;
	bool isUnix, isTcp ;
	std::string host ;
	return checkName(deviceName, isUnix, isTcp, host, portStart, portEnd) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::isTcpSocketName(const std::string& deviceName)
{
	unsigned portStart, portEnd ;
	bool isUnix, isTcp ;
	std::string host ;
	if (!checkName(deviceName, isUnix, isTcp, host, portStart, portEnd))
		return false ;

	return isTcp ;
}

//-------------------------------------------------------------------------------------------------------------
bool Socket::isUnixSocketName(const std::string& deviceName)
{
	unsigned portStart, portEnd ;
	bool isUnix, isTcp ;
	std::string host ;
	if (!checkName(deviceName, isUnix, isTcp, host, portStart, portEnd))
		return false ;

	return isUnix ;
}

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
	mUnixPath()
{
	DEBUG_COMMS_COUT << "Socket NEW @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
Socket::~Socket()
{
	DEBUG_COMMS_COUT << "Socket DEL @ " << this << " fd=" << getFd() << std::endl ;
	close() ;
	DEBUG_COMMS_COUT << "Socket DEL - END @ " << this << " fd=" << getFd() << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Socket::close()
{
	DEBUG_COMMS_COUT << "Socket::close() @ " << this << " fd=" << getFd() << std::endl ;
	IFile::FileStatus status = this->Comms::close() ;

	if (!mUnixPath.empty())
		::unlink(mUnixPath.c_str()) ;

	mType = SocketType::NONE ;
    mAddrSize = 0 ;
    mAddrPtr = nullptr ;
    mUnixPath.clear() ;

	DEBUG_COMMS_COUT << "Socket::close() - END @ " << this << " fd=" << getFd() << std::endl ;
	return status ;
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
	DEBUG_COMMS_COUT << "Socket::clientConnect() @ " << this << std::endl ;
	// create the socket & set up address
	if (!socketFactory(socketName))
		return false ;

	if (!isOpen())
		return false ;

	int fd(getFileDescriptor()) ;
    if (::connect(fd, mAddrPtr, mAddrSize) != -1)
    {
    	DEBUG_COMMS_COUT << "Socket::clientConnect() @ " << this << " - connected fd=" << fd << std::endl ;
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
	DEBUG_COMMS_COUT << "Socket::serverListen() @ " << this << std::endl ;

	setMaxConnections(maxConnections) ;

	// create the socket & set up address
	if (!socketFactory(socketName))
		return false ;

	if (!isOpen())
		return false ;

	int fd(getFileDescriptor()) ;

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

	DEBUG_COMMS_COUT << "Socket::serverListen() @ " << this << " - listening fd=" << fd << std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> Socket::accept() const
{
	if (!isOpen())
		return std::shared_ptr<IComms>() ;

	// Do blocking select until read is available
	std::set<IFile::SelectMode> mode = getFd()->select() ;
	if (mode.find(IFile::SelectMode::READ) == mode.end())
		return std::shared_ptr<IComms>() ;

	// Should now be ok to do accept
	int fd(getFileDescriptor()) ;

    int client;
    socklen_t addrLen = mAddrSize ;
    if ((client = ::accept(fd, mAddrPtr, &addrLen)) >= 0)
        return std::shared_ptr<IComms>(new Socket(client)) ;

   	DEBUG_COMMS_COUT << "Socket::accept() @ " << this << " error " << std::endl ;
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
	mUnixPath()
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

	unsigned portStart, portEnd ;
	bool isUnix, isTcp ;
	std::string host ;
	if (!checkName(socketName, isUnix, isTcp, host, portStart, portEnd))
		return false ;

	// Check for a UNIX socket name
	if (isUnix)
		return createUnix(socketName) ;

	// TCP - grab first valid port
	for (unsigned port = portStart; port <= portEnd; ++port)
	{
		if (createTcp(host, port))
			return true ;
	}

	return false ;
}


//-------------------------------------------------------------------------------------------------------------
bool Socket::checkName(const std::string& socketName, bool& isUnix, bool& isTcp, std::string& host, unsigned& portStart, unsigned& portEnd)
{
	isUnix = false ;
	isTcp = false ;
	host = "" ;
	portStart = 0 ;
	portEnd = 0 ;

	std::size_t pos(socketName.find(':')) ;

	// Check for a UNIX socket name
	if (pos == std::string::npos)
	{
		// skip tty
		if (socketName.substr(0, 8) == "/dev/tty")
			return false ;

		isUnix = true ;
		return true ;
	}

	// TCP
	isTcp = true ;
	host = socketName.substr(0, pos) ;
	std::string portStr(socketName.substr(pos+1)) ;

	// Look for a range
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

	return true ;
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

