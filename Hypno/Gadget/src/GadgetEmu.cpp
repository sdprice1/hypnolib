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
 * \file      GadgetEmu.cpp
 *
 * \date      18 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include <iostream>

#include "hypno/GadgetNoLock.hpp"
#include "hypno/GadgetEmu.h"

using namespace HypnoGadget ;
using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
GadgetEmu::GadgetEmu(std::shared_ptr<IPainter> painter) :
	Thread("GadgetEmu"),
	mPainter(painter),
	mModel(),
	mIo(std::make_shared<GadgetServer>()),
	mLock(std::make_shared<GadgetNoLock>()),
	mRxPacketState(),
	mTxPacketState(),
	mTxPacketBytes()
{
	PacketReset(&mRxPacketState) ;
	PacketReset(&mTxPacketState) ;

	// Create a new model based on the current painter model
	mModel = mPainter->getModel()->clone() ;
}

//-------------------------------------------------------------------------------------------------------------
GadgetEmu::GadgetEmu() :
	Thread("GadgetEmu"),
	mPainter(),
	mModel(),
	mIo(std::make_shared<GadgetServer>()),
	mLock(std::make_shared<GadgetNoLock>()),
	mRxPacketState(),
	mTxPacketState(),
	mTxPacketBytes()
{
	PacketReset(&mRxPacketState) ;
	PacketReset(&mTxPacketState) ;
}

//-------------------------------------------------------------------------------------------------------------
GadgetEmu::~GadgetEmu()
{
	mIo->Close() ;
	this->exit() ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::open(const std::string& deviceName)
{
	if (!mIo->Open(deviceName))
	{
		std::cerr << "Failed to open connection " << deviceName << std::endl ;
		return false ;
	}

	// ok to start thread
	this->start() ;

	return true ;
}


//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::run()
{
	// check emulator still running
	if (!mIo->isOpen())
		return false ;

	if (mIo->isReset())
	{
		PacketReset(&mRxPacketState) ;
		PacketReset(&mTxPacketState) ;
	}

	if (!doRead())
		return false ;

	if (!doWrite())
		return false ;

	return true ;
}


//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::doRead()
{
	// Wait for packets
	uint8_t packet[64] ;
	uint16_t num = mIo->ReadBytes(packet, sizeof(packet)) ;
	uint8_t* p = packet ;
	uint16_t plen = num ;

	if (num == 0)
		return true ;

	// Process all packets within this data block
	std::cerr << "Processing " << num << " bytes" << std::endl ;
	do
	{
		uint16_t numLeft = PacketDecodeBytes(&mRxPacketState, p, plen) ;
		p += plen - numLeft ;
		plen = numLeft ;

		std::cerr << "Got " << num << " bytes : left=" << numLeft << " : ready=" << mRxPacketState.dataBlockReady_ << std::endl ;

		uint8_t destination;
		uint8_t *data ;
		uint16_t length ;
		if (PacketGetData(&mRxPacketState, &destination, &data, &length))
		{
			if (length == 0)
			{
				std::cerr << "Invalid zero length data packet" << std::endl ;
				return false ;
			}

			uint8_t cmd(data[0]) ;
			std::cout << "Got cmd " << (unsigned)cmd << "  0x" << std::hex << (unsigned)cmd << std::dec << std::endl ;

			// skip command byte
			std::vector<uint8_t> datavec(data+1, data+length) ;

			std::cout << " * Encoded crc: 0x" << std::hex << PacketCRC(&mRxPacketState, false) << std::dec << std::endl ;
			std::cout << " * Decoded crc: 0x" << std::hex << PacketCRC(&mRxPacketState, true) << std::dec << std::endl ;

			// get decoded CRC
			uint16_t crc( PacketCRC(&mRxPacketState, true) ) ;
			uint8_t seq( PacketSequence(&mRxPacketState) ) ;

			switch (cmd)
			{
			case  Command::Login:
				handleLogin(datavec, seq, crc) ;
				break ;


			case  Command::SetFrame:
				// Update the model with this new data
				mModel->fromPacked(datavec) ;
				if (!sendAck(cmd, seq, crc))
					return false ;
				continue ;

			case  Command::FlipFrame:
				// Update the visualisation
				mPainter->paint(mModel) ;
				if (!sendAck(cmd, seq, crc))
					return false ;
				continue ;


			case  Command::Logout:
			case  Command::Version:
			case  Command::Ping:
			case  Command::Error:
			case  Command::Reset:
			case  Command::Options:
			case  Command::GetError:
			case  Command::Info:
			case  Command::MaxVisIndex:
			case  Command::SelectVis:
			case  Command::MaxTranIndex:
			case  Command::SelectTran:
			case  Command::GetFrame:
			case  Command::SetPixel:
			case  Command::GetPixel:
			case  Command::CurrentItem:
			case  Command::SetRate:
			case  Command::DrawLine:
			case  Command::DrawBox:
			case  Command::FillImage:
			case  Command::SetPFrame:
			case  Command::ScrollText:
			case  Command::LoadAnim:
				// just ACK for now....
				if (!sendAck(cmd, seq, crc))
					return false ;
				continue ;

			case  Command::Ack:
			case  Command::Unknown:
			default:
				std::cerr << "Unknown command 0x" <<
					std::hex << (unsigned)cmd <<
					std::dec << " (" << (unsigned)cmd << ")" <<
					std::endl ;
				return false ;
			}
		}

	} while(plen > 0) ;

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::doWrite()
{
	if (mTxPacketBytes.empty())
		return true ;

	mIo->WriteBytes(&mTxPacketBytes[0], static_cast<uint16_t>(mTxPacketBytes.size()) );
	mTxPacketBytes.resize(0);

	return true ;
}


//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::sendPacket(const uint8_t* data, uint16_t length)
{
	return HypnoGadget::PacketSendData(&mTxPacketState, ioWriteByte, this, 0, data, length);
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::sendPacket(const std::vector<uint8_t> data)
{
	return sendPacket(&data[0], data.size()) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::sendAck(uint8_t  cmd, uint8_t  seq, uint16_t  crc)
{
	std::vector<uint8_t> ack ;
	ack.push_back(Command::Ack) ;
	ack.push_back( (crc & 0xff00) >> 8 ) ;
	ack.push_back( (crc & 0xff) ) ;
	ack.push_back(cmd) ;
	ack.push_back(seq) ;
	return sendPacket(ack) ;
}

//=============================================================================================================
// PRIVATE STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void GadgetEmu::ioWriteByte(void* param, uint8_t byte)
{
	GadgetEmu* self(reinterpret_cast<GadgetEmu*>(param)) ;
	self->mTxPacketBytes.push_back(byte) ;
}


//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
bool GadgetEmu::handleLogin(const std::vector<uint8_t>& data, uint8_t seq, uint16_t crc)
{
	return sendAck(Command::Login, seq, crc) ;
}

