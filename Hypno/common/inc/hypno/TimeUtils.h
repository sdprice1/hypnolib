/*!
 * Copyright (c) 2016 Steve Price.
 *
 * This file is part of Hypnolib
 *
 * Hypnolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hypnolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hypnolib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      TimeUtils.h
 *
 * \date      8 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNOQUARTZ_INC_TIMEUTILS_H_
#define LIBHYPNOQUARTZ_INC_TIMEUTILS_H_

#include <string>

namespace HypnoQuartz {

/*!
 * \class TimeUtils
 * \brief     Time related utilities
 * \details
 */
class TimeUtils {
public:
	/**
	 * Sleep for the specified number of milliseconds
	 */
	static void msSleep(unsigned msecs) ;

	/**
	 * Return a timestamp string useful for debug
	 */
	static std::string timestamp() ;

} ;

}

#endif /* LIBHYPNOQUARTZ_INC_TIMEUTILS_H_ */