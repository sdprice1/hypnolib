// HypnoCOMM - serial communications for the HypnoGadgets
// Copyright Chris Lomont 2007-2008
// www.HypnoCube.com, www.HypnoSquare.com
// code to handle the gadget 
#include "Gadget.h"
#include "Packet.h"
#include "Command.h"
#include "Options.h"

#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <deque>

using namespace std;
using namespace HypnoGadget;

// todo - walk through and clean

namespace {

// interface for bytes to be passed back and forth to the Command functions

static const char * errMsgs_[] = {
     "0 = no error",
     "1 = timeout - too long of a delay between packets.",
     "2 = missing packet, followed by missing sequence number.",
     "3 = invalid checksum.",
     "4 = invalid type (2 and 3 defined for now).",
     "5 = invalid sequence counter.",
     "6 = missing SYNC - SYNC out of order (2 SYNC in a row, for example).",
     "7 = invalid packet length.",
     "8 = invalid command.",
     "9 = invalid data (valid command).",
    "10 = invalid ESC sequence - illegal byte after ESC byte.",
    "11 = overflow - too much data was fed in with the packets.",
    "12 = command not implemented (in case command deliberately not allowed).",
    "13 = invalid login value."
	};

	}; // anonymous namespace


namespace HypnoGadget {

// todo - all GadgetControl member functions need moved to Implementation, and outer class merely locks, calls pImpl, then unlocks on all public and protected calls?!
/* Design: class GadgetImpl is designed to be on one thread, and can call functions within itself without locking
           class GadgetControl can be called from any thread, so locks and calls GadgetImpl versions
*/

// global function to write bytes from the packet code
// param hides the "this" class, and byte is written
void IOWriteByte(void * param, uint8_t byte); 

/********************* GadgetImpl Section ********************************/
/*                                                                       */
/*                                                                       */
/********************* GadgetImpl Section ********************************/

class GadgetControl::GadgetImpl
	{
public:
	GadgetImpl(GadgetIO & gadgetIO, GadgetLock* lock) : gadgetIO_(gadgetIO), lock_(lock)
		{
		obtainedFrame_  = false;
		loginState_     = Disconnected;
		challengeValue_ = 0xABADC0DE; // default value
		lastInfoType_   = lastInfoIndex_ = 0;
		optionsDirty_   = false;
		optionsLoaded_  = false;
		byteMode_       = GadgetControl::ConsoleMode;
		consoleSize_    = 10000; // default size
		::memset(&options_,0,sizeof(HypnoGadget::Options));
		::memset(frameBuffer_,0,sizeof(frameBuffer_));
		PacketReset(&packetState_);

		replyCallback_ = ReplyCallback() ;

		};

	struct VersionInfo
		{
		uint8_t major_, minor_;
		VersionInfo(void) : major_(0), minor_(0) {};
		};
	struct Visualization
		{
		string name_;
		};
	struct Transition
		{
		string name_;
		};

	// wrapper for packet data
	bool PacketSendData(uint8_t destination, const uint8_t * data, uint16_t length)
		{ // todo - this needs locked?! but cannot lock here else error!
		bool retval = HypnoGadget::PacketSendData(&packetState_, IOWriteByte, this, destination, data, length);
		return retval;
		}

	GadgetControl::ByteMode byteMode_; // console/packet

	string consoleText_; // string holding console bytes
	uint32_t consoleSize_; // max number of bytes we allow in console string

	HypnoGadget::Options options_; // local copy - todo - reset it?

	// frame
	bool obtainedFrame_;    // is there a frame in the buffer?
	uint8_t frameBuffer_[96]; // todo - define size somewhere?

