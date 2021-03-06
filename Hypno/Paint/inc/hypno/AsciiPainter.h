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
 * \file      AsciiPainter.h
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef CUBEEMU_INC_ASCIIPAINTER_H_
#define CUBEEMU_INC_ASCIIPAINTER_H_

#include "hypno/Painter.h"

namespace HypnoQuartz {

/*!
 * \class 	AsciiPainter
 * \brief   Render gadget as ascii
 * \details Displays the gadget model using ascii text
 */
class AsciiPainter : public Painter {
public:
	AsciiPainter() ;
	virtual ~AsciiPainter() ;

protected:
	/**
	 * Virtual method which must be overridden by derived classes. Called by paint() method once model
	 * has been copied. During the repaint, the mutex will be locked to prevent model changes during painting
	 */
	virtual void doPaint() override ;

} ;

}

#endif /* CUBEEMU_INC_ASCIIPAINTER_H_ */
