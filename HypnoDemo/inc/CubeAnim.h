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
 * \file      CubeAnim.h
 * \brief     Demo animation for cube
 *
 * \date      23 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef HYPNODEMO_INC_CUBEANIM_H_
#define HYPNODEMO_INC_CUBEANIM_H_

#include <memory>

#include "AnimCallback.h"

namespace HypnoQuartz {

/*!
 * \class CubeAnim
 */
class CubeAnim : public AnimCallback {
public:
	CubeAnim(std::shared_ptr<GadgetQuartz> gadget) ;
	virtual ~CubeAnim() ;

	/**
	 * New frame callback. If the function returns false then the animation is stopped
	 */
	virtual bool newFrame() override ;

private:
	unsigned mPos ;
} ;

}

#endif /* HYPNODEMO_INC_CUBEANIM_H_ */
