/*!
 * Copyright (c) 2016 Steve Price.
 *
 * This file is part of Hypnolib
 *
 * Hypnolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hypnolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hypnolib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      GadgetTTY.cpp
 * \brief     
 *
 * \date      11 May 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

#include "Path.h"
#include "TimeUtils.h"
#include "GadgetTTY.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================
const unsigned BUFF_SIZE{8192} ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
GadgetTTY::GadgetTTY() :
	Debuggable("GadgetTTY"),
	mRun(true),
	mFd(-1),
	mTxBuffer(),
	mRxBuffer(),
	mThread()
{
	// now create thread
	mThread = std::thread(&GadgetTTY::threadRun, this) ;
}

//-------------------------------------------------------------------------------------------------------------
GadgetTTY::~GadgetTTY()
{
	Close();

	mRun = false ;
	mThread.join() ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::Open(const std::string& portName)
{
	// Ensure any existing connection is closed first
	Close() ;

	// determine which type of open to use
	if (portName.compare(0, 8, "/dev/tty") == 0)
		return ttyOpen(portName) ;

	// allow use of files, pipes, fifos etc
	return genericOpen(portName) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::waitOpen(const std::string& portName)
{
	debugNormal << "GadgetTTY::waitOpen(" << portName << ")" << std::endl ;

	// Ensure any existing connection is closed first
	Close() ;

	// determine which type of open to use
	if (portName.compare(0, 8, "/dev/tty") != 0)
		return genericOpen(portName) ;

	// Wait here indefinitely until device becomes available
	while (!Path::exists(portName))
	{
		debugNormal << " * waiting for " << portName << " ..." << std::endl ;
		sleep(1) ;
	}

	debugNormal << " * " << portName << " available, connecting..." << std::endl ;

	// try to open the device direct
	int fd = -1 ;
	while (fd < 0)
	{
		fd = ::open(portName.c_str(), O_RDWR) ;

		if (fd < 0)
		{
			debugNormal << " * * Failed to open " << portName << " error=" << errno << std::endl ;
			sleep(1) ;
		}
	}
	::close(fd) ;

	// now do the full open with terminal param set
	return Open(portName) ;
}


//-------------------------------------------------------------------------------------------------------------
void GadgetTTY::Close(void)
{
debugNormal << "GadgetTTY::Close()" << std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;

	if (mFd > 0)
		::close(mFd) ;

	mFd = -1 ;

debugNormal << "GadgetTTY::Close() - DONE" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
int GadgetTTY::readNoBlocking(uint8_t * buffer, uint16_t  length)
{
	// non-blocking check for data
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		if (mRxBuffer.empty())
			return 0 ;
	}

	return read(buffer, length) ;
}

//-------------------------------------------------------------------------------------------------------------
int GadgetTTY::read(uint8_t * buffer, uint16_t  length)
{
//	std::cout << "read " << length << " bytes ... " << std::endl ;

	int bytesRead(0) ;
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		while (mRxBuffer.empty() && mRun)
		{
			mCondRx.wait_for(lock, std::chrono::milliseconds(100)) ;
		}

		if (mRxBuffer.empty())
		{
			debugNormal << "Error: Reading from serial port failed.\n";
			return 0;
		}

		bytesRead = mRxBuffer.size() ;
		if (bytesRead > length)
			bytesRead = length ;

		std::copy(mRxBuffer.begin(), mRxBuffer.begin()+bytesRead, buffer) ;
		mRxBuffer.erase(mRxBuffer.begin(), mRxBuffer.begin()+bytesRead) ;
	}

//	dump("Read", buffer, bytesRead) ;

	return bytesRead;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::isTxDone()
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	return mTxBuffer.empty() ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::isRxDone()
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	return mRxBuffer.empty() ;
}


//=============================================================================================================
// HypnoGadget::GadgetIO interface
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
uint16_t  GadgetTTY::ReadBytes(uint8_t  * buffer, uint16_t  length)
{
	return (uint16_t )readNoBlocking(buffer, length);
}

//-------------------------------------------------------------------------------------------------------------
void GadgetTTY::WriteBytes(const uint8_t  * buffer, uint16_t  length)
{
//	dump("Writing", buffer, length) ;

	std::unique_lock<std::mutex> lock(mMutex) ;
	mTxBuffer.insert(mTxBuffer.end(), buffer, buffer+length) ;
}

//=============================================================================================================
// PRVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::ttyOpen(const std::string& portName)
{
	std::unique_lock<std::mutex> lock(mMutex) ;

	mFd = ::open(portName.c_str(), O_RDWR) ;
	if (mFd <0)
	{
		char buff[512] ;
		debugNormal << "Error: could not open port " << portName <<
				" : " << strerror_r(errno, buff, sizeof(buff)) <<
				" (" << errno << ")" <<
				std::endl;
		return false ;
	}

	struct termios oldtio,newtio;
	tcgetattr(mFd,&oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/*
	BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	CRTSCTS : output hardware flow control (only used if the cable has
			all necessary lines. See sect. 7 of Serial-HOWTO)
	CS8     : 8n1 (8bit,no parity,1 stopbit)
	CLOCAL  : local connection, no modem contol
	CREAD   : enable receiving characters
	*/
	newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
	newtio.c_cflag &= ~CRTSCTS; // no hardware handshake

	/*
	IGNPAR  : ignore bytes with parity errors
	ICRNL   : map CR to NL (otherwise a CR input on the other computer
			will not terminate input)
	otherwise make device raw (no other input processing)
	*/
	newtio.c_iflag = IGNBRK;
