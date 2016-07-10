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
 * \file      IDebug.h
 * \brief     Virtual interface for debug
 *
 * \date      24 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef LIBHYPNOQUARTZ_DEBUG_INC_IDEBUG_H_
#define LIBHYPNOQUARTZ_DEBUG_INC_IDEBUG_H_

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace HypnoQuartz {

/*!
 * \class IDebug
 */
class IDebug {
public:
	IDebug() {}
	virtual ~IDebug() {}

	/**
	 * Singleton for debug manager
	 */
	static std::shared_ptr<IDebug> getInstance() ;

	/**
	 * Debug levels
	 */
	enum DebugLevel {
		NONE		=0,
		NORMAL		=1,
		VERBOSE		=2
	};

	/**
	 * Static method for dumping out a vector of data
	 */
	static void dump(const std::string& msg, const uint8_t  * buffer, uint16_t  dataSize);
	static void dump(const std::string& msg, const std::vector<uint8_t>& buffer);

	/**
	 * Adds this named object to the list of known names
	 */
	static void addName(const std::string& name) ;

	/**
	 * Sets the debug level globally
	 */
	static void setDebug(DebugLevel level) ;

	/**
	 * Sets debug level for these named objects
	 */
	static void setDebug(const std::string& name, DebugLevel level) ;

	/**
	 * Does the named class have a debug level at or above the specified value
	 */
	static bool isDebug(const std::string& name, DebugLevel level) ;

	/**
	 * Set/get global options. Options available are:
	 *
	 * Show timestamps at start of each debug line
	 * Show name of object generating the debug
	 * Enable ANSI colourisation
	 */
	static bool optionsTimestamp() ;
	static bool optionsName() ;
	static bool optionsColour() ;
	static bool optionsTimestamp(bool enable) ;
	static bool optionsName(bool enable) ;
	static bool optionsColour(bool enable) ;


	// IDebug interface

	/**
	 * Get the object's name
	 */
	virtual std::string getName() const =0 ;

	/**
	 * Set the debug level for this object
	 */
	virtual void debugLevel(DebugLevel level) =0 ;

	/**
	 * Is this object's debug level set to this level
	 */
	virtual bool isDebug(DebugLevel level) const =0 ;


	/**
	 * Get current timestamps option.
	 */
	virtual bool getOptionsTimestamp() const =0 ;
	/**
	 * Get current name option.
	 */
	virtual bool getOptionsName() const =0 ;
	/**
	 * Get current colour option.
	 */
	virtual bool getOptionsColour() const =0 ;

	/**
	 * Enable/disable timestamps. When enabled lines are formatted as:
	 *
	 * [10:11:21.3456] Line of debug
	 *
	 * When combined with other options, timestamp is always first to be displayed
	 */
	virtual bool setOptionsTimestamp(bool enable) =0 ;

	/**
	 * Enable/disable object names. When enabled lines are formatted as:
	 *
	 * [Class] Line of debug
	 *
	 * When combined with other options name is always second to be displayed
	 */
	virtual bool setOptionsName(bool enable) =0 ;

	/**
	 * Enable/disable colourisation using ANSI escape codes
	 */
	virtual bool setOptionsColour(bool enable) =0 ;



} ;

}

#endif /* LIBHYPNOQUARTZ_DEBUG_INC_IDEBUG_H_ */
