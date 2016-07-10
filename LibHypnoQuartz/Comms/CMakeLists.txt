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
    src/Comms.cpp
    src/Socket.cpp
    src/TtyComms.cpp
    src/CommsClient.cpp
    src/CommsServer.cpp
)

## Cppcheck
include (cppcheck)
addCppcheck(Comms)

# Load google test
include (gtest)

include_directories(.)
set(gtest_src
	gtest/Socket_unittest.cpp
	gtest/SocketClient_unittest.cpp
	gtest/SocketClientServer_unittest.cpp
	gtest/Tty_unittest.cpp
	gtest/TtyClient_unittest.cpp
	gtest/TtyClientServer_unittest.cpp
	gtest/test.cpp
)
addGtest(Comms_GTEST "" ${gtest_src})

## Valgrind
include (valgrind)
addMemcheck(Comms_GTEST)
addProfile(Comms_GTEST)


## Echo server demo program
set(demo_src
	gtest/EchoServer.cpp
)

add_executable(EchoServer ${demo_src})
getCurrentLib(lib)
addLinkLibs(EchoServer ${lib})
target_link_libraries(EchoServer rt)


