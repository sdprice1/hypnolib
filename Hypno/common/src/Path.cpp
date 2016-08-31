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
 * \file      Path.cpp
 * \brief     Various simple file/path utilities
 *
 * \date      7 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================

// C lib
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>

// C++
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "hypno/Path.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// TYPES
//=============================================================================================================

using Stat = struct stat;


//=============================================================================================================
// PUBLIC STATIC
//=============================================================================================================
std::string Path::SEPERATOR{"/"} ;
static const char SEPERATOR_CHAR{ Path::SEPERATOR[0] } ;

//-------------------------------------------------------------------------------------------------------------
bool Path::isDir(const std::string& path)
{
	// check that it's a dir
	struct stat s;
	if( ::lstat(path.c_str(), &s) != 0 )
		return false ;

    if( S_ISDIR(s.st_mode) )
    	return true ;

    return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Path::isFile(const std::string& path)
{
	// check that it's a file
	struct stat s;
	if( ::lstat(path.c_str(), &s) != 0 )
		return false ;

    if( S_ISREG(s.st_mode) )
    	return true ;

    return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Path::isLink(const std::string& path)
{
	// check that it's a file
	struct stat s;
	if( ::lstat(path.c_str(), &s) != 0 )
		return false ;

    if( S_ISLNK(s.st_mode) )
    	return true ;

    return false ;
}

//-------------------------------------------------------------------------------------------------------------
bool Path::exists(const std::string& path)
{
	struct stat s;
	if( ::lstat(path.c_str(), &s) != 0 )
		return false ;

	return true ;
}

//-------------------------------------------------------------------------------------------------------------
std::string Path::basename(const std::string& path)
{
	std::size_t sepPos(path.find_last_of(SEPERATOR)) ;
	if (sepPos == std::string::npos)
		return path ;

	return path.substr(sepPos+1) ;
}

//-------------------------------------------------------------------------------------------------------------
std::string Path::dir(const std::string& path)
{
	std::size_t sepPos(path.find_last_of(SEPERATOR)) ;
	if (sepPos == std::string::npos)
		return "" ;

	return path.substr(0, sepPos+1) ;
}

//-------------------------------------------------------------------------------------------------------------
std::string Path::join(const std::string& dir, const std::string& basename)
{
	if (dir.empty())
		return basename ;

	std::string path(dir) ;

	if (path.back() != SEPERATOR_CHAR)
		path.append(SEPERATOR) ;

	path.append(basename) ;
	return path ;
}

//---------------------------------------------------------------------
std::string Path::extension(const std::string& path)
{
	std::string base(basename(path)) ;

	// find the last '.'
	std::size_t pos(base.find_last_of('.')) ;
	if (pos == std::string::npos)
		return "" ;

	return base.substr(pos) ;
}


