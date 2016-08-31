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
 * \file      WinEmu.h
 * \brief     Emulate Windows functions
 *
 * \date      10 May 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef HYPNODEMO_LINUX_WINEMU_H_
#define HYPNODEMO_LINUX_WINEMU_H_

#ifndef WIN32

#include <sys/time.h>
#include "hypno/TimeUtils.h"

inline unsigned int timeGetTime()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_usec/1000;
}

#define Sleep(ms)	HypnoQuartz::TimeUtils::msSleep(ms)

#endif

#endif /* HYPNODEMO_LINUX_WINEMU_H_ */
