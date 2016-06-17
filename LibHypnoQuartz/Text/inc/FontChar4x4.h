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
 * \file      FontChar4x4.h
 * \brief     A 4 pixel by 4 pixel font character
 *
 * \date      9 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef LIBHYPNOQUARTZ_TEXT_INC_FONTCHAR4X4_H_
#define LIBHYPNOQUARTZ_TEXT_INC_FONTCHAR4X4_H_

#include "FontChar.h"

namespace HypnoQuartz {

/*!
 * \class FontChar4x4
 */
class FontChar4x4 : public FontChar {
public:
	FontChar4x4(char cc, std::vector<std::string> data) ;
	virtual ~FontChar4x4() ;

} ;

}

#endif /* LIBHYPNOQUARTZ_TEXT_INC_FONTCHAR4X4_H_ */
