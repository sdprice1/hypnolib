##======================================================================================================================
## Shared CMake functions, Macros
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
if (_INCLUDED_FUNCTIONS)
	return ()
endif (_INCLUDED_FUNCTIONS)
set (_INCLUDED_FUNCTIONS 1)

##----------------------------------------------------------------------------------------------------------------------
## logMessage( <message> ... )
##
## Function that displays the message(s)
##
function (logMessage)

	foreach (msg ${ARGV})
		message (STATUS "${msg}")
	endforeach ()

endfunction (logMessage)


##----------------------------------------------------------------------------------------------------------------------
## repoLibName( <lib> <repo> [ <sub-dir> ] )
## 
## Function that sets $lib with the correct library name determined by the repository name and optionally a sub-directory.
## With no sub-directory specified $lib is just set to the repository name. With a sub-directory specified it concatentantes
## them to produce a combined library name
##
function (repoLibName lib repo subdir)
	
	logDebug("repoLibName(repo=${repo} subdir=${subdir})")
	
	# If subdir is of the form "../ETNA/waveform" then convert to just "waveform" i.e. strip off ../<dir>
	string (REGEX REPLACE "^\\.\\./[^/]+/" "/" subdir ${subdir})
	logDebug("repoLibName(now subdir=${subdir})")
	
	if (subdir)
	
	    ## Don't use sub-dirs if the directory prefix is set
	    usingSrcDirPrefix(usingPrefix)
	    if (${usingPrefix} GREATER 0)
	    
    		# don't use subdir
    		set (subdir "")

	    else (${usingPrefix} GREATER 0)
    		# If sub-dir is something like ETNA/adc then we use the "ETNA" part, however if it's just something
    		# like fault (for example from the common repository) then we don't use that. So, we only use the first
    		# dir of a directory path if that path contains multiple dirs; otherwise we don't use it at all
    		#
    		string (FIND ${subdir} "/" pos)
    		if (${pos} GREATER 0)
    			# strip off any subdirectories
    			string (SUBSTRING ${subdir} 0 ${pos} subdir)
    
    			# capitalise the first letter of the sub-directory (leave rest in same case)
    			string (SUBSTRING ${subdir} 0 1 firstChar)
    			string (SUBSTRING ${subdir} 1 -1 body)
    			string (TOUPPER ${firstChar} firstChar)
    			string (CONCAT subdir ${firstChar} ${body})
    
    		logDebug("stripping trailing dirs...")
    			
    		else(${pos} GREATER 0)
    			# don't use subdir
    			set (subdir "")
    		endif(${pos} GREATER 0)

	    endif (${usingPrefix} GREATER 0)
		
		# create the combined name		
		set (${lib} "${repo}${subdir}" PARENT_SCOPE)
		
		logDebug("repoLibName = lib=${repo}${subdir} (pos=${pos})")
		
	else ()
	
		set (${lib} "${repo}" PARENT_SCOPE)

	endif ()

endfunction(repoLibName)


##----------------------------------------------------------------------------------------------------------------------
## getCurrentLib( <lib> )
## 
## Function that determines the current library. 
##
function (getCurrentLib lib)
	
 	set (libname ${PROJECT_NAME})
    logDebug("getCurrentLib() PROJECT_NAME=${PROJECT_NAME} relpath=${_relPath} lib=${libname}")

	set( ${lib} ${libname} PARENT_SCOPE)
	
endfunction(getCurrentLib)


##----------------------------------------------------------------------------------------------------------------------
## findIncludes( <dir> <var>)
## 
## Macro that finds header files in a variety of subdirectories under <dir>. Sets <var> with the results
##
function (findIncludes dir var)

	file(GLOB_RECURSE ${var} 
	    "${dir}/inc/*.h*" 
	    "${dir}/*/inc/*.h*" 
	    "${dir}/*/*/inc/*.h*" 
	    "${dir}/*/*/*/inc/*.h*" 
	    "${dir}/inc/*/*.h*" 
	    "${dir}/*/inc/*/*.h*" 
	    "${dir}/*/*/inc/*/*.h*" 
	    "${dir}/*/*/*/inc/*/*.h*" 
	)
	
    # return
    set(${var} ${${var}} PARENT_SCOPE)
	
endfunction (findIncludes)


##----------------------------------------------------------------------------------------------------------------------
## addIncludes()
## 
## Macro that adds the include path(s) to ${PROJECT_NAME}_INCLUDE_DIRS variable and also adds the paths to the included
## directories list. Searches in this subdirectory for any header files stored under 'inc' directory
##
macro (addIncludes)

    findIncludes(${CMAKE_CURRENT_SOURCE_DIR} _HEADERS)

