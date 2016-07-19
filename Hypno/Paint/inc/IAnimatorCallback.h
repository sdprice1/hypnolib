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
 * \file      IAnimatorCallback.h
 *
 * \date      23 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_PAINT_INC_IANIMATORCALLBACK_H_
#define LIBHYPNO_PAINT_INC_IANIMATORCALLBACK_H_

#include <cstdint>
#include <memory>
#include <vector>

namespace HypnoQuartz {

/*!
 * \class IAnimatorCallback
 * \brief     Virtual interface for animator callback class
 */
class IAnimatorCallback {
public:
	IAnimatorCallback() {}
	virtual ~IAnimatorCallback() {}


	/**
	 * New frame callback. If the function returns false then the animation is stopped
	 */
	virtual bool newFrame() =0 ;

	/**
	 * Get data callback. Returns the vector of new data ready to send to gadget
	 */
	virtual std::vector<uint8_t> getData() =0 ;

	/**
	 * Paint callback. Send the new data to the gadget. If the function returns false then the animation is stopped
	 */
	virtual bool paint(const std::vector<uint8_t>& data) =0 ;

};


}

#endif /* LIBHYPNO_PAINT_INC_IANIMATORCALLBACK_H_ */
