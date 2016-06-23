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
 * \file      Colour.h
 * \brief     Pixel (LED) colour setting
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   Specifies the RGB setting for each pixel as a triplet of 4-bit values
 *
 */


#ifndef LIBHYPNOQUARTZ_MODEL_INC_COLOUR_H_
#define LIBHYPNOQUARTZ_MODEL_INC_COLOUR_H_

#include <cstdint>
#include <random>

namespace HypnoQuartz {

/*!
 * \class Colour
 */
class Colour {
public:
	/**
	 * Default constructor (black)
	 */
	Colour() ;

	/**
	 * Specify r,g, and b values
	 */
	Colour(uint8_t  red, uint8_t  green, uint8_t  blue) ;

	/**
	 * Specify 12 bit value 3 nibbles in the order #rgb
	 */
	explicit Colour(uint16_t  rgb) ;

	virtual ~Colour() ;

	// Accessors
	uint8_t  red() const ;
	uint8_t  red(uint8_t  component) ;
	uint8_t  green() const ;
	uint8_t  green(uint8_t  component) ;
	uint8_t  blue() const ;
	uint8_t  blue(uint8_t  component) ;

	/**
	 * Predefined colours
	 */
	static const Colour BLACK ;
	static const Colour WHITE ;
	static const Colour RED ;
	static const Colour GREEN ;
	static const Colour BLUE ;
	static const Colour CYAN ;
	static const Colour MAGENTA ;
	static const Colour YELLOW ;
	static const Colour GREY ;

	/**
	 * Generate a random colour
	 */
	static const Colour rand() ;


private:
	static uint8_t  clamp(uint8_t  component) ;

private:
	static std::default_random_engine mRandGen ;
    static std::uniform_int_distribution<uint8_t> mRandDist ;

private:
	uint8_t  mRed ;
	uint8_t  mGreen ;
	uint8_t  mBlue ;
} ;

}

#endif /* LIBHYPNOQUARTZ_MODEL_INC_COLOUR_H_ */
