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
 * \file      Path_unittest.cpp
 * \brief     
 *
 * \date      9 Jun 2016
 * \author    sdprice1
 *
 * \details   
 *
 */


//=============================================================================================================
// INCLUDE
//=============================================================================================================
#include "Gtesting.h"

#include "Path.h"

using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

//===================================================================================================================
class PathTest : public Gtesting {};


//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, exists)
{
	EXPECT_TRUE(Path::exists("../../common/gtest/test/a_dir")) ;
	EXPECT_TRUE(Path::exists("../../common/gtest/test/b_dir")) ;
	EXPECT_FALSE(Path::exists("../../common/gtest/test/c_dir")) ;
	EXPECT_TRUE(Path::exists("../../common/gtest/test/a_file")) ;
	EXPECT_TRUE(Path::exists("../../common/gtest/test/b_file")) ;
	EXPECT_FALSE(Path::exists("../../common/gtest/test/c_file")) ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isDir)
{
	EXPECT_TRUE(Path::isDir("../../common/gtest/test/a_dir")) ;
	EXPECT_TRUE(Path::isDir("../../common/gtest/test/b_dir")) ;
	EXPECT_FALSE(Path::isDir("../../common/gtest/test/c_dir")) ;
	EXPECT_FALSE(Path::isDir("../../common/gtest/test/a_file")) ;
	EXPECT_FALSE(Path::isDir("../../common/gtest/test/b_file")) ;
	EXPECT_FALSE(Path::isDir("../../common/gtest/test/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isFile)
{
	EXPECT_FALSE(Path::isFile("../../common/gtest/test/a_dir")) ;
	EXPECT_FALSE(Path::isFile("../../common/gtest/test/b_dir")) ;
	EXPECT_FALSE(Path::isFile("../../common/gtest/test/c_dir")) ;

	EXPECT_TRUE(Path::isFile("../../common/gtest/test/a_file")) ;
	EXPECT_TRUE(Path::isFile("../../common/gtest/test/b_file")) ;
	EXPECT_FALSE(Path::isFile("../../common/gtest/test/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isLink)
{
	EXPECT_FALSE(Path::isLink("../../common/gtest/test/a_dir")) ;
	EXPECT_FALSE(Path::isLink("../../common/gtest/test/b_dir")) ;
	EXPECT_FALSE(Path::isLink("../../common/gtest/test/c_dir")) ;

	EXPECT_FALSE(Path::isLink("../../common/gtest/test/a_file")) ;
	EXPECT_FALSE(Path::isLink("../../common/gtest/test/b_file")) ;
	EXPECT_FALSE(Path::isLink("../../common/gtest/test/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, basename)
{
	EXPECT_EQ(std::string("a_dir"), Path::basename("../../common/gtest/test/a_dir")) ;
	EXPECT_EQ(std::string("b_dir"), Path::basename("../../common/gtest/test/b_dir")) ;
	EXPECT_EQ(std::string("a_file"), Path::basename("/test/a_file")) ;
	EXPECT_EQ(std::string("b_file"), Path::basename("/b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, directory)
{
	EXPECT_EQ(std::string("../../common/gtest/test/"), Path::dir("../../common/gtest/test/a_dir")) ;
	EXPECT_EQ(std::string("../../common/gtest/test/"), Path::dir("../../common/gtest/test/b_dir")) ;
	EXPECT_EQ(std::string("/test/"), Path::dir("/test/a_file")) ;
	EXPECT_EQ(std::string("/"), Path::dir("/b_file")) ;
	EXPECT_EQ(std::string(""), Path::dir("b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, join)
{
	EXPECT_EQ(std::string("../../common/gtest/test/a_dir"), Path::join("../../common/gtest/test", "a_dir")) ;
	EXPECT_EQ(std::string("../../common/gtest/test/a_dir"), Path::join("../../common/gtest/test/", "a_dir")) ;
	EXPECT_EQ(std::string("/test/a_file"), Path::join("/test/", "a_file")) ;
	EXPECT_EQ(std::string("/test/a_file"), Path::join("/test", "a_file")) ;
	EXPECT_EQ(std::string("/b_file"), Path::join("", "/b_file")) ;
	EXPECT_EQ(std::string("b_file"), Path::join("", "b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, extension)
{
	EXPECT_EQ(std::string(".tgz"), Path::extension("../../common/gtest/test/a_file.tgz")) ;
	EXPECT_EQ(std::string(".md5"), Path::extension("../../common/gtest/test/a_file.tgz.md5")) ;
	EXPECT_EQ(std::string(""), Path::extension("../../common/gtest/test/a_file")) ;
	EXPECT_EQ(std::string(""), Path::extension("a_file")) ;
	EXPECT_EQ(std::string(""), Path::extension("")) ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, links)
{
	system("ln -sf b_file ../../common/gtest/test/b_link ") ;
	system("ln -sf ../test/a_file ../../common/gtest/test/a_link ") ;

	EXPECT_TRUE(Path::exists("../../common/gtest/test/a_link")) ;
	EXPECT_TRUE(Path::exists("../../common/gtest/test/b_link")) ;

	EXPECT_FALSE(Path::isFile("../../common/gtest/test/a_link")) ;
	EXPECT_FALSE(Path::isFile("../../common/gtest/test/b_link")) ;

	EXPECT_TRUE(Path::isLink("../../common/gtest/test/a_link")) ;
	EXPECT_TRUE(Path::isLink("../../common/gtest/test/b_link")) ;

}


//===================================================================================================================

