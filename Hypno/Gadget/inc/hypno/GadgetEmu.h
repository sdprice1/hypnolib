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
 * \file      GadgetEmu.h
 *
 * \date      18 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef CUBEEMU_INC_GADGETEMU_H_
#define CUBEEMU_INC_GADGETEMU_H_

#include <vector>
#include <memory>

#include "hypno/Packet.h"
#include "hypno/Thread.h"
#include "hypno/GadgetServer.h"
#include "hypno/Model.h"
#include "hypno/IPainter.h"

namespace HypnoQuartz {

/*!
 * \class 	GadgetEmu
 * \brief   Gadget emulator
 * \details Creates a server which emulates the gadget communications (and optional provides visualisation of sent frames)
 */
class GadgetEmu : public Thread {
public:
	/**
	 * Create simple gadget with no visualisation
	 */
	GadgetEmu() ;

	/**
	 * Create a gadget that uses the IPainter object to visualise
	 * @param painter
	 */
	explicit GadgetEmu(std::shared_ptr<IPainter> painter) ;

	virtual ~GadgetEmu() ;

	/**
	 * Opens the server connection and starts the thread if opened ok
	 * @param deviceName
	 * @return true if opened and started; false otherwise
	 */
	virtual bool open(const std::string& deviceName) ;

protected:
	/**
	 * Called in the thread. Must be provided by derived object. Expected to perform some sort of processing and then return to this object's
	 * "main loop". Code in this method can be a loop as long as it regularly checks isStop() and exit if true
	 */
	virtual bool run() override ;

private:
	bool doRead() ;
	bool doWrite() ;
	bool sendPacket(const uint8_t  * data, uint16_t  length) ;
	bool sendPacket(const std::vector<uint8_t > data) ;

	bool sendAck(uint8_t  cmd, uint8_t  seq, uint16_t  crc) ;
	bool handleLogin(const std::vector<uint8_t >& data, uint8_t  seq, uint16_t  crc) ;

private:
	static void ioWriteByte(void* param, uint8_t  byte) ;

private:
	std::shared_ptr<IPainter> mPainter ;
	std::shared_ptr<Model> mModel ;
	std::shared_ptr<GadgetServer> mIo ;
	std::shared_ptr<HypnoGadget::GadgetLock> mLock ;

	HypnoGadget::PacketHandlerState mRxPacketState ;
	HypnoGadget::PacketHandlerState mTxPacketState ;
	std::vector<uint8_t > mTxPacketBytes ;
} ;

}

#endif /* CUBEEMU_INC_GADGETEMU_H_ */
