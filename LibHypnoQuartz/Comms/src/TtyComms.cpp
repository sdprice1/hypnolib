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
 * \file      TtyComms.cpp
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

#include "TtyComms.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CLIENT
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
class ClientTtyComms : public TtyComms {
public:
	ClientTtyComms()
	{
		std::cerr << "ClientTtyComms NEW @ " << this << std::endl ;
	}

	virtual ~ClientTtyComms()
	{
		std::cerr << "ClientTtyComms DEL @ " << this << std::endl ;
		close() ;
	}

	/**
	 * Close the socket
	 */
	virtual void close() override
	{
		std::cerr << "ClientTtyComms::close() @ " << this << std::endl ;
		setFd(-1) ;
	}

	// This client connection is in use when it's file descriptor is set
	bool isInUse() const
	{
		return getFd() > 0 ;
	}

	void setInUse(int fd)
	{
		std::cerr << "TtyComms::setInUse(fd=" << fd << ") @ " << this << std::endl ;
		setFd(fd) ;
	}

} ;


//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TtyComms::TtyComms() :
	Comms(),
	mType(TtyType::NONE),
    mRxBuff(),
	mClient()
{
	std::cerr << "TtyComms NEW @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
TtyComms::~TtyComms()
{
	std::cerr << "TtyComms DEL @ " << this << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void TtyComms::close()
{
//	// skip close if client copy
//	if (mType == TtyType::CLIENT_COPY)
//		return ;

	std::cerr << "TtyComms::close() @ " << this << std::endl ;
	this->Comms::close() ;
}

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::send(const std::string& s)
{
	return send(s.c_str(), s.size()) ;
}

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::send(const std::vector<uint8_t>& data)
{
	return send(&data[0], data.size()) ;
}

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::receive(std::string& s)
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
bool TtyComms::receive(std::vector<uint8_t>& data)
{
	unsigned numBytes ;
	if (!receive(numBytes))
		return false ;

	data.insert(data.end(), mRxBuff.begin(), mRxBuff.begin()+numBytes) ;
	return true;
}



//-------------------------------------------------------------------------------------------------------------
bool TtyComms::clientConnect(const std::string& deviceName)
{
	if (!ttyOpen(deviceName))
		return false ;

	mType = TtyType::CLIENT ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::serverListen(const std::string& deviceName, unsigned maxConnections)
{
	// can only ever have 1 connection
	setMaxConnections(1) ;

	if (!ttyOpen(deviceName))
		return false ;

	mType = TtyType::SERVER ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<IComms> TtyComms::accept() const
{
	// Need to create client first time round
	if (!mClient)
		mClient = std::make_shared<ClientTtyComms>() ;

	std::shared_ptr<ClientTtyComms> client(std::dynamic_pointer_cast<ClientTtyComms>(mClient)) ;

	std::cerr << "TtyComms::accept() @ " << this << std::endl ;

	// There can only ever be 1 accepted client
	if (client->isInUse())
		return std::shared_ptr<IComms>() ;

	std::cerr << "TtyComms::accept() - accepted " << std::endl ;

	// return a copy of this fd
	client->setInUse(getFd()) ;
	return mClient ;
}

////=============================================================================================================
//// PROTECTED
////=============================================================================================================
//
////-------------------------------------------------------------------------------------------------------------
//TtyComms::TtyComms(int fd) :
//	Comms(),
//	mType(TtyType::CLIENT_COPY)
//{
//	setFd(fd) ;
//}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::send(const void* data, unsigned size) const
{
	if (size == 0)
		return true ;

	int num = ::write(getFd(), data, size) ;
	if (num <= 0)
	{
		// error handling?
		return false ;
	}
	if ( (unsigned)num < size)
		return false ;

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::receive(unsigned & numBytes)
{
	// Allocate a receive buffer dynamically when it's first needed
    int maxRecBuf{8192};
	if (mRxBuff.empty())
	{
		mRxBuff.reserve(maxRecBuf) ;
		mRxBuff.resize(maxRecBuf, 0) ;
	}

	int fd(getFd()) ;
    int status = ::read(fd, &mRxBuff[0], maxRecBuf);

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

//-------------------------------------------------------------------------------------------------------------
bool TtyComms::ttyOpen(const std::string& device)
{
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

