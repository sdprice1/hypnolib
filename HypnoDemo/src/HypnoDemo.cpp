/**
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
 */

// code to demonstrate the interface to the HypnoGadget library
// Copyright 2008 Chris Lomont 
// Visual Studio 2005 C++

// Compile as a console program

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#include "WinEmu.h"

#include "HypnoDemo.h" // include helper classes
#include "Gadget.h"    // include this to access the HypnoCube, HypnoSquare, etc.

#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <error.h>

using namespace std;
using namespace HypnoGadget; // the gadget interface is in this namespace

/* Two set pixel functions to demonstrate how to 
   put a pixel in the buffer for the cube or square
   */

// Set a pixel in the square or cube buffer
// red, green, and blue are values in 0-255.
// i,j are 0-7 coordinates
// buffer is a 96 byte buffer to be sent to the gadget
void SetPixelSquare(
		int i, int j,        // coordinates
		unsigned char red,   // colors
		unsigned char green, // colors
		unsigned char blue,  // colors
		uint8 * buffer       // where to draw
		)
	{
	// each pixel on the gadget is RGB values 0-15, and packed
	// so each three bytes contains two pixels as in R1G1 B1R2 G2B2
	if ((i < 0) || (7 < i) || (j < 0) || (7 < j))
		return; // nothing to do

	// find first byte to set
	buffer += 12*j;      // 3/2 bytes per pixel * 8 pixels per row = 12 bytes per row
	buffer +=  3*(i>>1); // 3/2 bytes per pixel
	if (i&1) 
		{ // odd pixel, ..R2 G2B2 above
		++buffer;            // next byte
		*buffer &= 0xF0;     // mask out low bits
		*buffer |= (red>>4); // set high red nibble to low part of byte
		++buffer;            // next byte
		// set high green nibble and high blue nibble
		*buffer  = (green&0xF0)|(blue>>4); 
		}
	else
		{ // even pixel, R1G1 B2.. above
		// set high red nibble and high green nibble
		*buffer  = (red&0xF0)|(green>>4); 
		++buffer;            // next byte
		*buffer &= 0x0F;     // mask out high bits
		*buffer |= (blue&0xF0); // set high blue nibble to high part of byte
		}
	} // SetPixelSquare

// See above comments
void SetPixelCube(
		int i, int j, int k, // coordinates
		unsigned char red,   // colors
		unsigned char green, // colors
		unsigned char blue,  // colors
		uint8 * buffer       // where to draw
		)
	{
	// each pixel on the gadget is RGB values 0-15, and packed
	// so each three bytes contains two pixels as in R1G1 B1R2 G2B2
	if ((i < 0) || (3 < i) || (j < 0) || (3 < j) || (k < 0) || (3 < k))
		return; // nothing to do

	j = 3-j; // reverse j to get right hand coord system used in cube

	buffer += k*24; // 3/2 bytes per pixel * 16 pixels per level  = 24 bytes
	buffer += i*6;  // 3/2 bytes per pixel * 4  pixels per column =  6 bytes
	buffer += 3*(j>>1);   // 3/2 bytes per pixel

	if (j&1)
		{ // odd pixel, ..R2 G2B2 above
		++buffer;            // next byte
		*buffer &= 0xF0;     // mask out low bits
		*buffer |= (red>>4); // set high red nibble to low part of byte
		++buffer;            // next byte
		// set high green nibble and high blue nibble
		*buffer  = (green&0xF0)|(blue>>4); 
		}
	else
		{ // even pixel, R1G1 B2.. above
		// set high red nibble and high green nibble
		*buffer  = (red&0xF0)|(green>>4); 
		++buffer;            // next byte
		*buffer &= 0x0F;     // mask out high bits
		*buffer |= (blue&0xF0); // set high blue nibble to high part of byte
		}			   
	} // SetPixelCube

// Draw a frame of animation on the gadget
void DrawFrame(GadgetControl & gadget, const string & device)
	{
	static int pos = 0; // position of pixel 0-63 - this drives this animation
	
	uint8 image[96]; // RGB buffer, 4 bits per color, packed

	// clear screen by setting all values to 0
	memset(image,0,sizeof(image));

	// draw a scrolling random colored pixel
	if ("CUBE" == device)
		{  // cube device we scroll on x,y,z
		int x,y,z;  // coordinates
		x = pos/16; // convert pos 0-63 to x,y,z in [0,3]x[0,3]x[0,3]
		y = (pos/4)&3;
		z = pos&3;
#if 1
		SetPixelCube(x,y,z,rand(),rand(),rand(),image);
#else
		// draw testing axis
		for (int x = 0; x < 4; ++x)
			SetPixelCube(x,0,0,255,0,0,image); // red x axis
		for (int y = 0; y < 4; ++y)
			SetPixelCube(0,y,0,0,255,0,image); // green y axis
		for (int z = 0; z < 4; ++z)
			SetPixelCube(0,0,z,0,0,255,image); // blue z axis
#endif
		}
	else if ("SQUARE" == device)
		{   // square device we scroll on x,y
		int x,y;    // coordinates
		x = pos&7;  // convert pos 0-63 to x,y in [0,7]x[0,7]
		y = pos/8;
#if 1
		SetPixelSquare(x,y,rand(),rand(),rand(),image);
#else
		// draw testing axis
		for (int x = 0; x < 8; ++x)
			SetPixelSquare(x,0,255,0,0,image); // red x axis
		for (int y = 0; y < 8; ++y)
			SetPixelSquare(0,y,0,255,0,image); // green y axis
#endif
		}

	// next pixel for next frame of animation
	pos = (pos+1)&63; // count 0-63 and repeat

	// send the image
	gadget.SetFrame(image);

	// show the image
	gadget.FlipFrame();

	} // DrawFrame

