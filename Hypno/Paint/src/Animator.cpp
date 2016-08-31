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
 * \file      Animator.cpp
 * \brief     
 *
 * \date      22 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "hypno/Animator.h"

#include <iostream>

#include "hypno/TimeUtils.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Animator::Animator() :
	Debuggable("Animator"),
	mRateMs(30),
	mNewFrame(),
	mGetData(),
	mPaint(),
	mRun(true),
	mStop(false),
	mAnimate(false),
	mThread()
{
	debugVerbose << "Animator NEW" << std::endl ;
	mThread = std::thread(&Animator::threadRun, this) ;
}

//-------------------------------------------------------------------------------------------------------------
Animator::~Animator()
{
	debugVerbose << "Animator DEL" << std::endl ;
	mRun = false ;
	stop() ;
	mThread.join() ;
	debugVerbose << "Animator DEL - done" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
void Animator::setFrameRate(unsigned rateMs)
{
	debugVerbose << "Animator::setFrameRate" << std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;
	mRateMs = rateMs ;
}

//-------------------------------------------------------------------------------------------------------------
bool Animator::start(NewFrameCallback newFrame,
		GetDataCallback getData, PaintCallback paint)
{
	debugVerbose << "Animator::start(callbacks)" << std::endl ;
	stop() ;

	// Set callbacks
	{
		std::unique_lock<std::mutex> lock(mMutex) ;

		mNewFrame = newFrame ;
		mGetData = getData ;
		mPaint = paint ;
	}

	return start() ;
}

//-------------------------------------------------------------------------------------------------------------
bool Animator::start(std::shared_ptr<IAnimatorCallback> callbackObj)
{
	return start(
		[callbackObj]()->bool { return callbackObj->newFrame(); },
		[callbackObj]()->std::vector<uint8_t> { return callbackObj->getData(); },
		[callbackObj](const std::vector<uint8_t>& data)->bool { return callbackObj->paint(data); }
	) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Animator::start()
{
	debugVerbose << "Animator::start" << std::endl ;
	// Check callbacks first
	{
		std::unique_lock<std::mutex> lock(mMutex) ;

		if (!mNewFrame)
			return false ;
		if (!mGetData)
			return false ;
		if (!mPaint)
			return false ;
	}

	stop() ;

	std::unique_lock<std::mutex> lock(mMutex) ;
	mAnimate = true ;

	debugVerbose << "Animator::start - done" << std::endl ;
	return true ;
}

//-------------------------------------------------------------------------------------------------------------
void Animator::stop()
{
//	debugVerbose << "Animator::stop" << std::endl ;
	std::unique_lock<std::mutex> lock(mMutex) ;
	if (!mAnimate)
	{
//		debugVerbose << "Animator::stop - already stopped" << std::endl ;
		return ;
	}

	mStop = true ;
	mCondEvent.notify_all() ;

	// Now we need to wait until gadget stopped
	mCondTick.wait(lock, [this]()->bool {return !mAnimate;} ) ;

//	debugVerbose << "Animator::stop - now stopped : animate=" << mAnimate << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
bool Animator::isRunning() const
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	return mAnimate ;
}

//=============================================================================================================
// PRIVATE THREAD
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Animator::threadRun()
{
	debugVerbose << "Animator::threadRun - START" << std::endl ;
	std::chrono::system_clock::time_point untilTime(std::chrono::system_clock::now() + std::chrono::milliseconds(mRateMs)) ;

	while (mRun)
	{
		bool animate(false) ;
		{
			std::unique_lock<std::mutex> lock(mMutex) ;
			while (mCondEvent.wait_until(lock, untilTime) == std::cv_status::no_timeout)
			{
				// Event
				if (!mRun)
				{
					debugVerbose << "Animator::threadRun - EXIT" << std::endl ;
					return ;
				}
			}

			// Updated based on any stop request
			if (mStop)
			{
				mStop = false ;
				mAnimate = false ;
			}

			// pass flag out
			animate = mAnimate ;

			// Prepare for next frame
			untilTime += std::chrono::milliseconds(mRateMs) ;

			// Notify we're doing a frame
			mCondTick.notify_all() ;
		}

		// skip rest if not animating
		if (!animate)
			continue ;


//		debugVerbose << TimeUtils::timestamp() << " Animator::threadRun - animate" << std::endl ;

		// Calculate next frame
		animate = mNewFrame() ;

		// Get frame data
		std::vector<uint8_t> data ;
		if (animate)
			data = mGetData() ;

		// Send to gadget
		if (animate)
			animate = mPaint(data) ;

		// disable animation if necessary
		if (!animate)
		{
			std::unique_lock<std::mutex> lock(mMutex) ;
			mAnimate = false ;
		}
	}

	debugVerbose << "Animator::threadRun - END" << std::endl ;
}

