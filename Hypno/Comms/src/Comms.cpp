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

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>
#include <algorithm>

#include "hypno/FileDescriptor.h"
#include "hypno/Comms.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Comms::Comms() :
	mFd(std::make_shared<FileDescriptor>()),
	mBinary(false),
	mMaxConnections(IComms::DEFAULT_MAX_CONNECTIONS),
	mExiting(false)
{
}

//-------------------------------------------------------------------------------------------------------------
Comms::~Comms()
{
	std::cerr << "Comms DEL @ " << this << " fd=" << mFd << std::endl ;
	this->exit() ;
	std::cerr << "Comms DEL - END @ " << this << " fd=" << mFd << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::exit()
{
	mExiting = true ;
	this->close() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::isExit() const
{
	return mExiting ;
}

//-------------------------------------------------------------------------------------------------------------
int Comms::getFileDescriptor() const
{
	return mFd->getFileDescriptor() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::isOpen() const
{
	if (mExiting)
		return false ;
	return mFd->isOpen() ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Comms::close()
{
	std::cerr << "Comms::close() @ " << this << " fd=" << mFd << std::endl ;
	return mFd->close() ;
}


//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Comms::read(std::vector<uint8_t>& data, unsigned numBytes, unsigned & bytesRead)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	return mFd->read(data, numBytes, bytesRead) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Comms::read(std::vector<uint8_t>& data, unsigned numBytes, unsigned & bytesRead, unsigned timeoutMs)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	return mFd->read(data, numBytes, bytesRead, timeoutMs) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Comms::write(const std::vector<uint8_t>& data, unsigned & bytesWritten)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	return mFd->write(data, bytesWritten) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Comms::write(const std::vector<uint8_t>& data, unsigned & bytesWritten, unsigned timeoutMs)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	return mFd->write(data, bytesWritten, timeoutMs) ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::abort()
{
	mFd->abort() ;
}


//-------------------------------------------------------------------------------------------------------------
void Comms::setBinary(bool binary)
{
	mBinary = binary ;
}

//-------------------------------------------------------------------------------------------------------------
std::set<IComms::SelectMode> Comms::select(unsigned timeoutMs, bool read, bool write)
{
	if (!isOpen())
		return std::set<IComms::SelectMode>() ;

	return mFd->select(timeoutMs, read, write) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::send(const std::string& s)
{
	unsigned bytesWritten ;
	return this->write(std::vector<uint8_t>(s.begin(), s.end()), bytesWritten) == IFile::FileStatus::OK ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::send(const std::vector<uint8_t>& data)
{
	unsigned bytesWritten ;
	return this->write(data, bytesWritten) == IFile::FileStatus::OK ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::receive(std::string& s)
{
	std::vector<uint8_t> rxBuff ;
	unsigned numBytes ;
	if (!_receive(numBytes, rxBuff))
		return false ;

	if (isBinary())
	{
		// Use range constructor to ensure binary data (e.g. telnet control sequences) get copied across
		// properly (otherwise 0 will terminate the string prematurely)
		s = std::string(rxBuff.begin(), rxBuff.begin()+numBytes) ;
	}
	else
	{
		// text copy (to terminating NUL)
		auto pos(std::find(rxBuff.begin(), rxBuff.begin()+numBytes, (uint8_t)0)) ;
		s = std::string(rxBuff.begin(), pos) ;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::receive(std::vector<uint8_t>& data)
{
	std::vector<uint8_t> rxBuff ;
	unsigned numBytes ;
	if (!_receive(numBytes, rxBuff))
		return false ;

	data.insert(data.end(), rxBuff.begin(), rxBuff.begin()+numBytes) ;
	return true;
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
void Comms::setFd(int fd)
{
	mFd->setFd(fd) ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IFile> Comms::getFd() const
{
	return mFd ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::isBinary() const
{
	return mBinary ;
}

//-------------------------------------------------------------------------------------------------------------
void Comms::setMaxConnections(unsigned maxConnections)
{
	mMaxConnections = maxConnections ;
}

//-------------------------------------------------------------------------------------------------------------
bool Comms::_receive(unsigned& numBytes, std::vector<uint8_t>& rxBuff)
{
	numBytes = 0 ;

	if (!isOpen())
		return false ;

    int maxRecBuf{8192};
	rxBuff.clear() ;
	if (this->read( rxBuff, maxRecBuf, numBytes) != IFile::FileStatus::OK)
		return false ;

	return true;
}

