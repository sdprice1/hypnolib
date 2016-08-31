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
 * \file      FontChar.cpp
 * \brief     
 *
 * \date      9 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "hypno/FontChar.h"

#include <stdexcept>
#include <iostream>


using namespace HypnoQuartz ;

//=============================================================================================================
// CONSTANTS
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
static const uint8_t null(0) ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
FontChar::FontChar(unsigned width, unsigned height, char cc, std::vector<std::string> data) :
	mWidth(width),
	mHeight(height),
	mMaxIndex(0),
	mCc(cc),
	mPixels(mHeight * mWidth, false)
{
	// Bounds checks
	if (data.size() != mHeight)
		throw std::range_error("FontChar creation with height=" + std::to_string(mHeight) +
				" but with " + std::to_string(data.size()) + " lines of data") ;
	for (auto line : data)
	{
		if (line.size() != mWidth)
			throw std::range_error("FontChar creation with width=" + std::to_string(mWidth) +
					" but with " + std::to_string(line.size()) + " characters") ;
	}

	// Process the data
	unsigned y(mHeight-1) ;
	for (auto line : data)
	{
		unsigned x(0) ;
		for (auto cc : line)
		{
			if (cc != ' ')
				mPixels[toIndex(x, y)] = true ;

			++x ;
		}
		--y ;
	}

	// Finally ok to set the limits
	mMaxIndex = mWidth * mHeight - 1 ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::~FontChar()
{
}

//-------------------------------------------------------------------------------------------------------------
unsigned FontChar::getWidth() const
{
	return mWidth ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned FontChar::getHeight() const
{
	return mHeight ;
}

//-------------------------------------------------------------------------------------------------------------
char FontChar::getChar() const
{
	return mCc ;
}

//-------------------------------------------------------------------------------------------------------------
bool FontChar::getPixel(unsigned x, unsigned y) const
{
	unsigned index(toIndex(x, y)) ;
	if (index > mMaxIndex)
		return false ;

	return (mPixels[index] == 0) ;
}


//=============================================================================================================
// PUBLIC ITERATOR METHODS
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator FontChar::begin() const
{
	return const_iterator(this, 0) ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator FontChar::end() const
{
	return const_iterator(this, mMaxIndex+1) ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator FontChar::cbegin() const
{
	return begin() ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator FontChar::cend() const
{
	return end() ;
}

//=============================================================================================================
// PROTECTED ITERATOR METHODS
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
const uint8_t& FontChar::operator [](unsigned index) const
{
	if (index > mMaxIndex)
		return null ;

	return mPixels[index] ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned FontChar::toIndex(unsigned x, unsigned y) const
{
	return(y * mWidth + x) ;
}

//=============================================================================================================
// FontChar const_iterator
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator::const_iterator() :
	mFc(nullptr),
	mIndex(0),
	mMaxIndex(0)
{
//	std::cerr << "FontChar::const_iterator::const_iterator()" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator::const_iterator(const FontChar* fc, unsigned index) :
	mFc(fc),
	mIndex(index),
	mMaxIndex(fc->getHeight() * fc->getWidth() - 1)
{
//	std::cerr << "FontChar::const_iterator::const_iterator(" << index << ")" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator::const_iterator(const const_iterator& rhs) :
	mFc(rhs.mFc),
	mIndex(rhs.mIndex),
	mMaxIndex(rhs.mMaxIndex)
{
//	std::cerr << "FontChar::const_iterator::const_iterator(rhs)" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator::~const_iterator()
{
//	std::cerr << "FontChar::const_iterator::~const_iterator()" << std::endl ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator& FontChar::const_iterator::operator =(const const_iterator& rhs)
{
//	std::cerr << "FontChar::const_iterator::operator =(rhs)" << std::endl ;
	mFc = rhs.mFc ;
	mIndex = rhs.mIndex ;
	mMaxIndex = rhs.mMaxIndex ;
	return *this ;
}

//-------------------------------------------------------------------------------------------------------------
bool FontChar::const_iterator::operator ==(const const_iterator& rhs) const
{
//	std::cerr << "FontChar::const_iterator::operator ==()" << std::endl ;
	return (mFc == rhs.mFc) && (mIndex == rhs.mIndex) ;
}

//-------------------------------------------------------------------------------------------------------------
bool FontChar::const_iterator::operator !=(const const_iterator& rhs) const
{
//	std::cerr << "FontChar::const_iterator::operator !=()" << std::endl ;
	return ! operator==(rhs) ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator& FontChar::const_iterator::operator ++()
{
//	std::cerr << "FontChar::const_iterator::operator ++()" << std::endl ;
	if (mIndex <= mMaxIndex)
		++mIndex ;

	return *this ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_reference FontChar::const_iterator::operator *() const
{
//	std::cerr << "FontChar::const_iterator::operator *()" << std::endl ;

	if (!mFc)
		return null ;

	if (mIndex > mMaxIndex)
		return null ;

	return (*mFc)[mIndex] ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_iterator FontChar::const_iterator::operator ++(int)
{
	FontChar::const_iterator current(*this) ;
	operator++() ;
	return current ;
}

//-------------------------------------------------------------------------------------------------------------
FontChar::const_pointer FontChar::const_iterator::operator ->() const
{
//	std::cerr << "FontChar::const_iterator::operator ->()" << std::endl ;

	if (!mFc)
		return &null ;

	if (mIndex > mMaxIndex)
		return &null ;

	return &(*mFc)[mIndex] ;
}

////-------------------------------------------------------------------------------------------------------------
//bool FontChar::const_iterator::operator bool() const
//{
////TODO
//	return false ;
//}

