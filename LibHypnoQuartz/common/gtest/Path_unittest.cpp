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

const std::string TEST_PATH{"../../../common/gtest/test"} ;

//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, exists)
{
	EXPECT_TRUE(Path::exists(TEST_PATH + "/a_dir")) ;
	EXPECT_TRUE(Path::exists(TEST_PATH + "/b_dir")) ;
	EXPECT_FALSE(Path::exists(TEST_PATH + "/c_dir")) ;
	EXPECT_TRUE(Path::exists(TEST_PATH + "/a_file")) ;
	EXPECT_TRUE(Path::exists(TEST_PATH + "/b_file")) ;
	EXPECT_FALSE(Path::exists(TEST_PATH + "/c_file")) ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isDir)
{
	EXPECT_TRUE(Path::isDir(TEST_PATH + "/a_dir")) ;
	EXPECT_TRUE(Path::isDir(TEST_PATH + "/b_dir")) ;
	EXPECT_FALSE(Path::isDir(TEST_PATH + "/c_dir")) ;
	EXPECT_FALSE(Path::isDir(TEST_PATH + "/a_file")) ;
	EXPECT_FALSE(Path::isDir(TEST_PATH + "/b_file")) ;
	EXPECT_FALSE(Path::isDir(TEST_PATH + "/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isFile)
{
	EXPECT_FALSE(Path::isFile(TEST_PATH + "/a_dir")) ;
	EXPECT_FALSE(Path::isFile(TEST_PATH + "/b_dir")) ;
	EXPECT_FALSE(Path::isFile(TEST_PATH + "/c_dir")) ;

	EXPECT_TRUE(Path::isFile(TEST_PATH + "/a_file")) ;
	EXPECT_TRUE(Path::isFile(TEST_PATH + "/b_file")) ;
	EXPECT_FALSE(Path::isFile(TEST_PATH + "/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, isLink)
{
	EXPECT_FALSE(Path::isLink(TEST_PATH + "/a_dir")) ;
	EXPECT_FALSE(Path::isLink(TEST_PATH + "/b_dir")) ;
	EXPECT_FALSE(Path::isLink(TEST_PATH + "/c_dir")) ;

	EXPECT_FALSE(Path::isLink(TEST_PATH + "/a_file")) ;
	EXPECT_FALSE(Path::isLink(TEST_PATH + "/b_file")) ;
	EXPECT_FALSE(Path::isLink(TEST_PATH + "/c_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, basename)
{
	EXPECT_EQ(std::string("a_dir"), Path::basename(TEST_PATH + "/a_dir")) ;
	EXPECT_EQ(std::string("b_dir"), Path::basename(TEST_PATH + "/b_dir")) ;
	EXPECT_EQ(std::string("a_file"), Path::basename("/test/a_file")) ;
	EXPECT_EQ(std::string("b_file"), Path::basename("/b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, directory)
{
	EXPECT_EQ(std::string(TEST_PATH + "/"), Path::dir(TEST_PATH + "/a_dir")) ;
	EXPECT_EQ(std::string(TEST_PATH + "/"), Path::dir(TEST_PATH + "/b_dir")) ;
	EXPECT_EQ(std::string("/test/"), Path::dir("/test/a_file")) ;
	EXPECT_EQ(std::string("/"), Path::dir("/b_file")) ;
	EXPECT_EQ(std::string(""), Path::dir("b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, join)
{
	EXPECT_EQ(std::string(TEST_PATH + "/a_dir"), Path::join(TEST_PATH + "", "a_dir")) ;
	EXPECT_EQ(std::string(TEST_PATH + "/a_dir"), Path::join(TEST_PATH + "/", "a_dir")) ;
	EXPECT_EQ(std::string("/test/a_file"), Path::join("/test/", "a_file")) ;
	EXPECT_EQ(std::string("/test/a_file"), Path::join("/test", "a_file")) ;
	EXPECT_EQ(std::string("/b_file"), Path::join("", "/b_file")) ;
	EXPECT_EQ(std::string("b_file"), Path::join("", "b_file")) ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, extension)
{
	EXPECT_EQ(std::string(".tgz"), Path::extension(TEST_PATH + "/a_file.tgz")) ;
	EXPECT_EQ(std::string(".md5"), Path::extension(TEST_PATH + "/a_file.tgz.md5")) ;
	EXPECT_EQ(std::string(""), Path::extension(TEST_PATH + "/a_file")) ;
	EXPECT_EQ(std::string(""), Path::extension("a_file")) ;
	EXPECT_EQ(std::string(""), Path::extension("")) ;
}


//-------------------------------------------------------------------------------------------------------------------
TEST_F(PathTest, links)
{
	int rc ;
	rc = system( std::string("ln -sf b_file " + TEST_PATH + "/b_link ").c_str() ) ;
	if (rc < 0)
		::perror("system") ;
	rc = system( std::string("ln -sf ../test/a_file " + TEST_PATH + "/a_link ").c_str() ) ;
	if (rc < 0)
		::perror("system") ;

	EXPECT_TRUE(Path::exists(TEST_PATH + "/a_link")) ;
	EXPECT_TRUE(Path::exists(TEST_PATH + "/b_link")) ;

	EXPECT_FALSE(Path::isFile(TEST_PATH + "/a_link")) ;
	EXPECT_FALSE(Path::isFile(TEST_PATH + "/b_link")) ;

	EXPECT_TRUE(Path::isLink(TEST_PATH + "/a_link")) ;
	EXPECT_TRUE(Path::isLink(TEST_PATH + "/b_link")) ;

}


//===================================================================================================================

