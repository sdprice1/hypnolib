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
 * \file      AsciiColour.h
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_MODEL_INC_ASCIICOLOUR_H_
#define LIBHYPNO_MODEL_INC_ASCIICOLOUR_H_

#include "hypno/Colour.h"

namespace HypnoQuartz {

/*!
 * \class AsciiColour
 * \brief     Convert Colour into ASCII colour
 * \details   Quatises the colour values into simple RGB/CMY/W and uses space as black
 */
class AsciiColour : Colour {
public:
	/**
	 * Default constructor (black)
	 */
	AsciiColour() ;

	/**
	 * Create from a Colour
	 */
	explicit AsciiColour(const Colour& colour) ;

	virtual ~AsciiColour() ;

	/**
	 * Get the ASCII char that represents this colour
	 * @return a colour character
	 */
	char getChar() const ;

} ;

}

#endif /* LIBHYPNO_MODEL_INC_ASCIICOLOUR_H_ */
