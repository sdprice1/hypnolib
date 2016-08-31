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
 * \file      GadgetNoLock.hpp
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_GADGET_INC_GADGETNOLOCK_HPP_
#define LIBHYPNO_GADGET_INC_GADGETNOLOCK_HPP_

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "hypno/Gadget.h"  // the gadget interface
#include "hypno/CommsClient.h"
#include "hypno/Debuggable.h"

namespace HypnoQuartz {

/*!
 * \class 	GadgetNoLock
 * \brief   GadgetLock which does nothing
 */
class GadgetNoLock : public HypnoGadget::GadgetLock
{
public:
	GadgetNoLock() {}
	virtual ~GadgetNoLock() {}

	void Lock(void) {};	   // do nothing
	void Unlock(void) {};  // do nothing
} ;

}

#endif /* LIBHYPNO_GADGET_INC_GADGETNOLOCK_HPP_ */
