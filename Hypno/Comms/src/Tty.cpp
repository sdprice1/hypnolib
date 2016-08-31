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
 * \file      Tty.cpp
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
#include "hypno/Tty.h"

#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <atomic>

// for developer debug
#include "hypno/CommsDebug.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CLIENT
//=============================================================================================================

namespace HypnoQuartz {
//-------------------------------------------------------------------------------------------------------------
class ClientTty : public Tty {
public:
	ClientTty() :
		mExit(false)
	{
		DEBUG_COMMS_COUT << "ClientTty NEW @ " << this << std::endl ;
	}

	virtual ~ClientTty()
	{
		DEBUG_COMMS_COUT << "ClientTty DEL @ " << this << std::endl ;
		close() ;
	}

	/**
	 * Complete shutdown
	 */
	virtual void exit() override
	{
		mExit = true ;
		this->close() ;
	}

	/**
	 * Close the connection
	 */
	virtual IFile::FileStatus close() override
	{
		std::stringstream ss ;
		ss << "ClientTty::close() @ " << this ;
		DEBUG_COMMS_COUT << ss.str() << std::endl ;

		setFd(-1) ;
		return IFile::FileStatus::OK ;
	}

	// This client connection is in use when it's file descriptor is set
	bool isInUse() const
	{
		bool inUse(mExit || isOpen()) ;

		std::stringstream ss ;
		ss << "ClientTty::isInUse(fd=" << getFileDescriptor() << ") @ " << this << " = " << inUse << " exit=" << mExit  ;
		DEBUG_COMMS_COUT << ss.str() << std::endl ;

		return inUse ;
	}

	bool isExiting() const
	{
		return mExit ;
	}

	void setInUse(int fd)
	{
		std::stringstream ss ;
		ss << "ClientTty::setInUse(fd=" << fd << ") @ " << this ;
		DEBUG_COMMS_COUT << ss.str() << std::endl ;

		if (mExit)
			fd = -1 ;

		setFd(fd) ;
	}

private:
	// Flag only set once and that's when the client is being exited
	// This then ensures the correct server accept behaviour
	std::atomic<bool> mExit ;
} ;
}

//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool Tty::isTtyName(const std::string& deviceName)
{
	if (deviceName.substr(0, 8) == "/dev/tty")
		return true ;
	return false ;
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Tty::Tty() :
	Comms(),
	mType(TtyType::NONE),
    mRxBuff(),
	mClient()
{
	DEBUG_COMMS_COUT << "Tty NEW @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
Tty::~Tty()
{
	DEBUG_COMMS_COUT << "Tty DEL @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
IFile::FileStatus Tty::close()
{
	DEBUG_COMMS_COUT << "Tty::close() @ " << this << std::endl ;

	return this->Comms::close() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Tty::clientConnect(const std::string& deviceName)
{
	if (!ttyOpen(deviceName))
		return false ;

	mType = TtyType::CLIENT ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Tty::serverListen(const std::string& deviceName, unsigned maxConnections)
{
	// can only ever have 1 connection
	setMaxConnections(1) ;

	if (!ttyOpen(deviceName))
		return false ;

	mType = TtyType::SERVER ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> Tty::accept() const
{
	// Need to create client first time round
	if (!mClient)
		mClient = std::make_shared<ClientTty>() ;

	std::shared_ptr<ClientTty> client(std::dynamic_pointer_cast<ClientTty>(mClient)) ;

	DEBUG_COMMS_COUT << "Tty::accept() @ " << this << std::endl ;


	// Don't accept if exiting
	if (client->isExiting())
	{
		DEBUG_COMMS_COUT << "Tty::accept() @ " << this << " client exiting" << std::endl ;

		return std::shared_ptr<IComms>() ;
	}

	// There can only ever be 1 accepted client
	if (client->isInUse())
	{
		DEBUG_COMMS_COUT << "Tty::accept() @ " << this << " client is in use" << std::endl ;

		return std::shared_ptr<IComms>() ;
	}

	// Skip if fd is closed
	if (!isOpen())
	{
		DEBUG_COMMS_COUT << "Tty::accept() @ " << this << " FD closed" << std::endl ;

		return std::shared_ptr<IComms>() ;
	}

	DEBUG_COMMS_COUT << "Tty::accept() - accepted : fd=" << getFileDescriptor() << " open? " << isOpen() << std::endl ;


	// return a copy of this fd
	client->setInUse(getFileDescriptor()) ;
	return mClient ;
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool Tty::ttyOpen(const std::string& device)
{
	DEBUG_COMMS_COUT << "Tty::ttyOpen(" << device << ") @ " << this << std::endl ;

	int fd = ::open(device.c_str(), O_RDWR) ;
	if (fd <0)
	{
		char buff[512] ;
		std::cerr << "Error: could not open device " << device <<
				" : " << strerror_r(errno, buff, sizeof(buff)) <<
				" (" << errno << ")" <<
				std::endl;
		return false ;
	}

	struct termios oldtio,newtio;
	tcgetattr(fd,&oldtio); /* save current serial port settings */
	::memset(&newtio, 0, sizeof(newtio)); /* clear struct for new port settings */

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
	tcflush(fd, TCIFLUSH);

	if (tcsetattr(fd,TCSANOW,&newtio) != 0)
	{
		char buff[512] ;
		std::cerr << "Error: unable to set attributes on device " << device <<
				" : " << strerror_r(errno, buff, sizeof(buff)) <<
				" (" << errno << ")" <<
				std::endl;

		if (errno == EACCES)
		{
			std::cerr << "Check read/write permissions on " << device <<
					" (you may need to add 'dialout' group or similar to your user)" <<
					std::endl ;
		}
		::close(fd) ;
		return false ;
	}

	setFd(fd) ;
	return true;
}

