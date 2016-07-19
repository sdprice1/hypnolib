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
 * \file      GadgetClient.h
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_GADGET_INC_GADGETCLIENT_H_
#define LIBHYPNO_GADGET_INC_GADGETCLIENT_H_

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../../HypnoSrc/inc/Gadget.h"  // the gadget interface
#include "CommsClient.h"
#include "Debuggable.h"


namespace HypnoQuartz {

/*!
 * \class 	GadgetClient
 * \brief   Client gadget
 * \details Creates a client connection to the actual gadget (or emulator)
 */
class GadgetClient : public HypnoGadget::GadgetIO, public Debuggable
{
public:
	GadgetClient() ;
	virtual ~GadgetClient() ;

	/**
	 * try to open a com port, given its name. Waits for device to be available (if it isn't already) and then attempts to connect
	 * @param portName
	 * @return true on success, else false
	 */
	bool waitOpen(const std::string & portName);

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

	/**
	 * Show if the comms is open
	 */
	bool isOpen() const ;

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
	std::shared_ptr<CommsClient> mClient ;
	std::shared_ptr<IComms> mComms ;
} ;

}

#endif /* LIBHYPNO_GADGET_INC_GADGETCLIENT_H_ */
