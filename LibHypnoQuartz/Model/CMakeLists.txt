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
## Library Source
add_sources (SOURCES
    src/Coord.cpp
    src/CoordMap.cpp
    src/CubeCoordMap.cpp
    src/Colour.cpp
    src/Model.cpp
    src/SquareModel.cpp
    src/CubeModel.cpp
)

## Cppcheck
include (cppcheck)
addCppcheck(Model)

# Load google test
include (gtest)

include_directories(.)
set(gtest_src
	gtest/Coord_unittest.cpp
	gtest/CubeCoordMap_unittest.cpp
	gtest/Colour_unittest.cpp
)
addGtest(Model_GTEST "" ${gtest_src})

## Valgrind
include (valgrind)
addMemcheck(Model_GTEST)
addProfile(Model_GTEST)


