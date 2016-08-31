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
 * \file      CommsException.h
 *
 * \date      31 Aug 2016
 * \author    sdprice1
 *
 */


#ifndef HYPNO_COMMS_INC_HYPNO_COMMSEXCEPTION_H_
#define HYPNO_COMMS_INC_HYPNO_COMMSEXCEPTION_H_

#include <exception>
#include <string>

namespace HypnoQuartz {

/*!
 * \class 	CommsException
 * \brief   Exception for Comms
 * \details Throws exception specifically for Comms
 */
class CommsException : public std::exception {
public:
	explicit CommsException(const std::string& reason) :
		mReason(reason)
	{}

	virtual ~CommsException() {}

	virtual const char* what() const throw()
	{
		return mReason.c_str() ;
	}

private:
	std::string mReason ;
} ;

}

#endif /* HYPNO_COMMS_INC_HYPNO_COMMSEXCEPTION_H_ */
