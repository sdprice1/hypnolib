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
 * \file      Path.h
 * \brief     Various simple file/path utilities
 *
 * \date      7 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


#ifndef LIBHYPNOQUARTZ_INC_PATH_H_
#define LIBHYPNOQUARTZ_INC_PATH_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <errno.h>
#include <string>
#include <stdio.h>
#include <sys/stat.h>

namespace HypnoQuartz {

/*!
 * \class Path
 */
class Path {
public:
	static std::string SEPERATOR ;

	/**
	 * Is path a directory
	 * @param path
	 * @return true if directory
	 */
	static bool isDir(const std::string& path) ;

	/**
	 * Is path a file
	 * @param path
	 * @return true if file
	 */
	static bool isFile(const std::string& path);

	/**
	 * Is path a link
	 * @param path
	 * @return true if link
	 */
	static bool isLink(const std::string& path);

	/**
	 * Does path exist
	 * @param path
	 * @return true if exists
	 */
	static bool exists(const std::string& path) ;

	/**
	 * Split path into directory and basename
	 * @param path
	 * @return basename
	 */
	static std::string basename(const std::string& path) ;

	/**
	 * Split path into directory and basename
	 * @param path
	 * @return directory
	 */
	static std::string dir(const std::string& path) ;

	/**
	 * Join a directory and filename back into a full path
	 * @param dir
	 * @param filename
	 * @return path
	 */
	static std::string join(const std::string& dir, const std::string& filename) ;

	/**
	 * Extract last extension from filename. Examples:
	 *
	 * path				extension
	 * "file.txt"		".txt"
	 * "file.tgz.md5"	".md5"
	 * "file"			""
	 *
	 * @param path
	 * @return extension
	 */
	static std::string extension(const std::string& path) ;


} ;

}

#endif /* LIBHYPNOQUARTZ_INC_PATH_H_ */
