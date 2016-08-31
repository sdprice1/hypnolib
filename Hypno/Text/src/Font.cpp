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
 * \file      Font.cpp
 * \brief     
 *
 * \date      10 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "hypno/Font.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Font::Font() :
	mWidth(0),
	mHeight(0)
{
}

//-------------------------------------------------------------------------------------------------------------
Font::Font(unsigned width, unsigned height) :
	mWidth(width),
	mHeight(height)
{
}


//-------------------------------------------------------------------------------------------------------------
Font::~Font()
{
}

//-------------------------------------------------------------------------------------------------------------
unsigned Font::getWidth() const
{
	return mWidth ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Font::getHeight() const
{
	return mHeight ;
}

//-------------------------------------------------------------------------------------------------------------
bool Font::isChar(char cc) const
{
	return getChar(cc).get() != nullptr ;
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<FontChar> Font::getChar(char cc) const
{
	auto entry(mFont.find(cc)) ;
	if (entry == mFont.end())
		return std::shared_ptr<FontChar>() ;

	return entry->second ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Font::addFontChar(std::shared_ptr<FontChar> fc)
{
	if (!fc)
		return ;

	// Check char dimensions match
	if (mWidth && mHeight)
	{
		if (mWidth != fc->getWidth())
			throw std::range_error("Font defined with width=" + std::to_string(mWidth) +
				" adding FontChar of width=" + std::to_string(fc->getWidth())
			) ;
		if (mHeight != fc->getHeight())
			throw std::range_error("Font defined with height=" + std::to_string(mHeight) +
				" adding FontChar of height=" + std::to_string(fc->getHeight())
			) ;
	}
	else
	{
		mWidth = fc->getWidth() ;
		mHeight = fc->getHeight() ;
	}

	mFont[fc->getChar()] = fc ;
}
