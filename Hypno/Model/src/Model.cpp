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
 * \file      Model.cpp
 * \brief     
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "hypno/Model.h"

#include <algorithm>
#include <iostream>

#include "hypno/IDebug.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Model::Model(unsigned width, unsigned height, unsigned depth) :
	mWidth(width),
	mHeight(height),
	mDepth(depth),
	mMaxIndex( (mWidth * mDepth * mDepth) -1),

	mPixels(mMaxIndex+1, Colour::BLACK),
	mToRawMap(),
	mFromRawMap(),
	mPixelMap()
{
}

//-------------------------------------------------------------------------------------------------------------
Model::~Model()
{
}

//-------------------------------------------------------------------------------------------------------------
Model::Model(const Model& other) :
	mWidth(other.mWidth),
	mHeight(other.mHeight),
	mDepth(other.mDepth),
	mMaxIndex(other.mMaxIndex),

	mPixels( other.mPixels.begin(), other.mPixels.end() ),
	mToRawMap(other.mToRawMap),
	mFromRawMap(other.mFromRawMap),
	mPixelMap(other.mPixelMap)
{
}

//-------------------------------------------------------------------------------------------------------------
std::shared_ptr<Model> Model::clone() const
{
	return std::shared_ptr<Model>(new Model(*this)) ;
}


