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
 * \file      Debuggable.h
 * \brief     Debug class - makes any derived class debuggable
 *
 * \date      24 Jun 2016
 * \author    sdprice1
 *
 * \details   Deriving from this class allows debug output to be generated
 *
 */


#ifndef LIBHYPNOQUARTZ_DEBUG_INC_DEBUGGABLE_H_
#define LIBHYPNOQUARTZ_DEBUG_INC_DEBUGGABLE_H_

#include "IDebug.h"
#include "DebugStream.h"

namespace HypnoQuartz {

/*!
 * \class Debuggable
 */
class Debuggable : public virtual IDebug {
public:
	explicit Debuggable(const std::string& name) ;
	virtual ~Debuggable() ;

	/**
	 * Get the object's name
	 */
	virtual std::string getName() const override ;

	/**
	 * Set the debug level for this object
	 */
	virtual void debugLevel(IDebug::DebugLevel level) override ;

	/**
	 * Is this object's debug level set to this level
	 */
	virtual bool isDebug(IDebug::DebugLevel level) const override ;


	/**
	 * Get current timestamps option.
	 */
	virtual bool getOptionsTimestamp() const override ;
	/**
	 * Get current name option.
	 */
	virtual bool getOptionsName() const override ;
	/**
	 * Get current colour option.
	 */
	virtual bool getOptionsColour() const override ;

	/**
	 * Enable/disable timestamps. When enabled lines are formatted as:
	 *
	 * [10:11:21.3456] Line of debug
	 *
	 * When combined with other options, timestamp is always first to be displayed
	 */
	virtual bool setOptionsTimestamp(bool enable) override ;

	/**
	 * Enable/disable object names. When enabled lines are formatted as:
	 *
	 * [Class] Line of debug
	 *
	 * When combined with other options name is always second to be displayed
	 */
	virtual bool setOptionsName(bool enable) override ;

	/**
	 * Enable/disable colourisation using ANSI escape codes
	 */
	virtual bool setOptionsColour(bool enable) override ;


protected:
	/**
	 * Member data which provides the interface for generating debug output for normal level debug messages
	 */
	DebugStream debugNormal ;

	/**
	 * Member data which provides the interface for generating debug output for verbose level debug messages
	 */
	DebugStream debugVerbose ;

private:
	const std::string mName ;
} ;

}

#endif /* LIBHYPNOQUARTZ_DEBUG_INC_DEBUGGABLE_H_ */
