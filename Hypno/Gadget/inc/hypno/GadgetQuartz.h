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
 * \file      GadgetQuartz.h
 *
 * \date      7 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNOQUARTZ_INC_GADGETQUARTZ_H_
#define LIBHYPNOQUARTZ_INC_GADGETQUARTZ_H_

#include <string>
#include <stack>
#include <vector>
#include <memory>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "hypno/Command.h"
#include "hypno/Gadget.h"
#include "hypno/Debuggable.h"
#include "hypno/GadgetClient.h"
#include "hypno/GadgetNoLock.hpp"

namespace HypnoQuartz {

class ReplyEvent ;

/*!
 * \class GadgetQuartz
 * \brief     This is a wrapper class for the HypnoGadget to provide a slightly simpler interface
 * \details   Multi-threaded class which handles comms to the device and provides methods for initiating transfers either blocking or
 * 			non-blocking (with a wait method to wait for all transfers to complete)
 */
class GadgetQuartz : public Debuggable {
public:
	GadgetQuartz() ;
	virtual ~GadgetQuartz() ;

	// connect to port
	bool connect(const std::string& port) ;

	// disconnect from port
	bool disconnect() ;

	// perform login
	bool doLogin() ;

	// perform logout
	bool doLogout() ;

	// get info
	bool getInfo() ;

	// get options
	bool getOptions(HypnoGadget::Options& opts) ;

	// Write out an animation frame (i.e. set all the pixels/LEDs)
	bool writeFrame(const std::vector<uint8_t >& image) ;
	bool writeFrame(const uint8_t * image) ;

	// write the new frame
	bool setFrame(const std::vector<uint8_t >& image) ;

	// switch frame buffers
	bool flipFrame() ;

private:
	// Callback for whenever a reply is received
	void replyCallback(HypnoGadget::Command::CommandType type, uint16_t  length, uint16_t  crc) ;

	// Wait for a specific reply type indefinitely. Will always return true when finds searched for type
	bool waitReply(HypnoGadget::Command::CommandType type) ;

	// Wait for a specific reply type or until timeout (in millisecs). Will return true when finds searched
	// for type; false if times out
	bool waitReply(HypnoGadget::Command::CommandType type, unsigned timeoutMs) ;

	// Wait for one of a set of specific reply types indefinitely. Will always return true when finds searched for type
	bool waitReply(const std::set<HypnoGadget::Command::CommandType>& types) ;

	// Wait for one of a set of specific reply types or until timeout (in millisecs). Will return true when finds
	// searched for type; false if times out
	bool waitReply(const std::set<HypnoGadget::Command::CommandType>& types, unsigned timeoutMs) ;

	// Low-level routine that actually runs the wait
	using WaitFunc = std::function<bool(std::unique_lock<std::mutex>& lock)> ;
	bool _waitReply(const std::set<HypnoGadget::Command::CommandType>& types, WaitFunc wait) ;

private:
	// Thread handler
	void threadRun() ;

private:
	std::shared_ptr<HypnoGadget::GadgetControl> mGadget ;
	GadgetClient mGadgetClient ;
	GadgetNoLock mGadgetLock ;

	bool mRun ;
	std::stack<std::shared_ptr<ReplyEvent>> mReplyEvents ;

	std::thread mThread ;
	std::mutex mMutex ;
	std::condition_variable mCondReply ;
} ;

}

#endif /* LIBHYPNOQUARTZ_INC_GADGETQUARTZ_H_ */