//	newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_iflag &= ~(IXON|IXOFF|IXANY); // no software handshake

	/*
	Raw output.
	*/
	newtio.c_oflag = 0;

	/*
	ICANON  : enable canonical input
	disable all echo functionality, and don't send signals to calling program
	*/
//	newtio.c_lflag = ICANON;
	newtio.c_lflag = 0;

	/*
	initialize all control characters
	default values can be found in /usr/include/termios.h, and are given
	in the comments, but we don't need them here
	*/
//	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
//	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
//	newtio.c_cc[VERASE]   = 0;     /* del */
//	newtio.c_cc[VKILL]    = 0;     /* @ */
//	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
//	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
//	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
//	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
//	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
//	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
//	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
//	newtio.c_cc[VEOL]     = 0;     /* '\0' */
//	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
//	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
//	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
//	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
//	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	newtio.c_cc[VTIME]=1;
	newtio.c_cc[VMIN]=60;

	/*
	now clean the modem line and activate the settings for the port
	*/
	tcflush(mFd, TCIFLUSH);

	if (tcsetattr(mFd,TCSANOW,&newtio) != 0)
	{
		char buff[512] ;
		debugNormal << "Error: unable to set attributes on port " << portName <<
				" : " << strerror_r(errno, buff, sizeof(buff)) <<
				" (" << errno << ")" <<
				std::endl;

		if (errno == EACCES)
		{
			debugNormal << "Check read/write permissions on " << portName <<
					" (you may need to add 'dialout' group or similar to your user)" <<
					std::endl ;
		}
		return false ;
	}

	mCondOpen.notify_all() ;

	return true;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetTTY::genericOpen(const std::string& portName)
{
	std::unique_lock<std::mutex> lock(mMutex) ;

	// just open the "file"
	mFd = ::open(portName.c_str(), O_RDWR) ;
	if (mFd <0)
	{
		char buff[512] ;
		debugNormal << "Error: could not open port " << portName <<
				" : " << strerror_r(errno, buff, sizeof(buff)) <<
				" (" << errno << ")" <<
				std::endl;
		return false ;
	}

	mCondOpen.notify_all() ;

	return true ;
}

//=============================================================================================================
// PRVATE THREAD
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void GadgetTTY::threadRun()
{
debugNormal << "GadgetTTY::threadRun" << std::endl ;

	// TODO - loop back to reconnect if connection lost, device removed or turned off

	// wait for device open
	{
		std::unique_lock<std::mutex> lock(mMutex) ;
		while ( (mFd < 0) && mRun)
		{
			mCondOpen.wait_for(lock, std::chrono::milliseconds(100)) ;
		}

		if (!mRun)
			return ;
	}

	debugNormal << "GadgetTTY::threadRun - device open" << std::endl ;

	// Run a select loop on the device to send/receive bytes
	while (mRun)
	{
		int nfds = 0 ;
		fd_set rd_fds, wr_fds, err_fds ;

		{
			std::unique_lock<std::mutex> lock(mMutex) ;

			nfds = mFd + 1 ;
			FD_ZERO(&rd_fds) ;
			FD_ZERO(&wr_fds) ;
			FD_ZERO(&err_fds) ;

			FD_SET(mFd, &rd_fds) ;
			FD_SET(mFd, &err_fds) ;

			// Only wait for write access if we have something to write
			if (!mTxBuffer.empty())
				FD_SET(mFd, &wr_fds) ;
		}

		// 100ms tick time
		struct timeval timeout{ 0, 100000 } ;
		int rc = ::select(nfds, &rd_fds, &wr_fds, &err_fds, &timeout) ;
		if (rc < 0)
		{
			debugNormal << "select error errno=" << errno << std::endl ;
			return ;
		}

		// Error
		if (FD_ISSET(mFd, &err_fds))
		{
			debugNormal << "FD error errno=" << errno << std::endl ;
			return ;
		}

		// Read
		if (FD_ISSET(mFd, &rd_fds))
		{
			char buff[BUFF_SIZE] ;
//			debugNormal << "Reading.. " << std::endl ;
			int num = ::read(mFd, buff, sizeof(buff)) ;
//			debugNormal << "reading=" << num << std::endl ;
			if (num <= 0)
			{
				debugNormal << "read failed errno=" << errno << std::endl ;
				return ;
			}

			std::unique_lock<std::mutex> lock(mMutex) ;
			mRxBuffer.insert(mRxBuffer.end(), buff, buff+num) ;
			mCondRx.notify_all() ;
		}


		// Write
		if (FD_ISSET(mFd, &wr_fds))
		{
			std::unique_lock<std::mutex> lock(mMutex) ;
			if (mTxBuffer.empty())
				continue ;

//			debugNormal << TimeUtils::timestamp() << " Writing.. " << std::endl ;
			int num = ::write(mFd, mTxBuffer.data(), mTxBuffer.size()) ;
//			debugNormal << "writing=" << num << std::endl ;
			if (num <= 0)
			{
				debugNormal << "write failed errno=" << errno << std::endl ;
				return ;
			}

			mTxBuffer.clear() ;
		}
	}


	debugNormal << "GadgetTTY::threadRun - END" << std::endl ;
}

