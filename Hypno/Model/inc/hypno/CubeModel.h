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
 * \file      CubeModel.h
 *
 * \date      21 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_MODEL_INC_CUBEMODEL_H_
#define LIBHYPNO_MODEL_INC_CUBEMODEL_H_

#include "hypno/Model.h"

namespace HypnoQuartz {

/*!
 * \class CubeModel
 * \brief     Model of 4x4x4 cube
 */
class CubeModel : public Model {
public:
	CubeModel() ;
	virtual ~CubeModel() ;

	/**
	 * Create a clone of this object
	 * @return shared pointer to new copy
	 */
	virtual std::shared_ptr<Model> clone() const override ;

} ;

}

#endif /* LIBHYPNO_MODEL_INC_CUBEMODEL_H_ */
