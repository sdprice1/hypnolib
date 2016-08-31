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
 * \file      FileDescriptor_unittest.cpp
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
#include "hypno/FileDescriptor.h"
#include "hypno/Thread.h"
#include "hypno/TimeUtils.h"

#include "gtest/gtest.h"


using namespace HypnoQuartz ;

//=============================================================================================================
// PUBLIC
//=============================================================================================================

class ReadThread : public Thread {
public:
	ReadThread(FileDescriptor& fd) :
		Thread("ReadThread"),
		mFd(fd)
	{}

	virtual bool run() override
	{
		std::cerr << "Read thread started" << std::endl ;

		// Cannot read until written
		std::vector<uint8_t> rx ;
		unsigned bytesWritten{1000} ;
		unsigned bytesRead{2000} ;

		// expect this to be aborted
		std::cerr << "Blocking read...." << std::endl ;
		EXPECT_EQ(IFile::FileStatus::ABORTED, mFd.read(rx, bytesWritten, bytesRead)) ;
		EXPECT_EQ(0u, bytesRead) ;
		std::cerr << "Blocking read end" << std::endl ;

		return false ;
	}


	FileDescriptor& mFd ;
};

//===================================================================================================================
class FileDescriptorTest : public ::testing::Test
{
public:

    /*! @brief Setup the test
     */
    virtual void SetUp()
    {
    	const ::testing::TestInfo* const test_info =
    	  ::testing::UnitTest::GetInstance()->current_test_info();

    	std::cout << std::endl << "*** " << test_info->test_case_name() << " Tests ***" <<  std::endl;
    	std::cout << test_info->name() << " - START" <<  std::endl;

    }

    /*! @brief Tear down the test
     */
    virtual void TearDown()
    {

    }

};




//===================================================================================================================
/*
 * Unit tests
 */

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FileDescriptorTest, Basic)
{
	// Block SIGPIPE signal so we can handle it via errno
	sigset_t blockSet, prevSet ;
	::sigemptyset(&blockSet) ;
	::sigaddset(&blockSet, SIGPIPE) ;
	::sigprocmask(SIG_BLOCK, &blockSet, &prevSet) ;

	// open a pipe
	int filedes[2] ;
	if (::pipe(filedes) != 0)
	{
		return ;
	}

	FileDescriptor reader(filedes[0]) ;
	FileDescriptor writer(filedes[1]) ;

	// Abort should return immediately
	reader.abort() ;
	writer.abort() ;

	// Should be able to write to writer
	std::string msg{"a simple message"} ;

	std::vector<uint8_t> tx(msg.begin(), msg.end()) ;
	unsigned bytesWritten{1000} ;
	EXPECT_EQ(IFile::FileStatus::OK, writer.write(tx, bytesWritten)) ;
	EXPECT_EQ(tx.size(), bytesWritten) ;

	std::vector<uint8_t> rx ;
	unsigned bytesRead{2000} ;
	EXPECT_EQ(IFile::FileStatus::OK, reader.read(rx, bytesWritten, bytesRead)) ;
	EXPECT_EQ(rx.size(), bytesWritten) ;
	EXPECT_EQ(rx.size(), bytesRead) ;

	std::string rxmsg(rx.begin(), rx.end()) ;
	EXPECT_EQ(msg, rxmsg) ;

	reader.close() ;
	writer.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FileDescriptorTest, Timeout)
{
	// Block SIGPIPE signal so we can handle it via errno
	sigset_t blockSet, prevSet ;
	::sigemptyset(&blockSet) ;
	::sigaddset(&blockSet, SIGPIPE) ;
	::sigprocmask(SIG_BLOCK, &blockSet, &prevSet) ;

	// open a pipe
	int filedes[2] ;
	if (::pipe(filedes) != 0)
	{
		return ;
	}

	FileDescriptor reader(filedes[0]) ;
	FileDescriptor writer(filedes[1]) ;

	// Cannot read until written
	std::vector<uint8_t> rx ;
	unsigned bytesWritten{1000} ;
	unsigned bytesRead{2000} ;
	EXPECT_EQ(IFile::FileStatus::TIMEOUT, reader.read(rx, bytesWritten, bytesRead, 200)) ;
	EXPECT_EQ(0u, bytesRead) ;

	reader.close() ;
	writer.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
TEST_F(FileDescriptorTest, ReadAbort)
{
	// Block SIGPIPE signal so we can handle it via errno
	sigset_t blockSet, prevSet ;
	::sigemptyset(&blockSet) ;
	::sigaddset(&blockSet, SIGPIPE) ;
	::sigprocmask(SIG_BLOCK, &blockSet, &prevSet) ;

	// open a pipe
	int filedes[2] ;
	if (::pipe(filedes) != 0)
	{
		return ;
	}

	FileDescriptor reader(filedes[0]) ;
	FileDescriptor writer(filedes[1]) ;

	// Spin off a read thread
	ReadThread rdThread(reader) ;
	rdThread.start() ;

	// wait for thread to start
	std::cerr << "Wait until read thread running..." << std::endl ;
	while (!rdThread.isRunning())
		TimeUtils::msSleep(10) ;


	// make sure FileDescriptor has chance to initiate read
	TimeUtils::msSleep(200) ;
	std::cerr << "Read thread running" << std::endl ;

	// Now abort
	std::cerr << "Aborting.." << std::endl ;
	reader.abort() ;
	std::cerr << "Aborted" << std::endl ;

	unsigned loopCount(100) ;
	while (rdThread.isRunning() && (--loopCount > 0))
		TimeUtils::msSleep(10) ;

	// ensure thread stopped
	EXPECT_FALSE(rdThread.isRunning()) ;

	reader.close() ;
	writer.close() ;
}

//-------------------------------------------------------------------------------------------------------------------
#if 0
TEST_F(FileDescriptorTest, AutoAbort)
{
	std::shared_ptr<ReadThread> rdThread ;


	{
		// Block SIGPIPE signal so we can handle it via errno
		sigset_t blockSet, prevSet ;
		::sigemptyset(&blockSet) ;
		::sigaddset(&blockSet, SIGPIPE) ;
		::sigprocmask(SIG_BLOCK, &blockSet, &prevSet) ;

		// open a pipe
		int filedes[2] ;
		if (::pipe(filedes) != 0)
		{
			return ;
		}

		FileDescriptor reader(filedes[0]) ;
		FileDescriptor writer(filedes[1]) ;

		// Spin off a read thread
		rdThread = std::make_shared<ReadThread>(reader) ;
		rdThread->start() ;

		// wait for thread to start
		std::cerr << "Wait until read thread running..." << std::endl ;
		while (!rdThread->isRunning())
			TimeUtils::msSleep(10) ;

		std::cerr << "Read thread running" << std::endl ;

		// Now exit - should auto abort
		std::cerr << "Destroy (Aborting).." << std::endl ;
	}

	// wait a while for everything to catch up
	TimeUtils::msSleep(400) ;

	// ensure thread stopped
	EXPECT_FALSE(rdThread->isRunning()) ;


}
#endif


//===================================================================================================================