message("addIncludes(${PROJECT_NAME}): CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} HEADERS=${_HEADERS}")

	foreach (_headerFile ${_HEADERS})

	    get_filename_component(_dir ${_headerFile} PATH)
        list (APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${_dir})
	        
   	endforeach()
	
	list(LENGTH ${PROJECT_NAME}_INCLUDE_DIRS _numIncDirs)
	if (${_numIncDirs})
	
    	list(REMOVE_DUPLICATES ${PROJECT_NAME}_INCLUDE_DIRS)
    	include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

	endif()
	
message("addIncludes(${PROJECT_NAME}): ${PROJECT_NAME}_INCLUDE_DIRS=${${PROJECT_NAME}_INCLUDE_DIRS}")

endmacro (addIncludes)

##----------------------------------------------------------------------------------------------------------------------
## addInclude(path)
## 
## Macro that adds the specified path to the includes directories list as well as the ${PROJECT_NAME}_INCLUDE_DIRS list
## (used to create includes path for a library)
##
macro (addInclude path)

    list (APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${path})
    include_directories(${path})

endmacro (addInclude)


##----------------------------------------------------------------------------------------------------------------------
## exportLib()
## 
## Function that exports this library
##
function (exportLib)
	string (TOLOWER ${PROJECT_NAME} project_name)

#	message("exportLib(${PROJECT_NAME}) - exporting CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
	export (TARGETS ${PROJECT_NAME} FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake)
	
	# set up some variables used in the config files
	set (binary_dir ${${PROJECT_NAME}_BINARY_DIR})
	set (include_dirs ${${PROJECT_NAME}_INCLUDE_DIRS})
	
	configure_file( 
		${TOPDIR}/cmake/inc/pkg-config.cmake.in
		${${PROJECT_NAME}_BINARY_DIR}/${project_name}-config.cmake @ONLY)
	
	configure_file( 
		${TOPDIR}/cmake/inc/pkg-config-version.cmake.in
		${${PROJECT_NAME}_BINARY_DIR}/${project_name}-config-version.cmake @ONLY)
		
	## Also create an alias target so we can just run 'make lib'
	add_custom_target(lib DEPENDS ${PROJECT_NAME})

#	message("exportLib(${PROJECT_NAME}) - END")

endfunction(exportLib)

##----------------------------------------------------------------------------------------------------------------------
## addLib(var libname)
## 
## Macro that finds the specified library and adds the real library name to the 'var' list
##
macro (addLib var libName)

message("addLib CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} BUILD_TYPE=${BUILD_TYPE}")
    set(paths "${CMAKE_SOURCE_DIR}/${libName}/build/${BUILD_TYPE}")
    list(APPEND paths "${CMAKE_SOURCE_DIR}/../${libName}/build/${BUILD_TYPE}")
    
	# Find the package
	find_package (${libName} PATHS ${paths} NO_DEFAULT_PATH)
	if (NOT ${libName}_VERSION)
		message (FATAL_ERROR "Unable to load lib ${libName} - searched in ${paths} for build")
	endif ()
	
	# Package will set pkgName to the real name
message("pkgName=${pkgName}")
message("${pkgName}_INCLUDE_DIRS=${${pkgName}_INCLUDE_DIRS}")
	
	# Add the include paths
	include_directories (${${pkgName}_INCLUDE_DIRS})
	
	list (APPEND ${var} ${pkgName})
	
message("var=${var} value=${${var}}")

endmacro (addLib)

##----------------------------------------------------------------------------------------------------------------------
## addLinkLibs(<target>)
## 
## Function that adds link libraries to the target. Wraps the libraries in a start/end group
##
function (addLinkLibs target)

	target_link_libraries(${target} -Wl,--start-group ${ARGN} -Wl,--end-group)
	logDebug("addLinkLibs(${target}) LIBS: --start<< ${ARGN} >>end--")
	
endfunction(addLinkLibs)


##----------------------------------------------------------------------------------------------------------------------
## addExecutable(<name> <libs> <sources>...)
## 
## Function that adds the executable, <name>, built from <sources>. Adds the any additional libs specified by 
## <libs> to the link targets.
##
macro (addExecutable name libs)

message("addExecutable name=${name} libs=${libs} source=${ARGN})")

	# Create the test executable from the sources
	add_executable(${name} ${ARGN})
	
	addLinkLibs(${name} ${libs})
    target_link_libraries(${name} rt)

endmacro (addExecutable)


##----------------------------------------------------------------------------------------------------------------------
## pushSrcDir( <dir> )
## 
## Function which adds a sub-directory name to the list of sub-directory prefixes that will be used by add_sources
##
function (pushSrcDir dir)

logDebug ("pushSrcDir(${dir}) PROJECT_NAME=${PROJECT_NAME} CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")    

    get_property( src_dirs GLOBAL PROPERTY SRC_DIRS_PREFIX )
    
    list (APPEND src_dirs ${dir})
    
logDebug("pushSrcDir(${dir}) - src_dirs=${src_dirs}")    
    
    set_property( GLOBAL PROPERTY SRC_DIRS_PREFIX ${src_dirs} )

endfunction(pushSrcDir)

