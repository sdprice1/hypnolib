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
 * \file      GadgetQuartz.cpp
 * \brief     
 *
 * \date      7 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <chrono>
#include <iostream>

#include "TimeUtils.h"
#include "GadgetQuartz.h"

using namespace HypnoGadget ;
using namespace HypnoQuartz ;

//=============================================================================================================
// TYPES
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
// Reply information
namespace HypnoQuartz {

struct ReplyEvent {
	ReplyEvent(Command::CommandType type, HypnoGadget::uint16 length, HypnoGadget::uint16 crc) :
		mType(type),
		mLength(length),
		mCrc(crc)
	{}

	ReplyEvent() :
		mType(Command::Unknown),
		mLength(0),
		mCrc(0)
	{}

	Command::CommandType mType ;
	HypnoGadget::uint16 mLength ;
	HypnoGadget::uint16 mCrc ;
};

}

//=============================================================================================================
// CONSTANTS
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
const unsigned TICK_MSECS{100} ;
const unsigned REPLY_DELAY_MSECS{1000} ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
GadgetQuartz::GadgetQuartz() :
	mGadget(),
	mGadgetTty(),
	mGadgetLock(),

	mRun(true),
	mReplyEvents(),
	mThread()
{
	mGadget = std::make_shared<HypnoGadget::GadgetControl>(mGadgetTty, &mGadgetLock) ;
	mThread = std::thread(&GadgetQuartz::threadRun, this) ;

	// Now register our callback
	mGadget->registerReplyCallback([this](Command::CommandType type, uint16 length, uint16 crc){
		replyCallback(type, length, crc) ;
	}) ;
}

