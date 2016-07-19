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
 * \file      Model.h
 *
 * \date      20 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_MODEL_INC_MODEL_H_
#define LIBHYPNO_MODEL_INC_MODEL_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>

#include "Colour.h"
#include "Coord.h"
#include "CoordMap.h"

namespace HypnoQuartz {

/*!
 * \class Model
 * \brief     Model the pixels/LEDs of the gadget
 * \details   This is a 2D or 3D model of the LED settings in the gadget
 */
class Model {
public:
	Model(unsigned width, unsigned height, unsigned depth) ;
	virtual ~Model() ;

	explicit Model(const Model& other) ;

	/**
	 * Get the model depth
	 */
	virtual unsigned getDepth() const ;

	/**
	 * Get the model width
	 */
	virtual unsigned getWidth() const ;

	/**
	 * Get the model height
	 */
	virtual unsigned getHeight() const ;

	/**
	 * Create a clone of this object
	 * @return shared pointer to new copy
	 */
	virtual std::shared_ptr<Model> clone() const ;

	/**
	 * Set all pixels to BLACK
	 */
	virtual void clear() ;

	/**
	 * Set all pixels to this colour
	 */
	virtual void fill(const Colour& colour) ;

	/**
	 * Transpose current pixels by the delta Coord shifting the model in xyz space. The clearColour is used to set
	 * any "empty" pixels left as a result of the shift
	 */
	virtual void transpose(const Coord& delta, const Colour& clearColour = Colour::BLACK) ;

	/**
	 * Set the pixel colours from the packed data. Data is packed with 3 nibbles per pixel. So it takes 3 bytes for 2
	 * pixels
	 */
	virtual bool fromPacked(const std::vector<uint8_t >& packedData) ;

	/**
	 * Convert the current model contents into packed data
	 */
	virtual std::vector<uint8_t > toPacked() const ;

	/**
	 * Set the pixel at the specified coords to the specified colour. Return true if all ok
	 *
	 * Note that the coords are first translated with any current map set up by setCoordMap()
	 */
	virtual bool setPixel(const Coord& coord, const Colour& colour) ;

	/**
	 * Get the pixel at the specified coords.
	 *
	 * Note that the coords are first translated with any current map set up by setCoordMap()
	 */
	virtual Colour getPixel(const Coord& coord) const ;

	/**
	 * Specify a coordinate translation map to use while setting the pixels. Cleared by setting the map to a null pointer
	 */
	virtual void setCoordMap(std::shared_ptr<CoordMap> coordMap) ;

protected:
	// Set the coord translation scheme to map from internal coords to raw gadget coords
	void setToRawMap(std::shared_ptr<CoordMap> toRawMap) ;

	// Set the coord translation scheme to map to internal coords from raw gadget coords
	void setFromRawMap(std::shared_ptr<CoordMap> fromRawMap) ;

	// check the pixel coords are valid for this model
	bool checkCoord(const Coord& coord) const ;

	// convert from flat index to coord
	Coord fromIndex(unsigned index) const ;

	// convert from coord to flat index
	unsigned toIndex(const Coord& coord) const ;

private:
	unsigned mWidth ;
	unsigned mHeight ;
	unsigned mDepth ;
	unsigned mMaxIndex ;
	std::vector<Colour> mPixels ;

	std::shared_ptr<CoordMap> mToRawMap ;
	std::shared_ptr<CoordMap> mFromRawMap ;
	std::shared_ptr<CoordMap> mPixelMap ;

	mutable std::mutex mMutex ;
} ;

}

#endif /* LIBHYPNO_MODEL_INC_MODEL_H_ */
