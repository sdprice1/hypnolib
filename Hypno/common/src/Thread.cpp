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
 * \file      Thread.cpp
 * \brief     
 *
 * \date      6 Jul 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "hypno/Thread.h"

// Uncomment this line to enable verbose debug
//#define DEBUG_THREAD

#ifdef DEBUG_THREAD
#define DEBUG_THREAD_COUT	std::cout
#else
class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) { return c; }
};
static NullBuffer null_buffer;
static std::ostream null_stream(&null_buffer);
#define DEBUG_THREAD_COUT	null_stream
#endif

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Thread::Thread(const std::string& name) :
	Debuggable(name),
	mName(name),
	mRun(false),
	mExit(false),
	mRunning(false),
	mThreadRunning(false),
	mThread(),
	mMutex()
{
	DEBUG_THREAD_COUT << "Thread[" << mName <<"] NEW" << std::endl ;

	// now create thread
	mThread = std::thread(&Thread::threadRun, this) ;
}

//-------------------------------------------------------------------------------------------------------------
Thread::~Thread()
{
	DEBUG_THREAD_COUT << "Thread[" << mName <<"] DEL" << std::endl ;

	// cause the thread to exit
	this->exit() ;

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] DEL - join" << std::endl ;
	mThread.join() ;

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] DEL - End" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
std::string Thread::getName() const
{
	return mName ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::isRunning() const
{
	return mRunning ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::isStop() const
{
	return !mRun ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::start()
{
	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// send request
	startRequest() ;

	// wait for stop
	while(!mRunning)
	{
		TimeUtils::msSleep(1) ;
	}

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::stop()
{
	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// Request a stop
	stopRequest() ;

	// wait for stop
	while(mRunning)
	{
		TimeUtils::msSleep(1) ;

		// Request a stop
		stopRequest() ;
	}

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::exit()
{
	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// wait for thread exit
	while(mThreadRunning)
	{
//		DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << " exit request..." << std::endl ;

		// send request
		exitRequest() ;

		TimeUtils::msSleep(1) ;
	}

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
	return true ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Thread::startRequest()
{
	std::unique_lock<std::mutex> lock(mMutex);
	mRun = true ;
	mCond.notify_all() ;
}

//-------------------------------------------------------------------------------------------------------------
void Thread::stopRequest()
{
	std::unique_lock<std::mutex> lock(mMutex);
	mRun = false ;
	mCond.notify_all() ;
}

//-------------------------------------------------------------------------------------------------------------
void Thread::exitRequest()
{
	std::unique_lock<std::mutex> lock(mMutex);
	mRun = false ;
	mExit = true ;
	mCond.notify_all() ;
}

//=============================================================================================================
// PRIVATE
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Thread::threadRun()
{
	mThreadRunning = true ;

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - START" << std::endl ;
	while (!mExit)
	{
		// wait for event which causes either exit or the thread to start running
		{
			DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - waiting..." << std::endl ;
			std::unique_lock<std::mutex> lock(mMutex);
			while (!mRun && !mExit)
			{
				mCond.wait(lock) ;
				DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - cond signal run=" << mRun << " exit=" << mExit << std::endl ;
			}

			DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - run=" << mRun << " exit=" << mExit << std::endl ;

			if (mExit)
				break ;
		}

		mRunning = true ;
		DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - running..." << std::endl ;
		while (mRun && !mExit)
		{
			if (!run())
			{
				mRun = false ;
				break ;
			}
		}
		mRunning = false ;
		DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - not running" << std::endl ;
	}

	mThreadRunning = false ;

	DEBUG_THREAD_COUT << "Thread[" << mName <<"] threadRun - END" << std::endl ;
}
