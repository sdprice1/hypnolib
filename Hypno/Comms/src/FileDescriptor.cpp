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
 * \file      FileDescriptor.cpp
 *
 * \date      25 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>

#include "hypno/CommsException.h"
#include "hypno/FileDescriptor.h"

// for developer debug
#include "hypno/CommsDebug.h"

using namespace HypnoQuartz ;

#define IGNORE_RETURN		{int rc =
#define IGNORE_RETURN_END	; (void)rc; }

//=============================================================================================================
// LOCAL
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
namespace HypnoQuartz {
struct FileDescriptor::SelectData {
	SelectData() :
		timeoutMs(0),

		numBytesToRead(0),
		numBytesRead(nullptr),
		readData(nullptr),

		numBytesToWrite(0),
		numBytesWritten(nullptr),
		writeData(nullptr),

		selectSet(nullptr),
		selectRead(false),
		selectWrite(false)
	{}

	// Read
	SelectData(unsigned numReadBytes, std::vector<uint8_t>* data, unsigned* bytesRead, unsigned timeout=0) :
		timeoutMs(timeout),

		numBytesToRead(numReadBytes),
		numBytesRead(bytesRead),
		readData(data),

		numBytesToWrite(0),
		numBytesWritten(0),
		writeData(nullptr),

		selectSet(nullptr),
		selectRead(false),
		selectWrite(false)
	{
		*numBytesRead = 0 ;
	}

	// Write
	SelectData(const std::vector<uint8_t>* data, unsigned* bytesWritten, unsigned timeout=0) :
		timeoutMs(timeout),

		numBytesToRead(0),
		numBytesRead(nullptr),
		readData(nullptr),

		numBytesToWrite(data->size()),
		numBytesWritten(bytesWritten),
		writeData(data),

		selectSet(nullptr),
		selectRead(false),
		selectWrite(false)
	{
		*numBytesWritten = 0 ;
	}

	// Select
	SelectData(std::set<SelectMode>* select, unsigned timeout=0, bool read=true, bool write=false) :
		timeoutMs(timeout),

		numBytesToRead(0),
		numBytesRead(nullptr),
		readData(nullptr),

		numBytesToWrite(0),
		numBytesWritten(nullptr),
		writeData(nullptr),

		selectSet(select),
		selectRead(read),
		selectWrite(write)
	{}


public:
	unsigned timeoutMs ;

	// Read data
	unsigned numBytesToRead ;
	unsigned* numBytesRead ;
	std::vector<uint8_t>* readData ;

	// Write data
	unsigned numBytesToWrite ;
	unsigned* numBytesWritten ;
	const std::vector<uint8_t>* writeData ;

	// Select set
	std::set<IFile::SelectMode>* selectSet ;
	bool selectRead ;
	bool selectWrite ;
};
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
FileDescriptor::FileDescriptor() :
	mFd(-1),
	mPipeWrFd(-1),
	mPipeRdFd(-1),
	mMutex(),
	mAborting(false)
{
	DEBUG_COMMS_COUT << "FileDescriptor NEW @ " << this << " fd=" << mFd << std::endl ;

	init() ;
}


//-------------------------------------------------------------------------------------------------------------
FileDescriptor::FileDescriptor(int fd) :
	mFd(fd),
	mPipeWrFd(-1),
	mPipeRdFd(-1),
	mMutex(),
	mAborting(false)
{
	DEBUG_COMMS_COUT << "FileDescriptor NEW @ " << this << " fd=" << mFd << std::endl ;

	init() ;
}

//-------------------------------------------------------------------------------------------------------------
int FileDescriptor::getFileDescriptor() const
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	return mFd ;
}


//-------------------------------------------------------------------------------------------------------------
bool FileDescriptor::isOpen() const
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	return mFd > 0 ;
}

//-------------------------------------------------------------------------------------------------------------
void FileDescriptor::setFd(int fd)
{
	this->abort() ;

	std::unique_lock<std::mutex> lock(mMutex) ;
	closeFd() ;
	mFd = fd ;
}


//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus FileDescriptor::close()
{
	DEBUG_COMMS_COUT << "FileDescriptor::close() @ " << this  << " fd=" << mFd<< std::endl ;
	this->abort() ;

	std::unique_lock<std::mutex> lock(mMutex) ;
	closeFd() ;
	if (mPipeRdFd > 0)
		::close(mPipeRdFd) ;
	if (mPipeWrFd > 0)
		::close(mPipeWrFd) ;

	mPipeRdFd = -1 ;
	mPipeWrFd = -1 ;

	return IFile::FileStatus::OK ;
}