##----------------------------------------------------------------------------------------------------------------------
## popSrcDir( )
## 
## Function which removes a sub-directory name from the list of sub-directory prefixes that will be used by add_sources
##
function (popSrcDir)
    get_property( src_dirs GLOBAL PROPERTY SRC_DIRS_PREFIX )
    
logDebug("popSrcDir() - src_dirs=${src_dirs}")    

    list (LENGTH src_dirs len)
    if (${len} EQUAL 0)
        message(FATAL_ERROR "Attempting to pop source directory from empty list")
    endif ()
    
    list (REMOVE_AT src_dirs -1)
    
logDebug("popSrcDir() - now src_dirs=${src_dirs}")    

    set_property( GLOBAL PROPERTY SRC_DIRS_PREFIX ${src_dirs} )
    
endfunction(popSrcDir)

##----------------------------------------------------------------------------------------------------------------------
## addSrcDirPrefix( <path> )
## 
## Function which adds the list of sub-directory prefixes to the path stored in variable named <path>
##
function (addSrcDirPrefix path)
    get_property( src_dirs GLOBAL PROPERTY SRC_DIRS_PREFIX )
    
    list (LENGTH src_dirs len)
    if (${len} EQUAL 0)
        return ()
    endif ()
    
    # Prefix the path
    set (prefix "")
    foreach (dir ${src_dirs})
        set (prefix "${prefix}${dir}/")
    endforeach ()

logDebug("addSrcDirPrefix(${${path}}) - src_dirs=${src_dirs} prefix=${prefix}")    
    
    # return the new path
    set (${path} ${prefix}${${path}} PARENT_SCOPE)

endfunction(addSrcDirPrefix)

##----------------------------------------------------------------------------------------------------------------------
## usingSrcDirPrefix( <flag> )
## 
## Sets <flag> if we are using a source path directory prefix. This means that we're under some sub-directory and don't
## want to add these directory names to the top-level repository library name
##
function (usingSrcDirPrefix flag)
    get_property( src_dirs GLOBAL PROPERTY SRC_DIRS_PREFIX )
    
    list (LENGTH src_dirs len)

    # return flag    
    set (${flag} ${len} PARENT_SCOPE)

endfunction(usingSrcDirPrefix)



##----------------------------------------------------------------------------------------------------------------------
## add_sources( <var> ... )
## 
## Macro that adds the source files to the named variable
##
macro (add_sources var)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

logDebug ("add_sources(${var}) PROJECT_NAME=${PROJECT_NAME} CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} relPath=${_relPath}")    
	
	if (_relPath MATCHES "^\\.\\./.*")
	
	    # relative (external) path - use as-is
	    
	elseif(_relPath MATCHES "^/.*")
	
	    # absolute path - use as-is
	    
	else()
	
    	# strip off any prefix directories (library may be in a sub-directory under the main repository (e.g. hardware/common)
    	get_filename_component(_relPath ${_relPath} NAME)

    endif() 
    
    # Now add any directory prefix values
    addSrcDirPrefix(_relPath)

logDebug ("add_sources(${var}) relPath=${_relPath}")    

    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND ${var} "${_relPath}/${_src}")
        else()
            list (APPEND ${var} "${_src}")
        endif()
    endforeach()
    
logDebug ("add_sources(${var}) _relPath=${_relPath} LIST: ${var}=${${var}}")    
    
    if (_relPath)
        # propagate var to parent directory
        set (${var} ${${var}} PARENT_SCOPE)

logDebug ("add_sources(${var}) set ${var} in PARENT")    

    endif()
endmacro(add_sources)

##----------------------------------------------------------------------------------------------------------------------
## add_targets( <var> ... )
## 
## Macro that adds the targets to the named variable
##
macro (add_targets var)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_target ${ARGN})
        list (APPEND ${var} "${_target}")
    endforeach()
    if (_relPath)
        # propagate var to parent directory
        set (${var} ${${var}} PARENT_SCOPE)
    endif()
endmacro(add_targets)

##----------------------------------------------------------------------------------------------------------------------
## addOptDir( <dir> )
## 
## Macro that adds an optional subdirectory, but only adds it if it exists
##
macro (addOptDir dir)
	if (EXISTS ${CMAKE_SOURCE_DIR}/${dir})
		add_subdirectory(${dir})
    endif()
endmacro(addOptDir)


##----------------------------------------------------------------------------------------------------------------------
## addTests(<name> <libs> <sources>...)
## 
## Macro that adds all the back-end tests
##
macro (addTests name libs)

	## Cppcheck
	include (cppcheck)
	addCppcheck(${name})

	# Load google test
	include (gtest)
	addGtest(${name}_GTEST "${libs}" ${ARGN})

	## Valgrind
	include (valgrind)
	addMemcheck(${name}_GTEST)
	addProfile(${name}_GTEST)
	
	## Coverage
	include (coverage)
	addCoverage(${name}_GTEST)
	
	
endmacro (addTests)

