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
 * \file      Font.h
 * \brief     Font consists of a set of FontChar's
 *
 * \date      10 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef LIBHYPNOQUARTZ_TEXT_INC_FONT_H_
#define LIBHYPNOQUARTZ_TEXT_INC_FONT_H_

#include <map>
#include <memory>

#include "FontChar.h"

namespace HypnoQuartz {

/*!
 * \class Font
 */
class Font {
public:
	Font(unsigned width, unsigned height) ;
	Font() ;
	virtual ~Font() ;

	/**
	 * Accessor for width setting
	 */
	unsigned getWidth() const ;

	/**
	 * Accessor for height setting
	 */
	unsigned getHeight() const ;

	/**
	 * Is this character valid in this font
	 */
	bool isChar(char cc) const ;

	/**
	 * Get a font character
	 */
	std::shared_ptr<FontChar> getChar(char cc) const ;

protected:
	/**
	 * Protected method used when constructing the map of font chars
	 */
	void addFontChar(std::shared_ptr<FontChar> fc) ;

private:
	unsigned mWidth ;
	unsigned mHeight ;
	std::map<char, std::shared_ptr<FontChar>> mFont ;

} ;

}

#endif /* LIBHYPNOQUARTZ_TEXT_INC_FONT_H_ */
