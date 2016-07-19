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
 * \file      DebugStream.h
 *
 * \date      29 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_DEBUG_INC_DEBUGSTREAM_H_
#define LIBHYPNO_DEBUG_INC_DEBUGSTREAM_H_

#include <iostream>
#include <ostream>

#include "TimeUtils.h"
#include "IDebug.h"

namespace HypnoQuartz {

/*!
 * \class DebugStream
 * \brief     Special std::ostream for debug
 * \details   An ostream that controls whether debug messages are output or not depending on the parent's current debug level
 */
class DebugStream : public std::ostream {
public:
	DebugStream(const IDebug* parent, const IDebug::DebugLevel& level) :
		mParent(parent),
		mLevel(level)
	{}
	virtual ~DebugStream() {}

	template<class T>
	std::ostream& operator<< (const T& val)
	{
		if (!mParent->isDebug(mLevel))
		{
			// This return effectively stops the rest of the processing of the << chain
			return *this ;
		}

		// Start of chain
		bool prefix(IDebug::optionsName() || IDebug::optionsTimestamp()) ;

		if (prefix)
		{
			std::string prefixStr ;
			if (IDebug::optionsTimestamp())
				prefixStr = TimeUtils::timestamp() ;

			if (IDebug::optionsName())
			{
				if (!prefixStr.empty()) prefixStr += " " ;
				prefixStr += mParent->getName() ;
			}

			std::cout << "[" << prefixStr << "] " ;
		}

		// output value
		std::cout << val ;

		// carry on sending rest of chain to cout
		return std::cout ;
	}

private:
	const IDebug* mParent ;
	const IDebug::DebugLevel mLevel ;
} ;

}


#endif /* LIBHYPNO_DEBUG_INC_DEBUGSTREAM_H_ */