//-------------------------------------------------------------------------------------------------------------
FileDescriptor::~FileDescriptor()
{
	DEBUG_COMMS_COUT << "FileDescriptor DEL @ " << this  << " fd=" << mFd<< std::endl ;
	this->close() ;
	DEBUG_COMMS_COUT << "FileDescriptor DEL - END @ " << this  << " fd=" << mFd<< std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus FileDescriptor::read(std::vector<uint8_t>& data, unsigned numBytes, unsigned & bytesRead)
{
	return read(data, numBytes, bytesRead, 0) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus FileDescriptor::read(std::vector<uint8_t>& data, unsigned numBytes, unsigned & bytesRead, unsigned timeoutMs)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	DEBUG_COMMS_COUT << "FileDescriptor::read() @ " << this  << " fd=" << mFd<< std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;

	// Call the shared select method to do the transaction
	SelectData selData(numBytes, &data, &bytesRead, timeoutMs) ;
	return selectRW(selData) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus FileDescriptor::write(const std::vector<uint8_t>& data, unsigned & bytesWritten)
{
	return write(data, bytesWritten, 0) ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus FileDescriptor::write(const std::vector<uint8_t>& data, unsigned & bytesWritten, unsigned timeoutMs)
{
	if (!isOpen())
		return IFile::FileStatus::CLOSED ;

	DEBUG_COMMS_COUT << "FileDescriptor::write() @ " << this  << " fd=" << mFd<< std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;

	// Call the shared select method to do the transaction
	SelectData selData(&data, &bytesWritten, timeoutMs) ;
	return selectRW(selData) ;
}

//-------------------------------------------------------------------------------------------------------------
void FileDescriptor::abort()
{
	if (mPipeWrFd <= 0)
		return ;

	// Indicate abort
	mAborting = true ;

	// send dummy data - fill pipe with enough bytes to abort the 4 possible transactions: read, write, select, abort
	char buff[4] = {1, 2, 3, 4} ;
	IGNORE_RETURN ::write(mPipeWrFd, buff, 4) IGNORE_RETURN_END ;

	// Now take mutex - when all transactions have aborted we will get the lock (preventing other transactions)
	// and we then know that all transactions have stopped.
	std::unique_lock<std::mutex> lock(mMutex) ;

	// Now ok to finish
	mAborting = false ;

	// Flush out any remaining "abort bytes"
	setBlocking(mPipeRdFd, false) ;
	IGNORE_RETURN ::read(mPipeRdFd, buff, 4) IGNORE_RETURN_END ;
	setBlocking(mPipeRdFd, true) ;

	if (mFd > -1)
		DEBUG_COMMS_COUT << "FileDescriptor::abort() @ " << this  << " fd=" << mFd<< std::endl ;

}

//-------------------------------------------------------------------------------------------------------------
std::set<IFile::SelectMode> FileDescriptor::select(unsigned timeoutMs, bool read, bool write)
{
	if (!isOpen())
		return std::set<IFile::SelectMode>() ;

	DEBUG_COMMS_COUT << "FileDescriptor::select(read="<<read<<", write="<<write<<") @ " << this  << " fd=" << mFd<< std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;

	// Call the shared select method to do the transaction
	std::set<IFile::SelectMode> selectSet ;
	SelectData selData(&selectSet, timeoutMs, read, write) ;
	selectRW(selData) ;
	return selectSet ;
}



//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void FileDescriptor::init()
{
	// Block SIGPIPE signal so we can handle it via errno
	sigset_t blockSet, prevSet ;
	::sigemptyset(&blockSet) ;
	::sigaddset(&blockSet, SIGPIPE) ;
	::sigprocmask(SIG_BLOCK, &blockSet, &prevSet) ;

	// open a pipe
	int filedes[2] ;
	if (::pipe(filedes) != 0)
	{
		return ;
	}

	mPipeRdFd = filedes[0] ;
	mPipeWrFd = filedes[1] ;

	// Ensure write end is always available
	setBlocking(mPipeWrFd, false) ;
}

//-------------------------------------------------------------------------------------------------------------
void FileDescriptor::closeFd()
{
	if (mFd > 0)
		::close(mFd) ;
	mFd = -1 ;
}


//-------------------------------------------------------------------------------------------------------------
//IFile::FileStatus FileDescriptor::selectRW(SelectMode mode, std::vector<uint8_t>& data, unsigned & numBytes, unsigned timeoutMs)
IFile::FileStatus FileDescriptor::selectRW(SelectData& data)
{
	// stop if aborting
	if (mAborting || (mFd < 0) )
		return IFile::FileStatus::ABORTED ;


	// Check data
	if (data.numBytesToRead > 0)
		if (!data.readData)
			return IFile::FileStatus::ERROR ;
	if (data.numBytesToWrite > 0)
		if (!data.writeData)
			return IFile::FileStatus::ERROR ;

	// Simplify by setting flags on what to select on
	bool selRead = data.selectRead || (data.numBytesToRead > 0) ;
	bool selWrite = data.selectWrite || (data.numBytesToWrite > 0) ;

	DEBUG_COMMS_COUT << "FileDescriptor::select(" <<
			(selRead ? "READ " : "") <<
			(selWrite ? "WRITE " : "") <<
			") timeout=" << data.timeoutMs << " @ " << this  << " fd=" << mFd<< std::endl ;

	// Ensure we can break out if aborted
	int nfds = 0 ;
	fd_set rd_fds, wr_fds, err_fds ;

	FD_ZERO(&rd_fds) ;
	FD_ZERO(&wr_fds) ;
	FD_ZERO(&err_fds) ;

	FD_SET(mFd, &err_fds) ;
	FD_SET(mPipeRdFd, &rd_fds) ;

	if (selRead)
		FD_SET(mFd, &rd_fds) ;
	if (selWrite)
		FD_SET(mFd, &wr_fds) ;

	nfds = mFd + 1 ;
	if (nfds <= mPipeRdFd)
		nfds = mPipeRdFd + 1 ;

	struct timeval timeout{ 0, data.timeoutMs } ;
	struct timeval* timeoutPtr(nullptr) ;
	if (data.timeoutMs)
		timeoutPtr = &timeout ;
	int rc = ::select(nfds, &rd_fds, &wr_fds, &err_fds, timeoutPtr) ;
	if (rc == 0)
	{
		if (data.selectSet)
			data.selectSet->insert(IFile::SelectMode::TIMEOUT) ;
		return IFile::FileStatus::TIMEOUT ;
	}

	if (rc < 0)
	{
		DEBUG_COMMS_COUT << "FileDescriptor::select() ERROR @ " << this  << " fd=" << mFd<< std::endl ;
		if (data.selectSet)
			data.selectSet->insert(IFile::SelectMode::ERROR) ;
		return IFile::FileStatus::ERROR ;
	}

	// Error
	if (FD_ISSET(mFd, &err_fds))
	{
		DEBUG_COMMS_COUT << "FileDescriptor::select() ERROR FD @ " << this  << " fd=" << mFd<< std::endl ;
		if (data.selectSet)
			data.selectSet->insert(IFile::SelectMode::ERROR) ;
		return IFile::FileStatus::ERROR ;
	}

	// Abort
	if (FD_ISSET(mPipeRdFd, &rd_fds))
	{
		DEBUG_COMMS_COUT << "FileDescriptor::select() ABORT @ " << this  << " fd=" << mFd<< std::endl ;
		char dummy[1] ;
		IGNORE_RETURN ::read(mPipeRdFd, &dummy[0], 1) IGNORE_RETURN_END ;
		return IFile::FileStatus::ABORTED ;
	}

	// Write
	if (FD_ISSET(mFd, &wr_fds))
	{
		if (data.selectSet)
			data.selectSet->insert(IFile::SelectMode::WRITE) ;
		DEBUG_COMMS_COUT << "FileDescriptor::select() WRITE @ " << this  << " fd=" << mFd<< std::endl ;

		if (data.numBytesToWrite > 0)
		{
			rc = ::write(mFd, &(*data.writeData)[0], data.numBytesToWrite) ;
			if (rc < 0)
			{
				if (data.selectSet)
					data.selectSet->insert(IFile::SelectMode::ERROR) ;
				return IFile::FileStatus::ERROR ;
			}
			*(data.numBytesWritten) = static_cast<unsigned>(rc) ;
		}
	}

	// Read
	if (FD_ISSET(mFd, &rd_fds))
	{
		DEBUG_COMMS_COUT << "FileDescriptor::select() READ @ " << this  << " fd=" << mFd<< std::endl ;
		if (data.selectSet)
			data.selectSet->insert(IFile::SelectMode::READ) ;

		if (data.numBytesToRead)
		{
			std::vector<uint8_t> buff(data.numBytesToRead, 0) ;
			int rc = ::read(mFd, &buff[0], data.numBytesToRead) ;

	if (rc < 0) ::perror("READ") ;
	DEBUG_COMMS_COUT << "FileDescriptor::select() rc=" << rc << " @ " << this  << " fd=" << mFd<< std::endl ;

			if (rc < 0)
			{
				if (data.selectSet)
					data.selectSet->insert(IFile::SelectMode::ERROR) ;
				return IFile::FileStatus::ERROR ;
			}

			if (rc > 0)
			{
				std::vector<uint8_t>& vec(*(data.readData)) ;
				DEBUG_COMMS_COUT << "FileDescriptor::select() READ vec @ " << &vec << " END=" << (void*)&(*(vec.end())) << std::endl ;
				vec.insert(vec.end(), buff.begin(), buff.end()) ;
			}
			*(data.numBytesRead) = static_cast<unsigned>(rc) ;

			if (rc == 0)
			{
				if (mAborting)
					return IFile::FileStatus::ABORTED ;

				DEBUG_COMMS_COUT << "READ 0 bytes" << std::endl ;
				throw CommsException("Connection disconnected") ;
			}
		}
	}

	DEBUG_COMMS_COUT << "FileDescriptor::select() OK @ " << this  << " fd=" << mFd<< std::endl ;
	return IFile::FileStatus::OK ;
}

//-------------------------------------------------------------------------------------------------------------
void FileDescriptor::setBlocking(int fd, bool blocking)
{
    int opts = ::fcntl(fd, F_GETFL);

    if (opts <= 0)
    	return ;

	if (blocking)
	{
		opts = (opts & ~O_NONBLOCK);
	}
	else
	{
		opts = (opts | O_NONBLOCK);
	}
	(void)::fcntl(fd, F_SETFL, opts);
}