	// return true if a frame ready to read
	// resets internal flag when read
	// returns pointer to internal buffer and size of buffer
	bool GetFrame(uint8_t ** buffer, int & size)
		{
		bool ret = obtainedFrame_;
		obtainedFrame_ = false;
		*buffer = frameBuffer_;
		size = sizeof(frameBuffer_);
		return ret;
		}



ByteMode GetByteMode(void) const
	{
	return byteMode_;
	}

// console bytes are logged, this retrieves a copy of the current log
void ConsoleLog(std::string & text)
	{
	text = consoleText_;
	}

// only store so many characters (0 for infinite, default 10000)
void ConsoleReset(uint32_t size)
	{
	consoleSize_ = size;  // todo clamp text here
	}

// clear the console text
void ConsoleClear(void)
	{
	consoleText_ = "";
	}

/* commands to send to gadget */
void Login(uint32_t val)
	{
	uint8_t data[5]={Command::Login};
	data[1] = static_cast<uint8_t>(val>>24);
	data[2] = static_cast<uint8_t>(val>>16);
	data[3] = static_cast<uint8_t>(val>>8);
	data[4] = static_cast<uint8_t>(val);
	PacketSendData(0, data, 5);
	AddACKWatch(packetState_.packetEncodedCRC_,"Login",Command::Login);
	AddMessageToLog("Login sent");
	} // Login

void Logout(void)
	{
	uint8_t data[1]={Command::Logout};
	PacketSendData(0, data, sizeof(data));
	AddMessageToLog("Logout sent");
	AddACKWatch(packetState_.packetEncodedCRC_,"Logout",Command::Logout);
	}


void GetFrame(void)
	{
	uint8_t data[1]={Command::GetFrame};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"GetFrame",Command::GetFrame);
	AddMessageToLog("GetFrame sent");
	}

void SetFrame(const uint8_t * buffer)
	{
	uint8_t data[97];
	data[0] = Command::SetFrame;
	memcpy(data+1,buffer,96);
	PacketSendData(0, data, 97);
	AddACKWatch(packetState_.packetEncodedCRC_,"SetFrame",Command::SetFrame);
	AddMessageToLog("SetFrame sent");
	} // SetFrame

void FlipFrame(void)
	{
	uint8_t data[1];
	data[0] = Command::FlipFrame;
	PacketSendData(0, data, 1);
	AddACKWatch(packetState_.packetEncodedCRC_,"FlipFrame",Command::FlipFrame);
	AddMessageToLog("FlipFrame sent");
	} // FlipFrame


void MaxVisIndex(void)
	{
	uint8_t data[1]={Command::MaxVisIndex};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"MaxVisIndex",Command::MaxVisIndex);
	AddMessageToLog("MaxVisIndex sent");
	}

void SelectVis(uint8_t vis)
	{
	uint8_t data[2]={Command::SelectVis,vis};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"SelectVis",Command::SelectVis);
	AddMessageToLog("SelectVis sent");
	}

void MaxTranIndex(void)
	{
	uint8_t data[1]={Command::MaxTranIndex};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"MaxTranIndex",Command::MaxTranIndex);
	AddMessageToLog("MaxTranIndex sent");
	}

void SelectTran(uint8_t trans)
	{
	uint8_t data[2]={Command::SelectTran,trans};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"SelectTran",Command::SelectTran);
	AddMessageToLog("SelectTran sent");
	}

// send Options command, writing data if write = true
// else requesting reading data
void Options(bool write)
	{
	if (false == write)
		{
		uint8_t data[1]={Command::Options};
		PacketSendData(0, data, sizeof(data));
		}
	else
		{
		uint8_t data[sizeof(::Options)];
		memcpy(data,&options_,sizeof(::Options));
		data[0] = Command::Options;
		PacketSendData(0, data, sizeof(data));
		}
	AddACKWatch(packetState_.packetEncodedCRC_,"Options",Command::Options);
	AddMessageToLog("Options sent");
	} // Options

void Version(void)
	{
	uint8_t data[1]={Command::Version};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"Version",Command::Version);
	AddMessageToLog("Version sent");
	}

void Info(uint8_t type, uint8_t index)
	{
	uint8_t data[3]={Command::Info,type,index};
	PacketSendData(0, data, sizeof(data));
	lastInfoType_ = type;   // getting this type
	lastInfoIndex_ = index; // getting this index
#if 0 // todo - remove
	char temp[100];
	sprintf(temp,"Info sent %d %d",type,index);
	AddMessageToLog(temp);
#else
	AddMessageToLog("Info sent");
#endif
	}

void Ping(void)
	{
	uint8_t data[1]={Command::Ping};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"Ping",Command::Ping);
	AddMessageToLog("Ping sent");
	} // Ping

void Reset(void)
	{
	uint8_t data[1]={Command::Reset};
	PacketSendData(0, data, sizeof(data));
	AddACKWatch(packetState_.packetEncodedCRC_,"Reset",Command::Reset); // todo- only add those that generate an ACK?
	AddMessageToLog("Reset sent");
	} // Reset

