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
 * \file      GadgetServer.h
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_GADGET_INC_GADGETSERVER_H_
#define LIBHYPNO_GADGET_INC_GADGETSERVER_H_

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../../HypnoSrc/inc/Gadget.h"  // the gadget interface
#include "CommsServer.h"
#include "Debuggable.h"

namespace HypnoQuartz {

/*!
 * \class 	GadgetServer
 * \brief   Server gadget
 * \details Creates a server connection for emulating a gadget
 */
class GadgetServer : public HypnoGadget::GadgetIO, public Debuggable
{
public:

	// default constructor
	GadgetServer(void) ;

	// default destructor
	virtual ~GadgetServer() ;

	/**
	 * Has the connection reset
	 */
	bool isReset() ;

	/**
	 * try to open a com port, given its name
	 * @param portName
	 * @return true on success, else false
	 */
	bool Open(const std::string & portName);

	/**
	 * close any open port
	 */
	void Close(void) ;

public:
	// HypnoGadget::GadgetIO interface

	// Needed by the GadgetControl class
	// read bytes from the IO buffer to be processed
	// of max given length, and return size read
	virtual uint16_t  ReadBytes(uint8_t  * buffer, uint16_t  length) override ;

	// Needed by the GadgetControl class
	// write bytes to the IO buffer
	virtual void WriteBytes(const uint8_t  * buffer, uint16_t  length) override ;

private:
	class EmuServer ;
	std::shared_ptr<EmuServer> mServer ;
	std::shared_ptr<IComms> mComms ;
} ;

}

#endif /* LIBHYPNO_GADGET_INC_GADGETSERVER_H_ */
