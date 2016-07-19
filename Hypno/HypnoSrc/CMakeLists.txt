
##----------------------------------------------------------------------------------------------------------------------
## Library Source
add_sources (SOURCES
    src/CRC16.cpp
    src/Command.cpp
    src/Gadget.cpp
    src/Packet.cpp
)

include_directories(.)
set(gtest_src
	gtest/Packet_unittest.cpp
)
addTests(hypnoSrc "" ${gtest_src})



