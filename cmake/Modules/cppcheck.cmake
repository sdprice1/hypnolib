##======================================================================================================================
## Cppcheck CMake functions
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
if (_INCLUDED_CPPCHECK)
	return ()
endif ()
set (_INCLUDED_CPPCHECK 1)

##----------------------------------------------------------------------------------------------------------------------
## Find executable
FIND_PROGRAM( CPPCHECK_EXECUTABLE cppcheck PATHS /usr/bin NO_DEFAULT_PATH)
FIND_PROGRAM( CPPCHECK_EXECUTABLE cppcheck PATHS /usr/local/bin NO_DEFAULT_PATH)
logDebug("CPPCHECK=${CPPCHECK_EXECUTABLE}")


##----------------------------------------------------------------------------------------------------------------------
## addCppcheck(<name>)
## 
## Macro that adds a cppcheck. $name is the subdirectory name and will be used to create the cppcheck target
##
macro (addCppcheck name)

    if (CPPCHECK_EXECUTABLE)
	
	    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	    set (executable ${name})
	    set (srcDir "${CMAKE_SOURCE_DIR}/src")
	    set (incDir "${CMAKE_SOURCE_DIR}/inc")
        if (_relPath)
		    set (executable ${_relPath}/${name})
		    set (srcDir "${CMAKE_SOURCE_DIR}/${_relPath}/src")
		    set (incDir "${CMAKE_SOURCE_DIR}/${_relPath}/inc")
        endif()

        set (_targetname ${name}_cppcheck)
        
        ## Need to first check to ensure we have some source
        if (IS_DIRECTORY ${srcDir})

message("Adding ${name} for cppcheck (rel=${_relPath}) : src=${srcDir} inc=${incDir} cppcheck=${CPPCHECK_EXECUTABLE}")

			# check for include directory - we run cppcheck differently if there is or isn't an inc
	        if (IS_DIRECTORY ${incDir})
	
				ADD_CUSTOM_TARGET(${_targetname}
			
					# run cppcheck
					${CPPCHECK_EXECUTABLE}
					    --inline-suppr 
					    -j 4 
					    --enable=style 
					    --std=c++11 
					    --xml --xml-version=2
						--force 
						${srcDir} 
						-I ${incDir} 
						2>cppcheck.xml 
						1>cppcheck.log
			
					COMMENT "Running cppcheck on ${name}"
				)
				
			else (IS_DIRECTORY ${incDir})
			
				ADD_CUSTOM_TARGET(${_targetname}
			
					# run cppcheck
					${CPPCHECK_EXECUTABLE} 
					    --inline-suppr 
					    -j 4 
					    --enable=style 
					    --std=c++11 
					    --xml --xml-version=2
						--force 
						--force 
						${srcDir}
						-I ${CMAKE_SOURCE_DIR}/inc 
						2>cppcheck.xml 
						1>cppcheck.log
			
					COMMENT "Running cppcheck on ${name}"
				)
				
			endif (IS_DIRECTORY ${incDir})

			# Add this target to the project targets list
			add_targets( ${PROJECT_NAME}_cppcheck ${_targetname} )
		
		endif (IS_DIRECTORY ${srcDir})
		
	endif()

endmacro (addCppcheck)

##----------------------------------------------------------------------------------------------------------------------
## addCppcheckTarget()
## 
## Function that adds a cppcheck target for this repository
##
function (addCppcheckTarget)

	message ("PROJECT_NAME=${PROJECT_NAME} cppcheck=${${PROJECT_NAME}_cppcheck}")
	if (${PROJECT_NAME}_cppcheck)
		add_custom_target (cppcheck DEPENDS ${${PROJECT_NAME}_cppcheck})
	else()
	    # dummy
		add_custom_target (cppcheck)
	endif()

endfunction(addCppcheckTarget)

