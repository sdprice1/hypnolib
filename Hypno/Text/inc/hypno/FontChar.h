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
 * \file      FontChar.h
 *
 * \date      9 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_TEXT_INC_FONTCHAR_H_
#define LIBHYPNO_TEXT_INC_FONTCHAR_H_

#include <cstdint>
#include <vector>
#include <string>

namespace HypnoQuartz {

/*!
 * \class FontChar
 * \brief     Representation of a character
 * \details   Stores an XY plane of boolean pixels for an image that represents a character
 *
 * NOTE: This class would ideally use a vector of bool, but that isn't really a vector and causes problems with iterators
 * (see http://www.gotw.ca/publications/mill09.htm for the gory details)
 *
 */
class FontChar {
public:
    typedef uint8_t   value_type;
    typedef uint8_t   difference_type;
    typedef uint8_t*  pointer;
    typedef uint8_t&  reference;
    typedef const uint8_t*  const_pointer;
    typedef const uint8_t&  const_reference;


	/**
	 * Sets the height and width of the character in pixels. Data vector of strings is used to represent the pixel states.
	 *
	 * Each char in the string sets the pixel on or off: space ' ' is off; anything else is on.
	 * Font pixels are stored top to bottom, left to right. So the top left hand corner pixel of the font character is stored
	 * at data[0][0]
	 *
	 * Using this representation means that it's easy to "draw" the font characters direct into code
	 */
	FontChar(unsigned width, unsigned height, char cc, std::vector<std::string> data) ;
	virtual ~FontChar() ;

	/**
	 * Accessor for width setting
	 */
	virtual unsigned getWidth() const ;

	/**
	 * Accessor for height setting
	 */
	virtual unsigned getHeight() const ;

	/**
	 * Accessor for the character this represents
	 */
	virtual char getChar() const ;

	/**
	 * Pixel accessor
	 */
	virtual bool getPixel(unsigned x, unsigned y) const ;


// ITERATOR
public:
	class const_iterator
	{
	friend class FontChar ;
	public:
		typedef FontChar::difference_type difference_type;
		typedef FontChar::value_type value_type;
		typedef FontChar::const_reference const_reference;
		typedef FontChar::const_pointer const_pointer;
		typedef std::random_access_iterator_tag iterator_category;

		const_iterator ();
		const_iterator (const FontChar* fc, unsigned index);
		const_iterator (const const_iterator& rhs);
		~const_iterator();

		const_iterator& operator=(const const_iterator& rhs);
		bool operator==(const const_iterator& rhs) const;
		bool operator!=(const const_iterator& rhs) const;
//		bool operator<(const const_iterator& rhs) const; //optional
//		bool operator>(const const_iterator& rhs) const; //optional
//		bool operator<=(const const_iterator& rhs) const; //optional
//		bool operator>=(const const_iterator& rhs) const; //optional

		const_iterator& operator++();
		const_iterator operator++(int);
//		const_iterator& operator--(); //optional
//		const_iterator operator--(int delta); //optional
//		const_iterator& operator+=(std::size_type delta); //optional
//		const_iterator operator+(std::size_type delta) const; //optional
//		const_iterator& operator-=(std::size_type delta); //optional
//		const_iterator operator-(std::size_type delta) const; //optional
//		difference_type operator-(const_iterator delta) const; //optional

		const_reference operator*() const;
		const_pointer operator->() const;
//		const_reference operator[](std::size_type index) const; //optional

	private:
		const FontChar* mFc ;
		unsigned mIndex ;
		unsigned mMaxIndex ;
	};


// FontChar iterate
public:
	const_iterator begin() const ;
	const_iterator end() const ;
	const_iterator cbegin() const ;
	const_iterator cend() const ;

protected:
	const uint8_t& operator [](unsigned index) const ;
	unsigned toIndex(unsigned x, unsigned y) const ;

private:
	unsigned mWidth ;
	unsigned mHeight ;
	unsigned mMaxIndex ;
	char mCc ;
	std::vector<uint8_t> mPixels ;
} ;

}

#endif /* LIBHYPNO_TEXT_INC_FONTCHAR_H_ */
