##======================================================================================================================
## CMake settings
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

# Include guard
if (_INCLUDED_SETTINGS)
	return ()
endif (_INCLUDED_SETTINGS)
set (_INCLUDED_SETTINGS 1)


##----------------------------------------------------------------------------------------------------------------------
## logDebug( <message> ... )
##
## Function that displays the message(s) if $DEBUG_CMAKLIST is true
##
function (logDebug)

	if (DEBUG_CMAKELIST)
		foreach (msg ${ARGV})
			message (STATUS "DEBUG: ${msg}")
		endforeach ()
	endif()

endfunction (logDebug)

##----------------------------------------------------------------------------------------------------------------------
## getEnvVar( <var> <default> )
##
## Macro that creates a new variable based on the current environment variable value with the same name. If the env
## var is not set then uses the specified default value
##
macro (getEnvVar var default)

	set (${var} $ENV{${var}})
	logDebug("getEnvVar(${var}) ENV=${${var}} (default=${default})")
	if (NOT DEFINED ${var})
		set (${var} ${default})
		logDebug("getEnvVar(${var}) : set default=${default}")
	endif()

endmacro (getEnvVar)

##----------------------------------------------------------------------------------------------------------------------
## getCachedEnvVar( <var> <default> <description> )
##
## Macro that first checks for a set environment variable and if present uses that value. If not it uses the cached variable
## or default if cached variable isn't set. The final value is then saved in the cache
##
macro (getCachedEnvVar var default description)

	set (envVar $ENV{${var}})
	logDebug("getCachedEnvVar(${var}) ENV=${envVar} (default=${default})")
	if (DEFINED envVar)
	
		set (${var} ${envVar})
		logDebug("getCachedEnvVar(${var}) = ENV ${envVar}")
		
	else()
	    # See if got cached value
    	if (NOT DEFINED ${var})
    
    		logDebug("getCachedEnvVar(${var}) : set default = ${default}")
    
    		# get the variable
    		getEnvVar( ${var} ${default} )
    
    	endif()
	endif()

	# save to the cache
	set (${var} ${${var}} CACHE STRING "${description}" FORCE)
	logDebug("Saved ${var} = ${${var}} to cache")

endmacro (getCachedEnvVar)


##----------------------------------------------------------------------------------------------------------------------
## Settings

## Expect TOPDIR to be set here

##----------------------------------------------------------------------------------------------------------------------
## Set up link flags
set (LINK_FLAGS "")

# coverage
if (${BUILD_TYPE} STREQUAL "Coverage")
	set (LINK_FLAGS "-pg --coverage ${LINK_FLAGS}")
endif ()


##----------------------------------------------------------------------------------------------------------------------
## Set up compiler flags

# Debug
if (${BUILD_TYPE} STREQUAL "Debug")
	set (COMPILE_FLAGS "${COMPILE_FLAGS} -O0 -g3")
endif()

# coverage
if (${BUILD_TYPE} STREQUAL "Coverage")
	set (COMPILE_FLAGS "-pg --coverage ${COMPILE_FLAGS}")
endif ()

if (TEST_TTY)
	set (COMPILE_FLAGS "${COMPILE_FLAGS} -DTEST_TTY")
endif()

##----------------------------------------------------------------------------------------------------------------------
## Common compile settings

# C Compiler 
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fmessage-length=0 ${COMPILE_FLAGS}")

# C++ Compiler 
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fmessage-length=0 -std=gnu++11 -pthread ${COMPILE_FLAGS}")

# Linker
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  ${LINK_FLAGS} ")


##----------------------------------------------------------------------------------------------------------------------
## Testing

# Allow testing for host builds
enable_testing()




