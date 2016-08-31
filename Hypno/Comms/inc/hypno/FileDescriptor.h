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
 * \file      FileDescriptor.h
 *
 * \date      20 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef HYPNO_COMMS_INC_HYPNO_FILEDESCRIPTOR_H_
#define HYPNO_COMMS_INC_HYPNO_FILEDESCRIPTOR_H_

#include <mutex>
#include <atomic>

#include <cstdint>
#include <vector>

#include "IFile.h"

namespace HypnoQuartz {

/*!
 * \class 	FileDescriptor
 * \brief   Interruptable file descriptor
 * \details Provides a class to manage a file descriptor. The class provides read/write access to this file descriptor
 * 			but makes these methods interruptable in that the abort() method will terminate any current access
 */
class FileDescriptor : public virtual IFile {
public:
	FileDescriptor() ;
	explicit FileDescriptor(int fd) ;
	virtual ~FileDescriptor() ;

	/**
	 * Return the raw file descriptor
	 */
	virtual int getFileDescriptor() const override ;

	/**
	 * Is the file descriptor open
	 * @return true if open
	 */
	virtual bool isOpen() const override ;

	/**
	 * Set the file descriptor. If a valid fd is currently set then that is closed before being changed to this new one
	 */
	virtual void setFd(int fd) override ;

	/**
	 * Close down - also closes the associated file descriptor
	 * @return status of close
	 */
	virtual FileStatus close() override ;

	/**
	 * Read the data into the buffer
	 * @param data
	 * @param numBytes		Requested number of bytes to read
	 * @param bytesRead		Actual number of bytes read
	 * @return status of transaction
	 */
	virtual FileStatus read(std::vector<uint8_t>& data, unsigned numBytes, unsigned& bytesRead) override ;

	/**
	 * Read the data into the buffer
	 * @param data
	 * @param numBytes		Requested number of bytes to read
	 * @param bytesRead		Actual number of bytes read
	 * @param timeoutMs		Timeout period in millisecs
	 * @return status of transaction
	 */
	virtual FileStatus read(std::vector<uint8_t>& data, unsigned numBytes, unsigned& bytesRead, unsigned timeoutMs) override ;

	/**
	 * Write the data from the buffer
	 * @param data
	 * @param bytesWritten	Ref to variable set to the number of bytes actually written
	 * @return status of transaction
	 */
	virtual FileStatus write(const std::vector<uint8_t>& data, unsigned& bytesWritten) override ;

	/**
	 * Write the data from the buffer
	 * @param data
	 * @param bytesWritten	Ref to variable set to the number of bytes actually written
	 * @param timeoutMs		Timeout period in millisecs
	 * @return status of transaction
	 */
	virtual FileStatus write(const std::vector<uint8_t>& data, unsigned& bytesWritten, unsigned timeoutMs) override ;

	/**
	 * Terminates any current access
	 */
	virtual void abort() override ;

    /**
     * Do select on the file descriptor for this object. Defaults shown give blocking select until
     * read is available
     */
    virtual std::set<IFile::SelectMode> select(unsigned timeoutMs=0, bool read=true, bool write=false) override ;

private:
    // shared class init
    void init() ;

    // close the existing fd
    void closeFd() ;

	// Run select on the fd
    struct SelectData ;
	FileStatus selectRW(SelectData& data) ;

	// Make the fd non-blocking / blocking
	void setBlocking(int fd, bool blocking) ;

private:
	int mFd ;
	int mPipeWrFd ;
	int mPipeRdFd ;
	mutable std::mutex mMutex ;
	std::atomic<bool> mAborting ;
} ;

}

#endif /* HYPNO_COMMS_INC_HYPNO_FILEDESCRIPTOR_H_ */
