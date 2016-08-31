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
 * \file      AnimCallback.h
 *
 * \date      23 Jun 2016
 * \author    sdprice1
 *
 *
 */


#ifndef LIBHYPNO_PAINT_INC_ANIMCALLBACK_H_
#define LIBHYPNO_PAINT_INC_ANIMCALLBACK_H_

#include "hypno/GadgetQuartz.h"
#include "hypno/Colour.h"
#include "hypno/Coord.h"
#include "hypno/Model.h"
#include "hypno/IAnimatorCallback.h"

namespace HypnoQuartz {

/*!
 * \class AnimCallback
 * \brief     Generic animator callback class
 * \details   Base class which defines the common actions of an animator callback object. Just derive from this class
 * 			and override the newFrame method (and pass in a specific model to the constructor)
 */
class AnimCallback : public virtual IAnimatorCallback {
public:
	AnimCallback(std::shared_ptr<GadgetQuartz> gadget, std::shared_ptr<Model> model) ;
	virtual ~AnimCallback() ;

	/**
	 * New frame callback. If the function returns false then the animation is stopped
	 */
	virtual bool newFrame() override ;

	/**
	 * Get data callback. Returns the vector of new data ready to send to gadget
	 */
	virtual std::vector<uint8_t> getData() override ;

	/**
	 * Paint callback. Send the new data to the gadget. If the function returns false then the animation is stopped
	 */
	virtual bool paint(const std::vector<uint8_t>& data) override ;

protected:
	// Accessor
	std::shared_ptr<Model> model() ;

private:
	std::shared_ptr<GadgetQuartz> mGadget ;
	std::shared_ptr<Model> mModel ;
} ;

}

#endif /* LIBHYPNO_PAINT_INC_ANIMCALLBACK_H_ */
