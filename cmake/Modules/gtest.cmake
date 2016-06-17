##======================================================================================================================
## Google Test CMake functions
##
##======================================================================================================================

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

## Guard from multiple includes
if (_INCLUDED_GTEST)
	return ()
endif ()
set (_INCLUDED_GTEST 1)

##----------------------------------------------------------------------------------------------------------------------
#find_package (GTest REQUIRED)
find_package (GTest)
#message("GTEST: ${GTEST_FOUND}")

if (GTEST_FOUND)
include_directories (SYSTEM ${GTEST_INCLUDE_DIRS})
endif()

##----------------------------------------------------------------------------------------------------------------------
## addGtest(<name> <libs> <sources>...)
## 
## Function that adds a Google Test based test executable, <name>, built from <sources> and adds the test <name>
## so that CTest will run it. Adds the current library and any additional libs specified by <libs> to the link targets.
##
macro (addGtest name libs)

	if (GTEST_FOUND)
		# Create the test executable from the sources
		add_executable(${name} ${ARGN})
		
		# Link with gtest lib and this project lib
		getCurrentLib(lib)
		addLinkLibs(${name} ${GTEST_LIBRARIES} gtest_main ${lib} ${libs})
	    target_link_libraries(${name} rt)
		
		# Create a test
		add_test(NAME ${name} COMMAND ${name} --gtest_output=xml:gtest.xml)
	endif()

#message("add_test(${name})")

endmacro (addGtest)

