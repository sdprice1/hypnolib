/*!
 * Copyright (c) 2016 Steve Price.
 *
 * This file is part of Hypnolib
 *
 * Hypnolib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hypnolib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hypnolib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \file      Packet_unittest.cpp
 * \brief     
 *
 * \date      11 May 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include <memory>

#include "Packet.h"

using namespace HypnoGadget ;

namespace {

	bool compare(uint8_t* data, uint8_t* expected, uint16_t length)
	{
		for (unsigned i=0; i < length; ++i)
		{
			if (data[i] != expected[i])
			{
				std::cerr << "Mismatch at byte " << i <<
					std::hex <<
					" got 0x" << data[i] <<
					" expected 0x" << expected[i] <<
					std::dec <<
					std::endl ;
			}
		}

		return true ;
	}
}

//-------------------------------------------------------------------------------------------------------------
TEST(Packet, Login)
{
	const ::testing::TestInfo* const test_info =
	  ::testing::UnitTest::GetInstance()->current_test_info();

	std::cout << std::endl << "*** " << test_info->test_case_name() << " Tests ***" <<  std::endl;
	std::cout << test_info->name() << " - START" <<  std::endl;

	PacketHandlerState state ;
	PacketReset(&state) ;

	// Login packet data:
	//  0xc0 0x60      0x05 0x00 0x00 0xab 0xad 0xdb  0xdc 0xde  0xf2 0x62   0xc0
	// sync  type/seq  len  dest |<-------data---------------->| `--CRC16-'  sync
	//
	// Note that the data length should be 5, but the data contains 0xc0 which will be expanded
	//
	uint8_t pdata[] = {0xc0,  0x60, 0x05, 0x00, 0x00, 0xab, 0xad, 0xdb, 0xdc, 0xde,  0xf2, 0x62,  0xc0} ;
	EXPECT_EQ(0u, PacketDecodeBytes(&state, pdata, sizeof(pdata))) ;

	uint8_t destination;
	uint8_t *data ;
	uint16_t length ;
	EXPECT_TRUE(PacketGetData(&state, &destination, &data, &length));

	EXPECT_EQ(5u, length) ;
	uint8_t expected[] = { 0x00, 0xab, 0xad, 0xc0, 0xde } ;
	EXPECT_TRUE(compare(data, expected, length)) ;
}

//-------------------------------------------------------------------------------------------------------------
TEST(Packet, PartialPacket)
{
	const ::testing::TestInfo* const test_info =
	  ::testing::UnitTest::GetInstance()->current_test_info();

	std::cout << std::endl << "*** " << test_info->test_case_name() << " Tests ***" <<  std::endl;
	std::cout << test_info->name() << " - START" <<  std::endl;

	PacketHandlerState state ;
	PacketReset(&state) ;

	// Login packet data:
	//  0xc0 0x60      0x05 0x00 0x00 0xab 0xad 0xdb  0xdc 0xde  0xf2 0x62   0xc0
	// sync  type/seq  len  dest |<-------data---------------->| `--CRC16-'  sync
	//
	// Note that the data length should be 5, but the data contains 0xc0 which will be expanded
	//
	uint8_t pdata[] = {0xc0,  0x60, 0x05, 0x00, 0x00, 0xab, 0xad, 0xdb, 0xdc, 0xde,  0xf2, 0x62,  0xc0} ;
	uint16_t total = sizeof(pdata) ;

	uint8_t destination;
	uint8_t *data ;
	uint16_t length ;

	unsigned offset(0) ;
	EXPECT_EQ(0u, PacketDecodeBytes(&state, &pdata[offset], 4)) ;
	total -= 4 ;
	offset += 4 ;
	EXPECT_FALSE(PacketGetData(&state, &destination, &data, &length));

	EXPECT_EQ(0u, PacketDecodeBytes(&state, &pdata[offset], 6)) ;
	total -= 6 ;
	offset += 6 ;
	EXPECT_FALSE(PacketGetData(&state, &destination, &data, &length));


	EXPECT_EQ(0u, PacketDecodeBytes(&state, &pdata[offset], total)) ;
	EXPECT_TRUE(PacketGetData(&state, &destination, &data, &length));

	EXPECT_EQ(5u, length) ;
	uint8_t expected[] = { 0x00, 0xab, 0xad, 0xc0, 0xde } ;
	EXPECT_TRUE(compare(data, expected, length)) ;
}