//-------------------------------------------------------------------------------------------------------------
unsigned Model::getDepth() const
{
	return mDepth ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Model::getWidth() const
{
	return mWidth ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Model::getHeight() const
{
	return mHeight ;
}

//-------------------------------------------------------------------------------------------------------------
void Model::clear()
{
	fill(Colour::BLACK) ;
}

//-------------------------------------------------------------------------------------------------------------
void Model::fill(const Colour& colour)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	std::fill(mPixels.begin(), mPixels.end(), colour) ;
}

//-------------------------------------------------------------------------------------------------------------
void Model::transpose(const Coord& delta, const Colour& clearColour)
{
	std::vector<Colour> transposed(mMaxIndex+1, clearColour) ;
	std::unique_lock<std::mutex> lock(mMutex) ;

	// Transfer data
	for (unsigned index=0; index <= mMaxIndex; ++index)
	{
		Coord trans(fromIndex(index) + delta) ;
		if (!checkCoord(trans))
			continue ;

		transposed[toIndex(trans)] = mPixels[index] ;
	}


	using std::swap ;
	swap(mPixels, transposed) ;
}

//-------------------------------------------------------------------------------------------------------------
bool Model::fromPacked(const std::vector<uint8_t >& packedData)
{
//IDebug::dump("Model::fromPacked", packedData) ;

	// Need enough data
	unsigned packedSize = (mMaxIndex+1) * 3 / 2 ;
	if (packedData.size() != packedSize)
		return false ;

	// Start with cleared model
	clear() ;

	std::unique_lock<std::mutex> lock(mMutex) ;

	// Transfer data
	auto iter(packedData.begin()) ;
	for (unsigned index=0; index <= mMaxIndex && iter != packedData.end(); ++index)
	{
		uint16_t  packed(0) ;

		/* Data is packed as:
		//
		//   Pixel0      Pixel1
		// /----^----\ /---^----\
		// +--+--+ +--+--+ +--+--+
		// |R1:G1| |B1:R2| |G2:B2|
		// +--+--+ +--+--+ +--+--+
		*/
		switch (index % 2)
		{
		case 0:
			packed = (uint16_t )*iter++ << 4 ;
			packed |= (uint16_t )(*iter >> 4) & 0x00f ;
			break ;

		case 1:
		default:
			packed = (uint16_t )(*iter++ & 0x0f) << 8 ;
			packed |= (uint16_t )*iter++ ;
			break ;
		}

		unsigned mapIndex(index) ;
		if (mFromRawMap)
		{
			Coord mapped(mFromRawMap->translate(fromIndex(index)));
			mapIndex = toIndex(mapped) ;
		}
		mPixels[mapIndex] = Colour(packed) ;
	}

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::vector<uint8_t > Model::toPacked() const
{
	std::vector<uint8_t > packed ;
	packed.reserve( (mMaxIndex+1) * 3 / 2 ) ;

	std::unique_lock<std::mutex> lock(mMutex) ;

	// Need to work from the raw coords to internal coords
	uint8_t  byte(0) ;
	for (unsigned index=0; index <= mMaxIndex; ++index)
	{
		unsigned pixelIndex(index) ;
		if (mFromRawMap)
		{
			Coord mapped(fromIndex(index));
			Coord coord(mFromRawMap->translate(fromIndex(index)));
			pixelIndex = toIndex(coord) ;
		}
		Colour pixel(mPixels[pixelIndex]) ;

		/* Data is packed as:
		//
		//   Pixel0      Pixel1
		// /----^----\ /---^----\
		// +--+--+ +--+--+ +--+--+
		// |R1:G1| |B1:R2| |G2:B2|
		// +--+--+ +--+--+ +--+--+
		*/
		switch (index % 2)
		{
		case 0:
			byte = ((pixel.red() & 0xf) << 4) | (pixel.green() & 0xf) ;
			packed.push_back(byte) ;

			// store for next pixel
			byte = (pixel.blue() & 0xf) << 4 ;
			break ;

		case 1:
		default:
			// combine with prev pixel
			byte |= (pixel.red() & 0xf) ;
			packed.push_back(byte) ;

			byte = ((pixel.green() & 0xf) << 4) | (pixel.blue() & 0xf) ;
			packed.push_back(byte) ;
			break ;
		}

	}

	return packed ;
}

//-------------------------------------------------------------------------------------------------------------
bool Model::setPixel(const Coord& coord, const Colour& colour)
{
	std::unique_lock<std::mutex> lock(mMutex) ;

	Coord mapped(coord) ;
	if (mPixelMap)
		mapped = mPixelMap->translate(coord) ;

	if (!checkCoord(mapped))
		return false ;

	mPixels[toIndex(coord)] = colour ;

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
Colour Model::getPixel(const Coord& coord) const
{
	std::unique_lock<std::mutex> lock(mMutex) ;

	Coord mapped(coord) ;
	if (mPixelMap)
		mapped = mPixelMap->translate(coord) ;

	if (!checkCoord(mapped))
		return Colour::BLACK ;

	return mPixels.at(toIndex(coord)) ;
}

//-------------------------------------------------------------------------------------------------------------
void Model::setCoordMap(std::shared_ptr<CoordMap> coordMap)
{
	std::unique_lock<std::mutex> lock(mMutex) ;
	mPixelMap = coordMap ;
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Model::setToRawMap(std::shared_ptr<CoordMap> toRawMap)
{
	mToRawMap = toRawMap ;
}

//-------------------------------------------------------------------------------------------------------------
void Model::setFromRawMap(std::shared_ptr<CoordMap> fromRawMap)
{
	mFromRawMap = fromRawMap ;
}

//-------------------------------------------------------------------------------------------------------------
bool Model::checkCoord(const Coord& coord) const
{
	if (coord.x() < 0)
		return false ;

	if (coord.y() < 0)
		return false ;

	if (coord.z() < 0)
		return false ;

	if ( static_cast<unsigned>(coord.x()) >= mWidth)
		return false ;

	if ( static_cast<unsigned>(coord.y()) >= mHeight)
		return false ;

	if ( static_cast<unsigned>(coord.z()) >= mDepth)
		return false ;

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
Coord Model::fromIndex(unsigned index) const
{
	unsigned HxW = (mHeight * mWidth) ;
	unsigned z = index / HxW ;
	index -= z * HxW ;
	unsigned y = index / mWidth ;
	index -= y * mWidth ;

	return Coord(index, y, z) ;
}

//-------------------------------------------------------------------------------------------------------------
unsigned Model::toIndex(const Coord& coord) const
{
	return (coord.z() * mHeight * mWidth) + (coord.y() * mWidth) + coord.x() ;
}