void Info(const string & msg)
	{

#if 0 // todo - remove
	char temp[100];
	sprintf(temp,"Info msg %d %s",msg.length(),msg.c_str());
	AddMessageToLog(temp);
#endif

//std::cerr << "lastInfoType_=" << (unsigned)lastInfoType_ << " lastInfoIndex_=" << (unsigned)lastInfoIndex_ << std::endl ;
	switch (lastInfoType_)
		{
		case 0 :  // info about device
			switch (lastInfoIndex_)
				{
				case 0 : // name
					deviceName_ = msg;
					Info(0,1);
					break;
				case 1 : // description
					deviceDescription_ = msg;
					Info(0,2);
					break;
				case 2 : // copyright
					copyright_ = msg;
					Info(1,0); // read visualization items
					break;
				default :
					ErrorMessage("Error: unsupported Info command index");
					break;
				} // switch for Info about device
			break;
		case 1 :  // visualization info
			if (0 == lastInfoIndex_) // starting over
				visualizationList_.clear();
			if (0 == msg.length())
				{ // no item, last request out of bounds, go to next info item
				Info(2,0);
				}
			else
				{ // add item, request next one
				GadgetImpl::Visualization v;
				v.name_ = msg;
				visualizationList_.push_back(v);
				Info(1,lastInfoIndex_+1);
				}
			break;
		case 2 :  // transition info
			if (0 == lastInfoIndex_) // starting over
				transitionList_.clear();
			if (0 == msg.length())
				{ // no item, last request out of bounds, go to next info item
				// none left to read, let's get the options
				Options(false);
				}
			else
				{ // add item, request next one
				GadgetImpl::Transition t;
				t.name_ = msg;
				transitionList_.push_back(t);
				Info(2,lastInfoIndex_+1);
				}
			break;
		default:
			ErrorMessage("Error: unsupported Info command index");
			break;
		}
	} // Info

// get the count of items loaded
uint8_t GetCount(InfoType type)
	{
	if (VisualizationType == type)
		return static_cast<uint8_t>(visualizationList_.size());
	else if (TransitionType == type)
		return static_cast<uint8_t>(transitionList_.size());
	return 0;
	}

// get 0 numbered item, return blank string if out of bounds
void GetName(InfoType type, std::string & name, uint8_t index)
	{
	name = "";
	if ((VisualizationType == type) && (index < visualizationList_.size()))
		name = visualizationList_[index].name_;
	else if ((TransitionType == type) && (index < transitionList_.size()))
		name = transitionList_[index].name_;
	}


// Here is the ability to read and write options as a block
// get/set a copy of the options stored in the class
// to get them from the device, use the Options command
bool GetOptions(HypnoGadget::Options & opts)
	{
	// todo - this and many other places are not thread safe! things need locked on a finer level
	// pick variables that need locked, and this thread and the calling one need BOTH locked
	opts = options_;	  
	return optionsLoaded_;
	}

void SetOptions(const HypnoGadget::Options & opts)
	{
	options_ = opts;
	}

