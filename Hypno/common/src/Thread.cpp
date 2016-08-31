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
	std::cerr << "Thread[" << mName <<"] NEW" << std::endl ;

	// now create thread
	mThread = std::thread(&Thread::threadRun, this) ;
}

//-------------------------------------------------------------------------------------------------------------
Thread::~Thread()
{
	std::cerr << "Thread[" << mName <<"] DEL" << std::endl ;

	// cause the thread to exit
	this->exit() ;

	std::cerr << "Thread[" << mName <<"] DEL - join" << std::endl ;
	mThread.join() ;

	std::cerr << "Thread[" << mName <<"] DEL - End" << std::endl ;
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
	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// send request
	startRequest() ;

	// wait for stop
	while(!mRunning)
	{
		TimeUtils::msSleep(1) ;
	}

	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::stop()
{
	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// Request a stop
	stopRequest() ;

	// wait for stop
	while(mRunning)
	{
		TimeUtils::msSleep(1) ;

		// Request a stop
		stopRequest() ;
	}

	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
bool Thread::exit()
{
	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << std::endl ;

	// wait for thread exit
	while(mThreadRunning)
	{
//		std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << " exit request..." << std::endl ;

		// send request
		exitRequest() ;

		TimeUtils::msSleep(1) ;
	}

	std::cerr << "Thread[" << mName <<"] " << __FUNCTION__ << " - DONE"<< std::endl ;
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

	std::cerr << "Thread[" << mName <<"] threadRun - START" << std::endl ;
	while (!mExit)
	{
		// wait for event which causes either exit or the thread to start running
		{
			std::cerr << "Thread[" << mName <<"] threadRun - waiting..." << std::endl ;
			std::unique_lock<std::mutex> lock(mMutex);
			while (!mRun && !mExit)
			{
				mCond.wait(lock) ;
				std::cerr << "Thread[" << mName <<"] threadRun - cond signal run=" << mRun << " exit=" << mExit << std::endl ;
			}

			std::cerr << "Thread[" << mName <<"] threadRun - run=" << mRun << " exit=" << mExit << std::endl ;

			if (mExit)
				break ;
		}

		mRunning = true ;
		std::cerr << "Thread[" << mName <<"] threadRun - running..." << std::endl ;
		while (mRun && !mExit)
		{
			if (!run())
			{
				mRun = false ;
				break ;
			}
		}
		mRunning = false ;
		std::cerr << "Thread[" << mName <<"] threadRun - not running" << std::endl ;
	}

	mThreadRunning = false ;

	std::cerr << "Thread[" << mName <<"] threadRun - END" << std::endl ;
}
