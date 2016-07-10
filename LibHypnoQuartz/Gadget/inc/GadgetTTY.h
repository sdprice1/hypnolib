#ifndef GADGETTTY_H
#define GADGETTTY_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Gadget.h"  // the gadget interface
#include "Debuggable.h"

namespace HypnoQuartz {

class GadgetTtyNoLock : public HypnoGadget::GadgetLock
{
public:
	virtual ~GadgetTtyNoLock() {}

	void Lock(void) {};	   // do nothing
	void Unlock(void) {};  // do nothing
};

/* The second class handles reading and writing bytes from the serial port.
*/
class GadgetTTY : public HypnoGadget::GadgetIO, public Debuggable
{
public:
	// default constructor
	GadgetTTY(void) ;

	// default destructor
	virtual ~GadgetTTY() ;

	// try to open a com port, given its name. Waits for device to be available (if it isn't already) and then attempts to connect
	// return true on success, else false
	bool waitOpen(const std::string & portName);

	// try to open a com port, given its name
	// return true on success, else false
	bool Open(const std::string & portName);

	// close any open port
	void Close(void) ;

	// Read any available data - returns immediately
	int readNoBlocking(uint8_t  * buffer, uint16_t  length) ;

	// Wait for at least 1 byte of data before returning
	int read(uint8_t  * buffer, uint16_t  length) ;

	// TX buffer empty
	bool isTxDone() ;

	// RX buffer empty
	bool isRxDone() ;

public:
	// HypnoGadget::GadgetIO interface

	// Needed by the GadgetControl class
	// read bytes from the IO buffer to be processed
	// of max given length, and return size read
	virtual uint16_t  ReadBytes(uint8_t  * buffer, uint16_t  length) override ;

	// Needed by the GadgetControl class
	// write bytes to the IO buffer
	virtual void WriteBytes(const uint8_t  * buffer, uint16_t  length) override ;

private:
	// open tty
	bool ttyOpen(const std::string & portName) ;

	// generic "file" open
	bool genericOpen(const std::string & portName) ;

private:
	// Thread handler
	void threadRun() ;

private:
	bool mRun ;
	int mFd;
	std::vector<uint8_t > mTxBuffer ;
	std::vector<uint8_t > mRxBuffer ;

	std::thread mThread ;
	std::mutex mMutex ;
	std::condition_variable mCondOpen ;
	std::condition_variable mCondRx ;
};

};

#endif // GADGETTTY_H