// Runs the Gadget update and shows any messages/errors
void Update(GadgetControl & gadget)
{
	gadget.Update();

	// Check for messages
	std::string text ;
	gadget.ConsoleLog(text);
	gadget.ConsoleClear() ;
	if (!text.empty())
	{
		std::cout << text << std::endl ;
	}

	// Check for errors
	std::string error ;
	if (gadget.Error(error))
	{
		std::cerr << "ERROR: " << error << std::endl ;
	}

}



// attempt to login to a HypnoGadget.
// Return true on success, else return false
bool Login(GadgetControl & gadget)
{
	for (unsigned retry=1; retry <= 10; ++retry)
	{
		gadget.Login();
		for (int pos = 0; pos < 100; ++pos)
		{
			Update(gadget); // we need to do this ourselves since we are not multithreaded
			if (GadgetControl::LoggedIn == gadget.GetState())
				return true;
			Sleep(5);
		}

		// failed to login - logout and retry
		gadget.Logout();
		Sleep(100);
		Update(gadget); // we need to do this ourselves since we are not multithreaded
	}

	return false;
} // Login

// Run the gadget demo
// Assumes port is a string like COMx where x is a value
void RunDemo(const string & port, const string & device)
	{
	// Two classes we need to feed to the gadget control
	DemoGadgetIO     ioObj;	  // handles COM bytes
	DemoGadgetLock   lockObj; // handles threads

	// 1. Create a gadget
	GadgetControl gadget(ioObj, &lockObj);

	// 2. Open the connection
	if (false == ioObj.waitOpen(port))
		{
		cerr << "Error opening port " << port << endl;
		return;
		}

	// 3. Login to the gadget
	if (false == Login(gadget))
		{
		cerr << "Error: could not login to the gadget. Make sure you have the correct COM port.";
		ioObj.Close();
		return;
		}

	// 4. While no keys pressed, draw images


	// do not go faster than about 30 frames per second, 
	// or the device may lock up, so....
	unsigned long delay = 1000/30; // milliseconds per frame

	cout << "Ctrl-C to quit....\n";
//	while (!_kbhit())
	while (true)
		{
		// Draw a frame of the demo
		DrawFrame(gadget,device);

		// Loop, processing read and written bytes
		// until time for next frame
		unsigned long start = timeGetTime();
		while (timeGetTime()-start < delay)
			{
			// be sure to call this often to process serial bytes
			Update(gadget);
			Sleep(1);
			}
		} // loop until key hit

//	while (_kbhit())
//		_getch(); // eat any keypresses

	// 5. Logout
	gadget.Logout();  // we assume it logs out
	for (int pos = 0; pos < 10; ++pos)
		{
		Update(gadget);  // we must call this to process bytes
		Sleep(10);        // slight delay for 
		}

	// 6. Close the connection
	ioObj.Close();
	Sleep(100);       // slight delay
	} // RunDemo

// show the usage for the command line parameters
void ShowUsage(const string & programName)
	{
	cerr << "Usage: " << programName << " <serial device> [cube|square]\n";
	cerr << " Where <serial device> is the serial device with the gadget attached,\n";
	cerr << " and cube or square is the type of Hypno device attached.\n";
	cerr << "Example: " << programName << " /dev/ttyUSB0 cube\n";
	cerr << "Example: " << programName << " /dev/ttyUSB0 square\n";
	} // ShowUsage

// The program starts executing here (obviously...)
int main(int argc, char ** argv)
{
	cout << "Visit www.HypnoCube.com or www.HypnoSquare.com for updates!\n";
	cout << "HypnoDemo version 1.0, March 2008, by Chris Lomont\n";
	cout << "HypnoDemo version 1.1, May 2016, Linux version by Steve Price\n\n";
	if (3 != argc)
	{ // not enough command line parameters
		ShowUsage(argv[0]);
		exit(-1);
	}
	
	// get parameters
	string port(argv[1]);
	string device(argv[2]);

	// clean them
	transform(device.begin(), device.end(), device.begin(), [](const char cc)->char {return toupper(cc);} ); // uppercase

	// sanity check the arguments
	bool validDevice = ("CUBE" == device) || ("SQUARE" == device); // test accepted devices

//	if ((4 != port.length()) || ("COM" != port.substr(0,3)) ||
//		(false == isdigit(port[3])) || (false == validDevice))
//	{ // wrong command line parameters
//		ShowUsage(argv[0]);
//		exit(-2);
//	}

	if (false == validDevice)
	{ // wrong command line parameters
		ShowUsage(argv[0]);
		exit(-2);
	}


	// finally - run the demo!
	try {
		RunDemo(port,device);
	} catch (...) {
		std::cerr << "Unable to run demo" << std::endl ;
	}

	return 0;
} // main

// end - HypnoDemo.cpp