//-------------------------------------------------------------------------------------------------------------
GadgetQuartz::~GadgetQuartz()
{
	mGadgetTty.Close() ;

	mRun = false ;
	mThread.join() ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::connect(const std::string& port)
{
	return mGadgetTty.waitOpen(port) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::disconnect()
{
	mGadgetTty.Close() ;
	return true ;
}


//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::doLogin()
{
	std::cerr << "Login()" << std::endl ;

	for (unsigned retry=1; retry <= 10; ++retry)
	{
		std::cerr << TimeUtils::timestamp() << " CMD: Login" << std::endl ;
		mGadget->Login();
//		if (!waitReply(Command::Ack, REPLY_DELAY_MSECS))
		if (!waitReply(Command::Login, REPLY_DELAY_MSECS))
			continue ;

		if (GadgetControl::LoggedIn == mGadget->GetState())
		{
			std::cerr << "Login() - OK" << std::endl ;
			return true;
		}

		// failed to login - logout and retry
		std::cerr << "Login() - RETRY" << std::endl ;
		doLogout() ;
	}

	std::cerr << "Login() - FAILED" << std::endl ;
	return false;

}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::doLogout()
{
	mGadget->Logout();
//	waitReply(Command::Ack, 100) ;
	waitReply(Command::Logout, 100) ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::getInfo()
{
	// get a lot of info for this pass
//	mGadget->MaxVisIndex();
//	mGadget->MaxTranIndex();
//	mGadget->Version();

	// gets device info, visualisation list, transitions list, and then the options

std::cerr << TimeUtils::timestamp() << " CMD: getInfo(0,0)" << std::endl ;
	mGadget->Info(0,0); // kick off the process

	// Keep waiting for replies until we've gathered all of the info
	while (true)
	{
		bool ok = waitReply(std::set<Command::CommandType>({Command::Info, Command::Options}), 1000) ;
		uint8 vizCount = mGadget->GetCount(GadgetControl::InfoType::VisualizationType) ;
		uint8 transCount = mGadget->GetCount(GadgetControl::InfoType::TransitionType) ;

std::cerr << "VIZ: " << (unsigned)vizCount << " TRANS: " << (unsigned)transCount << std::endl ;

		// when we time out we're done
		if (!ok)
			break ;

	}


	uint8 vizCount = mGadget->GetCount(GadgetControl::InfoType::VisualizationType) ;
	uint8 transCount = mGadget->GetCount(GadgetControl::InfoType::TransitionType) ;

std::cerr << "VIZ: " << (unsigned)vizCount << " TRANS: " << (unsigned)transCount << std::endl ;

	std::cerr << "Got info" << std::endl ;
	for (uint8 viz=0; viz < vizCount; ++viz)
	{
		std::string name ;
		mGadget->GetName(GadgetControl::InfoType::VisualizationType, name, viz);
		std::cerr << (unsigned)viz << ": " << name << std::endl ;
	}

	for (uint8 trans=0; trans < transCount; ++trans)
	{
		std::string name ;
		mGadget->GetName(GadgetControl::InfoType::TransitionType, name, trans);
		std::cerr << (unsigned)trans << ": " << name << std::endl ;
	}

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::getOptions(HypnoGadget::Options& opts)
{
	// The only way we have of knowing the transfer worked is to check the options version
	// So start by inverting the bits
	opts.optionsVersion_ = ~OPTIONS_VERSION ;
	mGadget->SetOptions(opts) ;

	// now send the command to read the options
	std::cerr << TimeUtils::timestamp() << " CMD: getOptions()" << std::endl ;
	mGadget->Options(false);

	// wait for reply packet
	if (!waitReply(Command::Options, 10*REPLY_DELAY_MSECS))
		return false ;

	mGadget->GetOptions(opts) ;
	if (opts.optionsVersion_ == OPTIONS_VERSION)
	{
		std::cerr << "Options() - OK" << std::endl ;
		return true;
	}

	std::cerr << "Options() - FAILED" << std::endl ;
	return false;
}


//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::writeFrame(HypnoGadget::uint8* image)
{
	// send the image
	mGadget->SetFrame(image);

	// wait for reply packet
	if (!waitReply(Command::SetFrame, REPLY_DELAY_MSECS))
		return false ;

	// show the image
	mGadget->FlipFrame();

	// wait for reply packet
	if (!waitReply(Command::FlipFrame, REPLY_DELAY_MSECS))
		return false ;

	return true ;
}

//=============================================================================================================
// PRIVATE THREAD
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void GadgetQuartz::threadRun()
{
	while (mRun)
	{
		TimeUtils::msSleep(TICK_MSECS) ;

//std::cerr << "GadgetQuartz::threadRun()" << std::endl ;

		// Update gadget
		mGadget->Update();

		// Check for messages
		std::string text ;
		mGadget->ConsoleLog(text);
		mGadget->ConsoleClear() ;
		if (!text.empty())
		{
			std::cout << text << std::endl ;
		}

		// Check for errors
		std::string error ;
		if (mGadget->Error(error))
		{
			std::cerr << "ERROR: " << error << std::endl ;
		}

	}
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void GadgetQuartz::replyCallback(Command::CommandType type, HypnoGadget::uint16 length, HypnoGadget::uint16 crc)
{
	std::cerr << TimeUtils::timestamp() << " GadgetQuartz::replyCallback type=" << type << std::endl ;

	std::unique_lock<std::mutex> lock(mMutex) ;
	mReplyEvents.push(std::make_shared<ReplyEvent>(type, length, crc)) ;
	mCondReply.notify_all() ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::waitReply(Command::CommandType type)
{
	std::cerr << "waitReply type=" << type << std::endl ;

	return waitReply(std::set<Command::CommandType>({type})) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::waitReply(Command::CommandType type, unsigned timeoutMs)
{
	std::cerr << "waitReply type=" << type << " timeout=" << timeoutMs << std::endl ;

	return waitReply(std::set<Command::CommandType>({type}), timeoutMs) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::waitReply(const std::set<Command::CommandType>& types)
{
	return _waitReply(types, [this](std::unique_lock<std::mutex>& lock)->bool {
		mCondReply.wait(lock) ;
		return true ;
	}) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::waitReply(const std::set<Command::CommandType>& types, unsigned timeoutMs)
{
	auto now(std::chrono::system_clock::now()) ;
	auto untilTime(now + std::chrono::milliseconds(timeoutMs)) ;

	return _waitReply(types, [this, untilTime](std::unique_lock<std::mutex>& lock)->bool {
		return (mCondReply.wait_until(lock, untilTime)  == std::cv_status::no_timeout) ;
	}) ;
}

//-------------------------------------------------------------------------------------------------------------
bool GadgetQuartz::_waitReply(const std::set<Command::CommandType>& types, WaitFunc wait)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	while (true)
	{
		if (!wait(lock))
		{
			std::cerr << "waitReply timed out" << std::endl ;
			return false ;
		}

		while (!mReplyEvents.empty())
		{
			std::shared_ptr<ReplyEvent> event(mReplyEvents.top()) ;
			mReplyEvents.pop() ;
			if (types.find(event->mType) != types.end())
			{
				std::cerr << "waitReply DONE" << std::endl ;
				return true ;
			}
		}
	}

	// will never get here
	return false ;
}