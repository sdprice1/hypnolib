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
 * \file      Animator.h
 * \brief     Animate gadget frames
 *
 * \date      22 Jun 2016
 * \author    sdprice1
 *
 * \details   Creates a thread which runs at a fixed rate causing a new frame of pixels to be created and then
 * 			painting them to the gadget.
 *
 */


#ifndef LIBHYPNOQUARTZ_PAINT_INC_ANIMATOR_H_
#define LIBHYPNOQUARTZ_PAINT_INC_ANIMATOR_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "IAnimatorCallback.h"

namespace HypnoQuartz {


/*!
 * \class Animator
 */
class Animator {
public:
	Animator() ;
	virtual ~Animator() ;

	/**
	 * New frame callback. If the function returns false then the animation is stopped
	 */
	using NewFrameCallback = std::function<bool()> ;

	/**
	 * Get data callback. Returns the vector of new data ready to send to gadget
	 */
	using GetDataCallback = std::function<std::vector<uint8_t>()> ;

	/**
	 * Paint callback. Send the new data to the gadget. If the function returns false then the animation is stopped
	 */
	using PaintCallback = std::function<bool(const std::vector<uint8_t>&)> ;

	/**
	 * Change the frame rate (in milliseconds)
	 */
	void setFrameRate(unsigned rateMs) ;

	/**
	 * Set required callbacks and start animation. Returns true if started ok
	 */
	bool start(NewFrameCallback newFrame, GetDataCallback getData, PaintCallback paint) ;

	/**
	 * Start with a callback object. Returns true if started ok
	 *
	 * NOTE: Uses a shared pointer to ensure the object stays in scope even if the caller gos out of scope
	 */
	bool start(std::shared_ptr<IAnimatorCallback> callbackObj) ;

	/**
	 * Restart animation using existing callbacks. If none are set then will fail. Returns true if started ok
	 */
	bool start() ;

	/**
	 * Stop animation
	 */
	void stop() ;

	/**
	 * Is animation currently running
	 */
	bool isRunning() const ;

private:
	// Thread handler
	void threadRun() ;

private:
	unsigned mRateMs ;
	NewFrameCallback mNewFrame ;
	GetDataCallback mGetData ;
	PaintCallback mPaint ;

	bool mRun ;
	bool mStop ;
	bool mAnimate ;
	std::thread mThread ;
	mutable std::mutex mMutex ;
	std::condition_variable mCondEvent ;
	std::condition_variable mCondTick ;

} ;


}

#endif /* LIBHYPNOQUARTZ_PAINT_INC_ANIMATOR_H_ */
