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
 * \file      DebugMgr.h
 *
 * \date      29 Jun 2016
 * \author    sdprice1
 *
 */


#ifndef LIBHYPNO_DEBUG_INC_DEBUGMGR_H_
#define LIBHYPNO_DEBUG_INC_DEBUGMGR_H_

#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "IDebug.h"

namespace HypnoQuartz {

/*!
 * \class DebugMgr
 * \brief     Manages debugging
 * \details   Provides the static IDebug methods and manages debug
 */
class DebugMgr : public virtual IDebug {
public:
	DebugMgr() ;
	virtual ~DebugMgr() ;

	/**
	 * Adds this named object to the list of known names
	 */
	void addName(const std::string& name) ;

	/**
	 * Sets the debug level globally
	 */
	void setDebug(DebugLevel level) ;

	/**
	 * Sets debug level for these named objects
	 */
	void setDebug(const std::string& name, DebugLevel level) ;

	/**
	 * Does the named class have a debug level at or above the specified value
	 */
	bool isDebug(const std::string& name, DebugLevel level) const ;


	// IDebug interface

	/**
	 * Get the object's name
	 */
	virtual std::string getName() const override ;

	/**
	 * Set the debug level for this object
	 */
	virtual void debugLevel(DebugLevel level) override ;

	/**
	 * Is this object's debug level set to this level
	 */
	virtual bool isDebug(DebugLevel level) const override ;


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

private:
	IDebug::DebugLevel mGlobalLevel ;
	std::map<std::string, IDebug::DebugLevel> mLevels ;
	bool mOptionTimestamp ;
	bool mOptionName ;
	bool mOptionColour ;
	mutable std::mutex mMutex ;
} ;

}

#endif /* LIBHYPNO_DEBUG_INC_DEBUGMGR_H_ */
