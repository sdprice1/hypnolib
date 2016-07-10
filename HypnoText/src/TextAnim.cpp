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
 * \file      TextAnim.cpp
 * \brief     
 *
 * \date      23 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

#include "Font4x4.h"
#include "CubeModel.h"

#include "TextAnim.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
TextAnim::TextAnim(std::shared_ptr<GadgetQuartz> gadget, const std::string& text) :
	AnimCallback(gadget, std::make_shared<CubeModel>()),
	mText(text),
	mFg(Colour::BLUE),
	mBg(Colour::BLACK),
	mCharRepeat(0),

	mPos(0),
	mCharIndex(0)
{
	model()->fill(mBg) ;
}

//-------------------------------------------------------------------------------------------------------------
TextAnim::~TextAnim()
{
}

//-------------------------------------------------------------------------------------------------------------
bool TextAnim::newFrame()
{
	// shift all pixels forward by 1
	model()->transpose(Coord(0, 0, 1), mBg) ;

	// Display char
	unsigned numShow(mCharRepeat+1) ;
	if ( (mPos / numShow) == 0)
	{
		std::shared_ptr<Font> font(Font4x4::getInstance()) ;
		char cc(mText[mCharIndex]) ;
		std::shared_ptr<FontChar> fc(font->getChar(cc)) ;
		if (fc)
		{
			FontChar::const_iterator it(fc->begin()) ;
			for (unsigned y=0; y <= 3; ++y)
			{
				for (unsigned x=0; x <= 3; ++x)
				{
					if (*it++)
					{
						// paint character at the back of the cube
						model()->setPixel( Coord(x, y, 0), mFg) ;
					}
				}
			}
		}
	}

	// update indices
	if (++mPos > 3)
	{
		mPos = 0 ;
		if (++mCharIndex >= mText.size())
			mCharIndex = 0 ;
	}

	return true ;
}

