// header for options stored in permanent store
#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "hypno/Command.h"
#include "hypno/defines.h"

namespace HypnoGadget {

#define VIS_MAX    60  // max number of visualizations
#define TRANS_MAX  15  // max number of transitions

#ifdef WIN32
#pragma pack(1) // must have everything byte aligned like the PIC does
#define PACKED
#else
#define PACKED __attribute__((__packed__))
#endif

// per visualization options
typedef struct PACKED
	{
	uint8_t minSpeed_, maxSpeed_; // speed, must satisfy 1<=min<=max<=255;
	uint8_t frequency_;           // how often this occurs, relative to others
	uint16_t count_;              // playback counts
	} VisOptions;
// size 5 bytes

// per transition counts
typedef struct PACKED
	{
	uint8_t frequency_;           // how often this occurs, relative to others
	uint16_t count_;              // playback counts
	} TransOptions;
// size 3 bytes

// global system options
typedef struct PACKED
	{
	// this must be first, and goes here to allow transfer directly into location
//	Command::CommandType command_;  // WIN32 enlarges this enum type, making trouble for us
	uint8_t command_; 
	
	// version of the options structure 0-255
	uint8_t optionsVersion_; // loaded one must match or ignored on load
		
	// count number of times this has been stored
	// NOTE: not reset by reset function
	uint16_t storeCount_;
	
	// random number generator state
	uint32_t randState_[16];

	// set to true to indicate easter egg triggered
	uint8_t easterCounter_; // counter for triggering easter egg

	// count of total cube runs - after powered up a few seconds this gets incremented and stored
	uint16_t runCount_;

	// device id for networked devices
	uint16_t deviceId_;   

	// allow user to set, used to secure (somewhat) the 
	// device from attacks. Used in a challenge mode.
	uint32_t challenge_; 

	// when outer user logged in, requires ping to stay alive
	bool requirePing_; 
	uint8_t pingDelay_; // max time in 16th of sec between needed pings
	
	/************ VISUALIZATION ITEMS ***************/
	bool playSequentialVis_;          // sequential or random order vis
	bool useGlobalSpeed_;             // true to override per item speeds
	uint8_t minVisSpeed_,maxVisSpeed_;  // global speeds
	VisOptions visOptions_[VIS_MAX];  // this many
	uint8_t visOrder_[VIS_MAX];         // the order to play them back

	/************ TRANSITION ITEMS ***************/
	bool playSequentialTrans_;        // sequential or random transition order
	TransOptions transOptions_[TRANS_MAX];
	uint8_t transOrder_[TRANS_MAX];     // order to play them in

	} Options;
// total size 1 + 1 + 2 + 16*4 + 1 + 2 + 2 + 4 + 1 + 1 + 1 + 1 + 2 + 60*5 + 60 + 1 + 15*3 + 15
//  = 504 = 0x1F8

extern Options options_;

// loads options from permanent storage if available
// else resets them to default values
void OptionsLoad(void);

// reset to default values, does not store
void OptionsReset(void);

// store options to permanent storage
void OptionsStore(void);

#define OPTIONS_VERSION 1 // current version of options struct
#define OPTIONS_SIZE 504  // currently Options struct must be this size

#ifdef WIN32
#pragma pack() // default packing
#endif

}; // namespace HypnoGadget


#endif // _OPTIONS_H
// end - Options.h

