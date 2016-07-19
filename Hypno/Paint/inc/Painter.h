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
 * \file      Painter.h
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef CUBEEMU_INC_PAINTER_H_
#define CUBEEMU_INC_PAINTER_H_

#include <mutex>

#include "IPainter.h"

namespace HypnoQuartz {

/*!
 * \class 	Painter
 * \brief   Generic IPainter class
 * \details Provides the base methods for all IPainter classes
 */
class Painter : public virtual IPainter {
public:
	Painter() ;
	virtual ~Painter() ;

	/**
	 * Sets the model to be used for repainting. Any existing model is deleted
	 * @param newModel
	 */
	virtual void setModel(std::shared_ptr<Model> newModel) override ;

	/**
	 * Get the current model setting
	 * @return current model; or null pointer
	 */
	virtual std::shared_ptr<Model> getModel() const override ;

	/**
	 * Called whenever the gadget model changes and the visualisation must be re-rendered
	 * @param newModel	Updated model
	 */
	virtual void paint(std::shared_ptr<Model> newModel) override ;

protected:
	/**
	 * Virtual method which must be overridden by derived classes. Called by paint() method once model
	 * has been copied. During the repaint, the mutex will be locked to prevent model changes during painting
	 */
	virtual void doPaint() =0 ;

private:
	std::shared_ptr<Model> mModel ;
	std::mutex mMutex ;
} ;

}

#endif /* CUBEEMU_INC_PAINTER_H_ */
