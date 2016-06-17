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
 * \file      GadgetQuartzSettings.h
 * \brief     Defaults stored in the cube
 *
 * \date      8 Jun 2016
 * \author    sdprice1
 *
 * \details   These tables can be read out of the cube, but take a while to transfer, so I've cached them in here instead
 *
 */


#ifndef LIBHYPNOQUARTZ_INC_GADGETQUARTZSETTINGS_H_
#define LIBHYPNOQUARTZ_INC_GADGETQUARTZSETTINGS_H_


namespace HypnoQuartz {

/*!
 * \class GadgetQuartzSettings
 */
class GadgetQuartzSettings {
public:
	virtual ~GadgetQuartzSettings() ;

	/**
	 * Singleton
	 */
	std::shared_ptr<GadgetQuartzSettings> getInstance() ;

	/**
	 * Get a copy of the complete list of visualisations
	 */
	std::map<unsigned, std::string> getVisualisations() const ;

	/**
	 * Get a copy of the complete list of transitions
	 */
	std::map<unsigned, std::string> getTransitions() const ;

	/**
	 * Write a new list of visualisations
	 */
	void setVisualisations(const std::map<unsigned, std::string>& visualisations) ;

	/**
	 * Write a new list of transitions
	 */
	void setTransitions(const std::map<unsigned, std::string>& transitions) ;

protected:
	GadgetQuartzSettings() ;

private:
	std::map<unsigned, std::string> mVisualisations ;
	std::map<unsigned, std::string> mTransitions ;
} ;

}

#endif /* LIBHYPNOQUARTZ_INC_GADGETQUARTZSETTINGS_H_ */
