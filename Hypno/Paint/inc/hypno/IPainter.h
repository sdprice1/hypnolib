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
 * \file      IPainter.h
 *
 * \date      15 Jul 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_PAINT_INC_IPAINTER_H_
#define LIBHYPNO_PAINT_INC_IPAINTER_H_

#include <memory>

#include "hypno/Model.h"

namespace HypnoQuartz {

/*!
 * \class 	IPainter
 * \brief   Virtual interface to gadget paint
 * \details Defines the methods to be called to visualise the current gadget state (Model)
 */
class IPainter {
public:
	IPainter() {}
	virtual ~IPainter() {}

	/**
	 * Sets the model to be used for repainting. Any existing model is deleted
	 * @param newModel
	 */
	virtual void setModel(std::shared_ptr<Model> newModel) =0 ;

	/**
	 * Get the current model setting
	 * @return current model; or null pointer
	 */
	virtual std::shared_ptr<Model> getModel() const =0 ;

	/**
	 * Called whenever the gadget model changes and the visualisation must be re-rendered
	 * @param newModel	Updated model
	 */
	virtual void paint(std::shared_ptr<Model> newModel) =0 ;

} ;

}

#endif /* LIBHYPNO_PAINT_INC_IPAINTER_H_ */