// process commands being sent back and forth to the gadget
// call fairly often
// todo - rewrite this entire section - compartmentalize it
void Update(void)
	{

	// write any bytes out 
	Lock();
	if (false == packetBytes_.empty())
		{
		gadgetIO_.WriteBytes(&packetBytes_[0],
			static_cast<uint16_t>(packetBytes_.size()));
		packetBytes_.resize(0);
		}
	Unlock();



	uint8_t buffer[64];
	uint16_t byteCount = 0, bytesUsed = 0;

	// get any bytes that are ready from the connection, and process
	byteCount = gadgetIO_.ReadBytes(buffer,64);

	while (bytesUsed < byteCount)
		{
		// handle console mode and packet mode
		if (ConsoleMode == GetByteMode())
			{ // add bytes to console text until a sync character seen or out of bytes
			while ((bytesUsed < byteCount) && (PacketSYNC != buffer[bytesUsed]))
				consoleText_.push_back(buffer[bytesUsed++]);
			if ((0 != consoleSize_) && (consoleText_.length() > consoleSize_))   // truncate it
				consoleText_ = consoleText_.substr(consoleText_.length() - consoleSize_);
			if (PacketSYNC == buffer[bytesUsed])
				byteMode_ = PacketMode;
			}
		else // PacketMode 
			{ 
			// process any outstanding commands, posting bytes to 
			// packet decoder, until no bytes left to feed in

			// pass out any bytes read in
			uint16_t leftBytes;
			leftBytes = PacketDecodeBytes(&packetState_, buffer + bytesUsed, byteCount - bytesUsed);
			bytesUsed += (byteCount - bytesUsed) - leftBytes; // next location

			// see if a packet is ready, returns true iff one is ready
			// sets a pointer to the decoded data
			// if one was ready, resets internals to process the next packet
			uint8_t dest;    // who gets the command
			uint8_t * data;  // data for command
			uint16_t length; // length of data
			if (true == PacketGetData(&packetState_, &dest, &data, &length))
				{ // we have a command to process, do it
				ProcessCommand(dest,data,length);
				if (LoggedIn != GetState())
					byteMode_ = ConsoleMode; // return to console mode 
				}
			if (PacketErrorNone != PacketGetError(&packetState_))
				{
				PacketError error = PacketGetError(&packetState_);
				PacketClearError(&packetState_); // todo - handle better
				ErrorMessage(string("PacketError ") + errMsgs_[error]);
				}
			} // packet bytes
		} // while bytes left to process
	} // Update

// read/write state of the gadget
GadgetControl::LoginState GetState(void) const
	{
	return loginState_;
	}
void SetState(GadgetControl::LoginState state)
	{
	loginState_ = state;
	if (LoggedIn == state)
		byteMode_ = PacketMode; // must be the case
	}
string GetDescription(void)
	{
	return deviceDescription_;
	}
string GetDevice(void)
	{
	return deviceName_;
	}
string GetCopyright(void)
	{
	return copyright_;
	}

// process a single command from the gadget
void ProcessCommand(uint8_t dest, const uint8_t * data, uint16_t length)
	{
	if (0 == length)
		{
		ErrorMessage("Error: Invalid length 0 data packet");
		return;
		}
	Command::CommandType type = static_cast<Command::CommandType>(*data++);
	--length;
	uint16_t crc(0);

	switch (type)
		{
		// 0.3 protocol commands
		case Command::Logout :
			AddMessageToLog("Logout received");
			break;
		case Command::Ping :
			AddMessageToLog("Ping received");
			break;
		case Command::Ack:
			{
			Command::CommandType command;
			crc = *data;
			crc <<= 8;
			crc += *(data+1);
			string text, msg;
			if (true == RemoveACKWatch(crc,text,command))
			{
				msg = "Ack received: " + text;

				// Update callback that reply to this command has been received
				type = command ;
			}
			else
			{
				msg = "Ack received: UNKNOWN";
			}
			AddMessageToLog(msg);
//std::cerr << "Ack received: " << text << std::endl;

			if (Command::Login == command)
				{
				loginState_ = LoggedIn;
				byteMode_ = PacketMode;
				}
			else if (Command::Logout == command)
				{
				loginState_ = LoggedOut;
				byteMode_ = ConsoleMode;
				}
			}
			break;
		case Command::Version :
			{
			AddMessageToLog("Version received");
			hardwareVersion_.major_ = *data++; // hardware
			hardwareVersion_.minor_ = *data++;
			softwareVersion_.major_ = *data++; // software
			softwareVersion_.minor_ = *data++;
			protocolVersion_.major_ = *data++; // protocol
			protocolVersion_.minor_ = *data++;
			}
			break;
		case Command::Error :
		{
			AddMessageToLog("Error received");
			if (length >= 1)
			{
				ErrorMessage(string("Error packet: ") + errMsgs_[*data++]);
//				std::cerr << "Error packet: " << errMsgs_[*data++] << std::endl ;
			}
			else
			{
				ErrorMessage("Error packet: ???");
//				std::cerr << "Error packet: ???" << std::endl ;
			}
		}
			break;
		// 0.4 protocol commands
		case Command::FlipFrame :
			ErrorMessage("Error: received illegal FlipFrame command");
			break;
		case Command::SetFrame :
			ErrorMessage("Error: received illegal SetFrame command");
			break;

		// 0.5 protocol commands
		case Command::Options :
			AddMessageToLog("Options received");
			// NOTE - options block sent with the command embedded
			//        thus the +1 on length is correct, we also back up data
			--data;
			length++;
			if (length != sizeof(::Options)) 
			{
//				std::cerr << "Options size=" << length << " expected " << sizeof(::Options) << std::endl ;
				ErrorMessage("Error: Options received, wrong length");
				return ;
			}

			// copy options if correct version - todo - make getter/setter for fields
				if (OPTIONS_VERSION != (*(data+1)))
					{
					ErrorMessage("Error: Options received, wrong version");
					}
				else
					{
					memcpy(&options_,data,sizeof(::Options));
					optionsLoaded_ = true;
					}
			break;
		case Command::Info :
			{
			AddMessageToLog("Info received");
			string msg;
			while ((length--) && (0 != *data))
				msg.push_back(*data++);
			Info(msg);
			}
			break;

		// 0.6 protocol commands
		case Command::MaxVisIndex :
			AddMessageToLog("MaxVisIndex received");
			if (1 != length)
				ErrorMessage("Error: incorrect length");
			break;
		case Command::MaxTranIndex :
			AddMessageToLog("MaxTranIndex received");
			if (1 != length)
				ErrorMessage("Error: incorrect length");
			break;
		case Command::GetFrame :
			{
			AddMessageToLog("GetFrame received");
			if (sizeof(frameBuffer_) == length)
				{
				memcpy(frameBuffer_,data,sizeof(frameBuffer_));
				obtainedFrame_ = true;
				}
			else
				{
				ErrorMessage("Error: incorrect length");
				}
			}			
			break;
		default :
			{
			stringstream errMsg;
			errMsg << "Unknown packet type " << type;
			ErrorMessage(errMsg.str());
			type = Command::Unknown ;
			}
			break;
		}

//		std::cerr << "REPLY: " << type << " '" << Command::commandStr(type) << "'" << std::endl ;
		if (replyCallback_)
			replyCallback_(type, length+1, crc) ;

	} // ProcessCommand

