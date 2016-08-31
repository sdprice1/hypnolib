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
 * \file      CommsDebug.h
 * \brief     Debug Comms classes - only used for developer debug
 *
 * \date      31 Aug 2016
 * \author    sdprice1
 *
 */


#ifndef HYPNO_COMMS_INC_HYPNO_COMMSDEBUG_H_
#define HYPNO_COMMS_INC_HYPNO_COMMSDEBUG_H_

#include <iostream>

// Uncomment this line to enable verbose debug
//#define DEBUG_COMMS

#ifdef DEBUG_COMMS
#define DEBUG_COMMS_COUT	std::cout
#else
class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) { return c; }
};
static NullBuffer null_buffer;
static std::ostream null_stream(&null_buffer);
#define DEBUG_COMMS_COUT	null_stream
#endif


#endif /* HYPNO_COMMS_INC_HYPNO_COMMSDEBUG_H_ */
