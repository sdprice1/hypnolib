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
 * \file      Font4x4.h
 *
 * \date      10 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_TEXT_INC_FONT4X4_H_
#define LIBHYPNO_TEXT_INC_FONT4X4_H_

#include "Font.h"

namespace HypnoQuartz {

/*!
 * \class Font4x4
 * \brief     A collection of 4x4 chars
 */
class Font4x4 : public Font {
public:
	virtual ~Font4x4() ;

	/**
	 * Singleton
	 */
	static std::shared_ptr<Font4x4> getInstance() ;

protected:
	Font4x4() ;

private:
	void defineUppercase() ;
	void defineLowercase() ;
	void defineNumerals() ;
	void defineSpecial() ;

} ;

}

#endif /* LIBHYPNO_TEXT_INC_FONT4X4_H_ */