// add byte to the ones to sent to the physical gadget
void AddByte(uint8_t byte)
	{
	packetBytes_.push_back(byte);
	}

// return true if there has been an error, get the last error message
// resets error message
bool Error(string & errMsg)
	{
	errMsg = errorMessage_;
	errorMessage_ = "";
	return errMsg.size() > 0;
	}	   


// get message from log
// -1 index resets the log
// return true iff valid index
bool GetMessage(string & message, int index)
	{
	if (-1 == index)
		messages_.clear();
	else if ((index >= 0) && (index < static_cast<int>(messages_.size())))
		{
		message = messages_[index];
		return true;
		}
	return false;
	}

// get versions after Version called
void GetVersion(VersionType type, uint8_t & major, uint8_t & minor)
	{
	switch (type)
		{
		case SoftwareVersion :
			major = softwareVersion_.major_;
			minor = softwareVersion_.minor_;
			break;
		case HardwareVersion :
			major = hardwareVersion_.major_;
			minor = hardwareVersion_.minor_;
			break;
		case ProtocolVersion :
			major = protocolVersion_.major_;
			minor = protocolVersion_.minor_;
			break;
		}
	}

void registerReplyCallback(ReplyCallback callback)
{
	replyCallback_ = callback ;
}

private: 
	/* variables viewable from outside - need locked on write */
	vector<Visualization> visualizationList_;
	vector<Transition>    transitionList_;
	LoginState loginState_;
	vector<uint8_t> packetBytes_;

	// todo - default challenge value, allow setting it
	uint32_t challengeValue_; 

	/* variables usuable only inside - don't need thread locked */
	GadgetIO & gadgetIO_; // already threadsafe internally to object
	// gadget versions
	VersionInfo hardwareVersion_, softwareVersion_, protocolVersion_;
	string deviceName_;
	string deviceDescription_;
	string copyright_;
	// data written to go to the serial connection
	deque<string> messages_;
	string errorMessage_;

	// way to check packets sent to find ACK for them
	// ACK gives CRC16 and last byte counter for a command, 
	// map CRC to pair of command string and Command::CommandType
	typedef struct 
		{
		string message_;
		Command::CommandType command_;
		} Ack;
	map<uint16_t,Ack> ackMap_;

	/* unsorted threading case variables! TODO */

	// what we are asking for with the Info command
	uint8_t lastInfoType_;
	uint8_t lastInfoIndex_;

	bool optionsLoaded_, optionsDirty_;
	PacketHandlerState packetState_;


	GadgetLock* lock_;

	ReplyCallback replyCallback_ ;

	/* local functions */


