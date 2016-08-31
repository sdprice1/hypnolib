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
 * \file      Thread.h
 *
 * \date      6 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_COMMON_INC_THREAD_H_
#define LIBHYPNO_COMMON_INC_THREAD_H_

#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <memory>

#include "hypno/Debuggable.h"

namespace HypnoQuartz {

/*!
 * \class 	Thread
 * \brief   Run a new thread
 * \details Wraps up common tasks for running a method as a new thread (and terminating properly)
 */
class Thread : public Debuggable {
public:
	explicit Thread(const std::string& name) ;
	virtual ~Thread() ;

	/**
	 * Get the name of this thread
	 */
	virtual std::string getName() const ;

	/**
	 * Is the thread currently running. The stop request may be active (i.e. isStop() will return true) but the thread
	 * may still be running (and will stop when it can)
	 */
	virtual bool isRunning() const ;

	/**
	 * Stop requested
	 */
	virtual bool isStop() const ;

	/**
	 * Start the thread running (again). After creation, thread is idle until run. The thread will then either run until stopped via stop()
	 * or until the threadRun() method exits
	 */
	virtual bool start() ;

	/**
	 * Stop the running thread. Blocks until thread stopped. Leaves the thread ready to be restarted
	 */
	virtual bool stop() ;

	/**
	 * Exit from the thread
	 */
	virtual bool exit() ;


protected:
	/**
	 * Schedule a thread start - non-blocking.
	 */
	virtual void startRequest() ;

	/**
	 * Schedule a thread stop - non-blocking. Used by thread "run" methods to schedule the thread to safely stop
	 */
	virtual void stopRequest() ;

	/**
	 * Schedule a thread exit - non-blocking.
	 */
	virtual void exitRequest() ;


protected:
	/**
	 * Called in the thread. Must be provided by derived object. Expected to perform some sort of processing and then return to this object's
	 * "main loop". Code in this method can be a loop as long as it regularly checks isStop() and exit if true
	 */
	virtual bool run() =0 ;

private:
	/**
	 * Thread handler
	 */
	virtual void threadRun() ;

private:
	const std::string mName ;
	std::atomic<bool> mRun ;
	std::atomic<bool> mExit ;
	std::atomic<bool> mRunning ;
	std::atomic<bool> mThreadRunning ;
	std::thread mThread ;
	std::mutex mMutex ;
	std::condition_variable mCond ;
} ;

}

#endif /* LIBHYPNO_COMMON_INC_THREAD_H_ */
