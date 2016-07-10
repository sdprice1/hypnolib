##----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.7 FATAL_ERROR)
project (LibHypno CXX)
set (version "0.1")


## Copyright (c) 2016 Steve Price
##
## This file is part of Hypnocube
##
## Hypnocube is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Hypnocube is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Hypnocube.  If not, see <http://www.gnu.org/licenses/>.

##----------------------------------------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${TOPDIR}/cmake/Modules)
include (settings)
include (functions)

##----------------------------------------------------------------------------------------------------------------------
## Headers
message("LibHypno")
addIncludes()

##----------------------------------------------------------------------------------------------------------------------
## Library Source
set (SOURCES
    src/CRC16.cpp
    src/Command.cpp
    src/Gadget.cpp
    src/Packet.cpp
)

## Cppcheck
include (cppcheck)
addCppcheck(LibHypno)

	# Load google test
	include (gtest)

	include_directories(.)
	set(gtest_src
		gtest/Packet_unittest.cpp
	)
	addGtest(lib_GTEST "" ${gtest_src})


add_library (${PROJECT_NAME} STATIC ${SOURCES})
addCppcheckTarget()

##----------------------------------------------------------------------------------------------------------------------
exportLib()