// add item to watch
void AddACKWatch(uint16_t crc, const string & text, Command::CommandType command)
	{
	Ack ack;
	ack.message_ = text;
	ack.command_ = command;
	ackMap_[crc] = ack;
	}
// removes item and returns true if found, else return false and
bool RemoveACKWatch(uint16_t crc, string& text, Command::CommandType & command)
	{
	text = "";
	command = Command::Unknown;
	map<uint16_t,Ack>::iterator iter = ackMap_.find(crc);
	if (ackMap_.end() == iter)
		return false;
	// remove it, and return true
	Ack & ack = iter->second;
	text = ack.message_;
	command = ack.command_;
	ackMap_.erase(iter);
	return true;
	}

// get, release lock for threading
void Lock(void) const
	{
	lock_->Lock();
	}
void Unlock(void) const
	{
	lock_->Unlock();
	}

// todo - export to user? or store internal?
void AddMessageToLog(const std::string & msg) 
	{
	Lock();
	messages_.push_back(msg);
	while (messages_.size() > 1000) // todo - allow setting size
		messages_.pop_front();
	Unlock();
	}

void ErrorMessage(const std::string & msg) 
	{
	errorMessage_ = msg;
	}

	}; // class GadgetImpl

// global function to write bytes from the packet code
void IOWriteByte(void * param, uint8_t byte)
	{ // todo - cleaner?
	GadgetControl::GadgetImpl * pImpl_ = reinterpret_cast<GadgetControl::GadgetImpl*>(param);
	pImpl_->AddByte(byte);
	}


/********************* GadgetControl Section *****************************/
/*                                                                       */
/*                                                                       */
/********************* GadgetControl Section *****************************/

GadgetControl::GadgetControl(GadgetIO & gadgetIO, GadgetLock* lock) : lock_(lock)
	{
	pImpl_ = new GadgetControl::GadgetImpl(gadgetIO, lock);
	if (0 == pImpl_)
		throw runtime_error("Out of memory");
	}

GadgetControl::~GadgetControl(void)
	{
	if (0 != pImpl_)
		delete pImpl_;
	}


/* commands to send to gadget */
void GadgetControl::Login(uint32_t val)
	{
	Lock(); // todo - make lock class that acquires on create, release on destructor, using same lock?!
	pImpl_->Login(val);
	Unlock();
	} // Login


void GadgetControl::Logout(void)
	{
	Lock();
	pImpl_->Logout();
	Unlock();
	}

void GadgetControl::GetFrame(void)
	{
	Lock();
	pImpl_->GetFrame();
	Unlock();
	}

void GadgetControl::SetFrame(const uint8_t * buffer)
	{
	Lock();
	pImpl_->SetFrame(buffer);
	Unlock();
	} // SetFrame

void GadgetControl::FlipFrame(void)
	{
	Lock();
	pImpl_->FlipFrame();
	Unlock();
	} // FlipFrame


void GadgetControl::MaxVisIndex(void)
	{
	Lock();
	pImpl_->MaxVisIndex();
	Unlock();
	}

void GadgetControl::SelectVis(uint8_t vis)
	{
	Lock();
	pImpl_->SelectVis(vis);
	Unlock();		 
	}

void GadgetControl::MaxTranIndex(void)
	{
	Lock();
	pImpl_->MaxTranIndex();
	Unlock();
	}

void GadgetControl::SelectTran(uint8_t trans)
	{
	Lock();
	pImpl_->SelectTran(trans);
	Unlock();
	}

// send Options command, writing data if write = true
// else requesting reading data
void GadgetControl::Options(bool write)
	{
	Lock();
	pImpl_->Options(write);
	Unlock();
	} // Options

void GadgetControl::Version(void)
	{
	Lock();
	pImpl_->Version();
	Unlock();
	}

void GadgetControl::Info(uint8_t type, uint8_t index)
	{
	Lock();
	pImpl_->Info(type,index);
	Unlock();
	}

