##======================================================================================================================
## Valgrind CMake functions
##
##======================================================================================================================

## Guard from multiple includes
if (_INCLUDED_VALGRIND)
	return ()
endif ()
set (_INCLUDED_VALGRIND 1)



##----------------------------------------------------------------------------------------------------------------------
## Find executable
FIND_PROGRAM( VALGRIND_EXECUTABLE valgrind)
set (VALGRIND_FOUND 1)
if (${VALGRIND_EXECUTABLE} STREQUAL "VALGRIND_EXECUTABLE-NOTFOUND")
	set (VALGRIND_FOUND 0)
endif()
message("VALGRIND: Found=${VALGRIND_FOUND} prog=${VALGRIND_EXECUTABLE}")

##----------------------------------------------------------------------------------------------------------------------
## addMemcheck(<name>)
##
## Macro that adds a Valgrind memory leak test. $name is the executable name and will be used to create the coverage target
##
macro (addMemcheck name)

if (VALGRIND_FOUND)

    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    set (executable ${name})
    set (srcDir "${CMAKE_SOURCE_DIR}/src")

    if (_relPath)
    	if (_relPath MATCHES "^\\.\\./.*")

    	    # relative (external) path - use as-is

    	elseif(_relPath MATCHES "^/.*")

    	    # absolute path - use as-is

    	else()

		    set (executable ${_relPath}/${name})
		    set (srcDir "${CMAKE_SOURCE_DIR}/${_relPath}/src")

        endif()
    endif()


message("Adding ${name} for memcheck, exe=${executable} (rel=${_relPath})...")

	set (_targetname ${name}_memcheck)

	ADD_CUSTOM_TARGET(${_targetname}

		# run valgrind in memory leak check mode
		${VALGRIND_EXECUTABLE}
			--verbose --leak-check=full  --show-reachable=yes --track-origins=yes
			--undef-value-errors=yes
			--xml=yes --xml-file=${name}.memcheck
			./${name}
			&> memcheck.log
            || true

		COMMENT "Running valgrind memcheck on ${name}"
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}
	)

	# Add this coverage target to the project coverage targets list
	add_targets( ${PROJECT_NAME}_memcheck ${_targetname} )

message("Memcheck:  add ${_targetname} to ${PROJECT_NAME}_memcheck=${${PROJECT_NAME}_memcheck}")

endif (VALGRIND_FOUND)

endmacro (addMemcheck)

##----------------------------------------------------------------------------------------------------------------------
## addMemcheckTarget()
##
## Function that adds a memory leak target for this repository if we're building with settings that do not require coverage
## and there are some memory test results
##
function (addMemcheckTarget)

message("addMemcheckTarget")

	if (${PROJECT_NAME}_memcheck)
		add_custom_target (memcheck DEPENDS ${${PROJECT_NAME}_memcheck})
	else()
	    # Dummy target
		add_custom_target (memcheck)
	endif()

endfunction(addMemcheckTarget)

##----------------------------------------------------------------------------------------------------------------------
## addProfile(<name>)
##
## Macro that adds a Valgrind profiling test. $name is the executable name and will be used to create the profiling target
##
macro (addProfile name)

if (VALGRIND_FOUND)

    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    set (executable ${name})
    set (srcDir "${CMAKE_SOURCE_DIR}/src")

    if (_relPath)
    	if (_relPath MATCHES "^\\.\\./.*")

    	    # relative (external) path - use as-is

    	elseif(_relPath MATCHES "^/.*")

    	    # absolute path - use as-is

    	else()

		    set (executable ${_relPath}/${name})
		    set (srcDir "${CMAKE_SOURCE_DIR}/${_relPath}/src")

        endif()
    endif()


message("Adding ${name} for profiling, exe=${executable} (rel=${_relPath})...")

	set (_targetname ${name}_profile)

	ADD_CUSTOM_TARGET(${_targetname}

		# run valgrind in profiling mode
		${VALGRIND_EXECUTABLE}
			--tool=callgrind
			./${name}
            || true

		COMMENT "Running valgrind callgrind on ${name}"
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}
	)

	# Add this coverage target to the project coverage targets list
	add_targets( ${PROJECT_NAME}_profile ${_targetname} )

message("Profile:  add ${_targetname} to ${PROJECT_NAME}_profile=${${PROJECT_NAME}_profile}")

endif (VALGRIND_FOUND)

endmacro (addProfile)

##----------------------------------------------------------------------------------------------------------------------
## addProfileTarget()
##
## Function that adds a profiling target for this repository if we're building with settings that do not require coverage
##
function (addProfileTarget)

message("addProfileTarget")

	if (${PROJECT_NAME}_profile)
		add_custom_target (profile DEPENDS ${${PROJECT_NAME}_profile})
	else()
	    # Dummy target
		add_custom_target (profile)
	endif()

endfunction(addProfileTarget)

