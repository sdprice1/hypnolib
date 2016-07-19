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
 * \file      TimeUtils.cpp
 * \brief     
 *
 * \date      8 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "TimeUtils.h"

#include <sys/time.h>
#include <stdio.h>
#include <ctime>


using namespace HypnoQuartz;

//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================

//-------------------------------------------------------------------------------------------------------------
void TimeUtils::msSleep(unsigned msecs)
{
	struct timespec ts{0, 0};
	if( msecs >= 1000)
	{
		ts.tv_sec = msecs / 1000;
		ts.tv_nsec = (msecs % 1000) * 1000000L;
	}
	else
	{
		ts.tv_nsec = msecs * 1000000L;
	}
	nanosleep(&ts, (struct timespec *)0);
}

//-------------------------------------------------------------------------------------------------------------
std::string TimeUtils::timestamp()
{
	timespec tNow ;
	clock_gettime(CLOCK_REALTIME, &tNow) ;

	// Convert to string
    unsigned us = (unsigned)(tNow.tv_nsec / 1000L) ;

	struct tm result ;
	struct tm  *ts = localtime_r(&tNow.tv_sec, &result);

	char buff[80];
	std::strftime(buff, sizeof(buff), "%H:%M:%S", ts);

	char timer_buff[128];
	snprintf(timer_buff, sizeof(timer_buff), "%s.%06u", buff, us) ;

	std::string timestamp(timer_buff) ;
	return timestamp ;
}
