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
 * \file      AsciiPainter.cpp
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "AsciiPainter.h"

#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>

#include "AsciiColour.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// CLASSES
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
class AsciiPlane {
public:
	AsciiPlane(unsigned width, unsigned height) :
		mWidth(width),
		mHeight(height),
		mLines(mHeight, std::string(mWidth, ' '))
	{}
	~AsciiPlane() {}

	unsigned getHeight() const
	{
		// Allow for title lines
		return mHeight + 2 ;
	}

	void show(unsigned z, std::vector<std::string>& lines)
	{
		unsigned totalWidth(2 + mWidth + 1) ;

		std::stringstream ss ;
		ss << "z=" << z ;
		lines[0] += ss.str() + std::string(totalWidth-ss.str().size(), ' ') ;

		ss.str("") ;
		ss << "  " ;
		for (unsigned x=0; x < mWidth; ++x)
			ss << (x%10) ;
		lines[1] += ss.str() ;

		unsigned lineNum(2) ;
		unsigned y(mHeight-1) ;
		for (auto line : mLines)
		{
			ss.str("") ;
			ss << std::setw(2) << y ;
			ss << line ;
			lines[lineNum] += ss.str() ;
			--y ;
			++lineNum ;
		}
	}

	void set(unsigned z, std::shared_ptr<Model> model)
	{
		unsigned y(mHeight-1) ;
		for (auto& line : mLines)
		{
			for (unsigned x=0; x < mWidth; ++x)
			{
				Colour pixel(model->getPixel( Coord(x, y, z) )) ;
				AsciiColour ascii(pixel) ;
				line[x] = ascii.getChar() ;
			}
			--y ;
		}
	}

private:
	unsigned mWidth ;
	unsigned mHeight ;
	std::vector<std::string> mLines ;
};

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
AsciiPainter::AsciiPainter()
{
}

//-------------------------------------------------------------------------------------------------------------
AsciiPainter::~AsciiPainter()
{
}

//=============================================================================================================
// PROTECTED
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void AsciiPainter::doPaint()
{
	std::cerr << "!! PAINT !!" << std::endl ;

	std::shared_ptr<Model> model(getModel()) ;
	if (!model)
		return ;

	AsciiPlane plane(model->getWidth(), model->getHeight()) ;
	std::vector<std::string> lines(plane.getHeight(), "") ;

	for (unsigned z=0; z < model->getDepth(); ++z)
	{
		plane.set(z, model) ;
		plane.show(z, lines) ;
	}

	for (auto line : lines)
	{
		std::cout << line << std::endl ;
	}
	std::cout << "----" << std::endl ;
}