void GadgetControl::Ping(void)
	{
	Lock();
	pImpl_->Ping();
	Unlock();
	} // Ping

void GadgetControl::Reset(void)
	{
	Lock();
	pImpl_->Reset();
	Unlock();
	} // Reset

// process commands being sent back and forth to the gadget
// call fairly often
void GadgetControl::Update(void)
	{
	// NOTE: - this is on the same thread as the internals , so no locking!
	// TODO - is this correct?
	Lock();
	pImpl_->Update();
	Unlock();
	} // Update

// read/write state of the gadget
GadgetControl::LoginState GadgetControl::GetState(void) const
	{
	Lock();
	LoginState st = pImpl_->GetState();
	Unlock();
	return st;
	}

void GadgetControl::SetState(GadgetControl::LoginState state)
	{
	Lock();
	pImpl_->SetState(state);
	Unlock();
	}

GadgetControl::ByteMode GadgetControl::GetByteMode(void) const
	{
	Lock();
	ByteMode retval = pImpl_->GetByteMode();
	Unlock();
	return retval;
	}

// console bytes are logged, this retrieves a copy of the current log
void GadgetControl::ConsoleLog(std::string & text)
	{
	Lock();
	pImpl_->ConsoleLog(text);
	Unlock();
	}

// only store so many characters (0 for infinite, which is default)
void GadgetControl::ConsoleReset(uint32_t size)
	{
	Lock();
	pImpl_->ConsoleReset(size);
	Unlock();
	}

// clear the console text
void GadgetControl::ConsoleClear(void)
	{
	Lock();
	pImpl_->ConsoleClear();
	Unlock();
	}

// get message from log
// -1 index resets the log
// return true iff valid index
bool GadgetControl::GetMessage(string & message, int index)
	{
	Lock();
	bool ret = pImpl_->GetMessage(message,index);
	Unlock();
	return ret;
	}								  

// return true if there has been an error, get the last error message
// resets error message
bool GadgetControl::Error(string & errMsg)
	{
	Lock();
	bool ret = pImpl_->Error(errMsg);
	Unlock();
	return ret;
	}	   

// Here is the ability to read and write options as a block
// get/set a copy of the options stored in the class
// to get them from the device, use the Options command
bool GadgetControl::GetOptions(HypnoGadget::Options & opts)
	{
	Lock();
	bool ret = pImpl_->GetOptions(opts);
	Unlock();  
	return ret;
	}

void GadgetControl::SetOptions(const HypnoGadget::Options & opts)
	{
	Lock();
	pImpl_->SetOptions(opts);
	Unlock();
	}

std::string GadgetControl::GetDescription(void)
	{
	Lock();
	string str = pImpl_->GetDescription();
	Unlock();
	return str;
	}
std::string GadgetControl::GetDevice(void)
	{
	Lock();
	string str = pImpl_->GetDevice();
	Unlock();
	return str;
	}
std::string GadgetControl::GetCopyright(void)
	{
	Lock();
	string str = pImpl_->GetCopyright();
	Unlock();
	return str;
	}

// get versions after TODO called
void GadgetControl::GetVersion(VersionType type, uint8_t & major, uint8_t & minor)
	{
	Lock();
	pImpl_->GetVersion(type,major,minor);
	Unlock();
	}

// get the count of items loaded
uint8_t GadgetControl::GetCount(InfoType type)
	{
	Lock();
	uint8_t val = pImpl_->GetCount(type);
	Unlock();
	return val;
	}
// get 0 numbered item, return blank string if out of bounds
void GadgetControl::GetName(InfoType type, std::string & name, uint8_t index)
	{
	Lock();
	pImpl_->GetName(type,name,index);
	Unlock();
	}

// return true if a frame ready to read
// resets internal flag when read
// returns pointer to internal buffer and size of buffer
bool GadgetControl::GetFrame(uint8_t ** buffer, int & size)
	{
	Lock();
	bool ret = pImpl_->GetFrame(buffer,size);
	Unlock();
	return ret;
	}

}

void GadgetControl::registerReplyCallback(ReplyCallback callback)
{
	Lock();
	pImpl_->registerReplyCallback(callback) ;
	Unlock();
}



// namespace HypnoGadget

// end - Gadget.cpp
