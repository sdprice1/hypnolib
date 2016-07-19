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
 * \file      Comms.cpp
 * \brief     
 *
 * \date      4 Jul 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "Comms.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>


using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Comms::Comms() :
	mFd(-1),
	mBinary(false),
	mNonBlocking(false),
	mMaxConnections(IComms::DEFAULT_MAX_CONNECTIONS)
{
}

//-------------------------------------------------------------------------------------------------------------
Comms::~Comms()
{
	std::cerr << "Comms DEL @ " << this << " fd=" << mFd << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::close()
{
	std::cerr << "Comms::close() @ " << this << " fd=" << mFd << std::endl ;
	if (mFd < 0)
		return ;

	::close(mFd) ;
	mFd = -1 ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::setNonBlocking(bool nonBlocking)
{
    int opts = ::fcntl(mFd, F_GETFL);

    if (opts <= 0)
    	return ;

	if (nonBlocking)
	{
		opts = (opts | O_NONBLOCK);
	}
	else
	{
		opts = (opts & ~O_NONBLOCK);
	}
	(void)::fcntl(mFd, F_SETFL, opts);

	mNonBlocking = nonBlocking ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::setBinary(bool binary)
{
	mBinary = binary ;
}

//-------------------------------------------------------------------------------------------------------------
std::set<IComms::SelectMode> Comms::select(unsigned timeoutMs, bool read, bool write)
{
	int nfds = 0 ;
	fd_set rd_fds, wr_fds, err_fds ;

	nfds = mFd + 1 ;
	FD_ZERO(&rd_fds) ;
	FD_ZERO(&wr_fds) ;
	FD_ZERO(&err_fds) ;

	FD_SET(mFd, &err_fds) ;

	if (read)
		FD_SET(mFd, &rd_fds) ;
	if (write)
		FD_SET(mFd, &wr_fds) ;

	// 100ms tick time
	struct timeval timeout{ 0, timeoutMs } ;
	struct timeval* timeoutPtr(nullptr) ;
	if (timeoutMs)
		timeoutPtr = &timeout ;
	int rc = ::select(nfds, &rd_fds, &wr_fds, &err_fds, timeoutPtr) ;
	if (rc < 0)
	{
		return std::set<IComms::SelectMode>() ;
	}

	std::set<IComms::SelectMode> selectSet ;

	// Error
	if (FD_ISSET(mFd, &err_fds))
	{
		return std::set<IComms::SelectMode>({IComms::SelectMode::ERROR}) ;
	}

	// Read
	if (FD_ISSET(mFd, &rd_fds))
		selectSet.insert(IComms::SelectMode::READ) ;

	// Write
	if (FD_ISSET(mFd, &wr_fds))
		selectSet.insert(IComms::SelectMode::WRITE) ;

	// Timeout
	if (selectSet.empty())
		selectSet.insert(IComms::SelectMode::TIMEOUT) ;

	return selectSet ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::send(const std::string& data)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::send(const std::vector<uint8_t>& data)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::receive(std::string& data)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::receive(std::vector<uint8_t>& data)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::clientConnect(const std::string& deviceName)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::serverListen(const std::string& deviceName, unsigned maxConnections)
{
	// must override
	return false ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> Comms::accept() const
{
	// override
	return std::shared_ptr<IComms>() ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Comms::getMaxConnections() const
{
	return mMaxConnections ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void HypnoQuartz::Comms::setFd(int fd)
{
	mFd = fd ;
}

//-------------------------------------------------------------------------------------------------------------
int Comms::getFd() const
{
	return mFd ;
}

//-------------------------------------------------------------------------------------------------------------
bool HypnoQuartz::Comms::isNonBlocking() const
{
	return mNonBlocking ;
}

//-------------------------------------------------------------------------------------------------------------
bool HypnoQuartz::Comms::isBinary() const
{
	return mBinary ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::setMaxConnections(unsigned maxConnections)
{
	mMaxConnections = maxConnections ;
}

