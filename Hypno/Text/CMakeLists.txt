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
    src/FontChar.cpp
    src/FontChar4x4.cpp
    src/Font.cpp
    src/Font4x4.cpp
    src/AlienFont4x4.cpp
)

include_directories(.)
set(gtest_src
	gtest/FontChar_unittest.cpp
	gtest/Font4x4_unittest.cpp
)
addTests(Text "" ${gtest_src})
