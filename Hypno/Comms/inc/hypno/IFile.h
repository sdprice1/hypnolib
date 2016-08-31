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
 * \file      IFile.h
 *
 * \date      2 Aug 2016
 * \author    sdprice1
 *
 */


#ifndef HYPNO_COMMS_INC_HYPNO_IFILE_H_
#define HYPNO_COMMS_INC_HYPNO_IFILE_H_

#include <cstdint>
#include <vector>
#include <set>

namespace HypnoQuartz {

/*!
 * \class 	IFile
 * \brief   Virtual interface for files/device access
 * \details 
 */
class IFile {
public:
	IFile() {}
	virtual ~IFile() {}

	enum class FileStatus {
		OK,
		CLOSED,
		TIMEOUT,
		ABORTED,
		ERROR
	};


	/**
	 * Return the raw file descriptor
	 */
	virtual int getFileDescriptor() const =0 ;

	/**
	 * Is the file descriptor open
	 * @return true if open
	 */
	virtual bool isOpen() const =0 ;

	/**
	 * Set the file descriptor. If a valid fd is currently set then that is closed before being changed to this new one
	 */
	virtual void setFd(int fd) =0 ;

	/**
	 * Close down - also closes the associated file descriptor
	 * @return status of close
	 */
	virtual FileStatus close() =0 ;

	/**
	 * Read the data into the buffer
	 * @param data
	 * @param numBytes		Requested number of bytes to read
	 * @param bytesRead		Actual number of bytes read
	 * @return status of transaction
	 */
	virtual FileStatus read(std::vector<uint8_t>& data, unsigned numBytes, unsigned& bytesRead) =0 ;

	/**
	 * Read the data into the buffer
	 * @param data
	 * @param numBytes		Requested number of bytes to read
	 * @param bytesRead		Actual number of bytes read
	 * @param timeoutMs		Timeout period in millisecs
	 * @return status of transaction
	 */
	virtual FileStatus read(std::vector<uint8_t>& data, unsigned numBytes, unsigned& bytesRead, unsigned timeoutMs) =0 ;

	/**
	 * Write the data from the buffer
	 * @param data
	 * @param bytesWritten	Ref to variable set to the number of bytes actually written
	 * @return status of transaction
	 */
	virtual FileStatus write(const std::vector<uint8_t>& data, unsigned& bytesWritten) =0 ;

	/**
	 * Write the data from the buffer
	 * @param data
	 * @param bytesWritten	Ref to variable set to the number of bytes actually written
	 * @param timeoutMs		Timeout period in millisecs
	 * @return status of transaction
	 */
	virtual FileStatus write(const std::vector<uint8_t>& data, unsigned& bytesWritten, unsigned timeoutMs) =0 ;

	/**
	 * Terminates any current access
	 */
	virtual void abort() =0 ;


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


} ;

}

#endif /* HYPNO_COMMS_INC_HYPNO_IFILE_H_ */
