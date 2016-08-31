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
 * \file      Tty.h
 *
 * \date      30 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_
#define LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "hypno/Comms.h"

namespace HypnoQuartz {

/*!
 * Comms class for serial tty
 *
 * Socket-like interface for serial communications
 */
class Tty : public Comms {
public:
	Tty() ;
	virtual ~Tty() ;

	/**
	 * Determine whether the device name is a valid TTY name
	 */
	static bool isTtyName(const std::string& deviceName) ;

	// General methods

	/**
	 * Close the socket
	 */
	virtual FileStatus close()  override ;

	// Client methods

	/**
	 * Connect to the server socket using "deviceName" as described above
	 */
	virtual bool clientConnect(const std::string& deviceName)  override ;


	// Server methods

	/**
	 * Bind and listen to the socket using "deviceName" as described above
	 */
	virtual bool serverListen(const std::string& deviceName, unsigned maxConnections = DEFAULT_MAX_CONNECTIONS)  override ;

	/**
	 * Accept a new connection. Returns pointer to new IComms or null
	 */
	virtual std::shared_ptr<IComms> accept() const  override ;

private:
	// tty open
	bool ttyOpen(const std::string& device) ;

private:
	enum class TtyType {
		NONE,
		SERVER,
		CLIENT,
	};
	TtyType mType ;
    std::vector <uint8_t> mRxBuff ;
    mutable std::shared_ptr<IComms> mClient ;
} ;

}

#endif /* LIBHYPNOQUARTZ_COMMON_INC_TTYCOMMS_H_ */
