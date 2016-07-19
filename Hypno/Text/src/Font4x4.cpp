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
 * \file      Font4x4.cpp
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

#include "Font4x4.h"

#include "FontChar4x4.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// MACROS
//=============================================================================================================
#define MKFONT4x4(cc, line1, line2, line3, line4) \
	addFontChar(std::shared_ptr<FontChar4x4>(new FontChar4x4(cc, {\
		line1,\
		line2,\
		line3,\
		line4,\
	})))

//=============================================================================================================
// SINGLETON
//=============================================================================================================
namespace HypnoQuartz {
class Font4x4Instance : public Font4x4 {
public:
	Font4x4Instance() : Font4x4() {}
};
}
std::shared_ptr<Font4x4> Font4x4::getInstance()
{
	static std::shared_ptr<Font4x4> instance(std::make_shared<Font4x4Instance>()) ;
	return instance ;
}


//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
Font4x4::Font4x4() :
	Font(4, 4)
{
	defineUppercase() ;
	defineLowercase() ;
	defineNumerals() ;
	defineSpecial() ;

}

//-------------------------------------------------------------------------------------------------------------
Font4x4::~Font4x4()
{
}

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void Font4x4::defineUppercase()
{
	// Create the font set
	MKFONT4x4('A',		" XX ",
						"X  X",
						"XXXX",
						"X  X"
	) ;

	MKFONT4x4('B',		"XXX ",
						"X XX",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('C',		"XXXX",
						"X   ",
						"X   ",
						"XXXX"
	) ;

	MKFONT4x4('D',		"XXX ",
						"X  X",
						"X  X",
						"XXX "
	) ;

	MKFONT4x4('E',		"XXXX",
						"XXX ",
						"X   ",
						"XXXX"
	) ;

	MKFONT4x4('F',		"XXXX",
						"X   ",
						"XXX ",
						"X   "
	) ;

	MKFONT4x4('G',		"XXXX",
						"X   ",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('H',		"X  X",
						"XXXX",
						"X  X",
						"X  X"
	) ;

	MKFONT4x4('I',		"XXXX",
						" X  ",
						" X  ",
						"XXXX"
	) ;

	MKFONT4x4('J',		"XXXX",
						"   X",
						"   X",
						"XXX "
	) ;

	MKFONT4x4('K',		"X XX",
						"XX  ",
						"XXX ",
						"X  X"
	) ;

	MKFONT4x4('L',		"X   ",
						"X   ",
						"X   ",
						"XXXX"
	) ;

	MKFONT4x4('M',		"XXXX",
						"XXXX",
						"X XX",
						"X  X"
	) ;

	MKFONT4x4('N',		"X  X",
						"XX X",
						"X XX",
						"X  X"
	) ;

	MKFONT4x4('O',		"XXXX",
						"X  X",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('P',		"XXXX",
						"X  X",
						"XXXX",
						"X   "
	) ;

	MKFONT4x4('Q',		"XXXX",
						"X  X",
						"X XX",
						"XXXX"
	) ;

	MKFONT4x4('R',		"XXX ",
						"X  X",
						"XXX ",
						"X  X"
	) ;

	MKFONT4x4('S',		"XXXX",
						"XX  ",
						"  XX",
						"XXXX"
	) ;

	MKFONT4x4('T',		"XXXX",
						" X  ",
						" X  ",
						" X  "
	) ;

	MKFONT4x4('U',		"X  X",
						"X  X",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('V',		"X  X",
						"X  X",
						"X  X",
						" XX "
	) ;

	MKFONT4x4('W',		"X  X",
						"X XX",
						"XXXX",
						"XXXX"
	) ;

	MKFONT4x4('X',		"X  X",
						" XX ",
						" XX ",
						"X  X"
	) ;

	MKFONT4x4('Y',		"X  X",
						"X  X",
						" X  ",
						" X  "
	) ;

	MKFONT4x4('Z',		"XXXX",
						"  XX",
						"XX  ",
						"XXXX"
	) ;
}

//-------------------------------------------------------------------------------------------------------------
void Font4x4::defineLowercase()
{
	// Create the font set
	MKFONT4x4('a',		" XX ",
						"X  X",
						"X XX",
						"XX X"
	) ;

	MKFONT4x4('c',		"X   ",
						"XXX ",
						"X  X",
						"XXX "
	) ;

	MKFONT4x4('c',		" XXX",
						"X   ",
						"X   ",
						" XXX"
	) ;

	MKFONT4x4('d',		"   X",
						" XXX",
						"X  X",
						" XXX"
	) ;

	MKFONT4x4('e',		" XX ",
						"X XX",
						"XXX ",
						" XXX"
	) ;

	MKFONT4x4('f',		" XXX",
						"X   ",
						"XXX ",
						"X   "
	) ;

	MKFONT4x4('g',		" XXX",
						"X  X",
						" XXX",
						"XXX "
	) ;

	MKFONT4x4('h',		"X   ",
						"X   ",
						"XXX ",
						"X  X"
	) ;

	MKFONT4x4('i',		" X  ",
						"    ",
						" X  ",
						" XX "
	) ;

	MKFONT4x4('j',		"   X",
						"    ",
						"   X",
						"XXXX"
	) ;

	MKFONT4x4('k',		"X   ",
						"X  X",
						"XXX ",
						"X  X"
	) ;

	MKFONT4x4('l',		" XX ",
						"  X ",
						"  X ",
						"  X "
	) ;

	MKFONT4x4('m',		" XXX",
						"X XX",
						"X  X",
						"X  X"
	) ;

	MKFONT4x4('n',		"XXX ",
						"X  X",
						"X  X",
						"X  X"
	) ;

	MKFONT4x4('o',		" XX ",
						"X  X",
						"X  X",
						" XX "
	) ;

	MKFONT4x4('p',		"XXX ",
						"X  X",
						"XXX ",
						"X   "
	) ;

	MKFONT4x4('q',		" XX ",
						"X X ",
						" XX ",
						"  XX"
	) ;

	MKFONT4x4('r',		" XX ",
						"X  X",
						"X   ",
						"X   "
	) ;

	MKFONT4x4('s',		"  XX",
						" X  ",
						"  X ",
						"XX  "
	) ;

	MKFONT4x4('t',		" X  ",
						"XXX ",
						" X  ",
						" XX "
	) ;

	MKFONT4x4('u',		"X  X",
						"X  X",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('v',		"X  X",
						"X  X",
						"X  X",
						" XX "
	) ;

	MKFONT4x4('w',		"X  X",
						"X  X",
						"X XX",
						"XXXX"
	) ;

	MKFONT4x4('x',		"X  X",
						"X  X",
						" XX ",
						"X  X"
	) ;

	MKFONT4x4('y',		"X  X",
						"X  X",
						" X  ",
						" X  "
	) ;

	MKFONT4x4('z',		"XXXX",
						"  X ",
						" X  ",
						"XXXX"
	) ;
}

//-------------------------------------------------------------------------------------------------------------
void Font4x4::defineSpecial()
{
	MKFONT4x4(' ',		"    ",
						"    ",
						"    ",
						"    "
	) ;

	MKFONT4x4('!',		" XX ",
						" XX ",
						"    ",
						" XX "
	) ;

	MKFONT4x4('.',		"    ",
						"    ",
						"    ",
						" XX "
	) ;

	MKFONT4x4('@',		" XXX",
						"X XX",
						"X   ",
						" XX "
	) ;

	MKFONT4x4(',',		"    ",
						"    ",
						"  X ",
						" XX "
	) ;

	MKFONT4x4('[',		" XX ",
						" X  ",
						" X  ",
						" XX "
	) ;

	MKFONT4x4(']',		" XX ",
						"  X ",
						"  X ",
						" XX "
	) ;

	MKFONT4x4('(',		"  X ",
						" X  ",
						" X  ",
						"  X "
	) ;

	MKFONT4x4(')',		" X  ",
						"  X ",
						"  X ",
						" X  "
	) ;



	MKFONT4x4('^',		" X  ",
						"X X ",
						"    ",
						"    "
	) ;

	MKFONT4x4('"',		"X X ",
						"X X ",
						"    ",
						"    "
	) ;

	MKFONT4x4('?',		"XXX ",
						"X X ",
						"    ",
						" XX "
	) ;

	MKFONT4x4(';',		" X  ",
						"    ",
						" X  ",
						"XX  "
	) ;

	MKFONT4x4(':',		" X  ",
						"    ",
						" X  ",
						"    "
	) ;

	MKFONT4x4('~',		"    ",
						" X X",
						"X X ",
						"    "
	) ;

	MKFONT4x4('&',		"XXX ",
						"X X ",
						" XXX",
						"X X "
	) ;

	MKFONT4x4('+',		" XX ",
						"XXXX",
						" XX ",
						"    "
	) ;

	MKFONT4x4('-',		"    ",
						" XX ",
						"    ",
						"    "
	) ;

	MKFONT4x4('_',		"    ",
						"    ",
						"    ",
						"XXXX"
	) ;

	MKFONT4x4('|',		" X  ",
						" X  ",
						" X  ",
						" X  "
	) ;

	MKFONT4x4('\\',		"X   ",
						" X  ",
						"  X ",
						"   X"
	) ;

	MKFONT4x4('/',		"   X",
						"  X ",
						" X  ",
						"X   "
	) ;

	MKFONT4x4('=',		"    ",
						"XXXX",
						"    ",
						"XXXX"
	) ;

	MKFONT4x4('*',		"X  X",
						"  X ",
						" X  ",
						"X  X"
	) ;


}



//-------------------------------------------------------------------------------------------------------------
void Font4x4::defineNumerals()
{
	MKFONT4x4('0',		"XXXX",
						"XX X",
						"X XX",
						"XXXX"
	) ;

	MKFONT4x4('1',		"XXX ",
						" XX ",
						" XX ",
						"XXXX"
	) ;

	MKFONT4x4('2',		"XXXX",
						" XXX",
						"XXX ",
						"XXXX"
	) ;

	MKFONT4x4('3',		"XXXX",
						" XXX",
						"  XX",
						"XXXX"
	) ;

	MKFONT4x4('4',		"X XX",
						"XXXX",
						"XXXX",
						"  XX"
	) ;

	MKFONT4x4('5',		"XXXX",
						"XXX ",
						" XXX",
						"XXXX"
	) ;

	MKFONT4x4('6',		"X   ",
						"XXXX",
						"X  X",
						"XXXX"
	) ;

	MKFONT4x4('7',		"XXXX",
						"  XX",
						"  XX",
						"  XX"
	) ;

	MKFONT4x4('8',		" XXX",
						" X X",
						"XXXX",
						"XXXX"
	) ;



	MKFONT4x4('9',		"XXXX",
						"X XX",
						"XXXX",
						"  XX"
	) ;


}
