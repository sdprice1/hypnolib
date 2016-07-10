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

#include <string>
#include <iostream>
#include <memory>
#include <algorithm>

#include "TextAnim.h"
#include "Animator.h"
#include "GadgetQuartz.h"

using namespace HypnoGadget;
using namespace HypnoQuartz;

// Run the gadget demo
// Assumes port is a string like COMx where x is a value
void RunDemo(const std::string& port, const std::string& text)
{
	// 1. Create a gadget
	std::shared_ptr<HypnoQuartz::GadgetQuartz> gadget(std::make_shared<HypnoQuartz::GadgetQuartz>()) ;

	// 2. Open the connection
	if (!gadget->connect(port))
	{
		std::cerr << "Error opening port " << port << std::endl;
		return;
	}

	// 3. Login to the gadget
	if (!gadget->doLogin())
	{
		std::cerr << "Error: could not login to the gadget. Make sure you have the correct COM port." << std::endl ;
		return;
	}

	// 4. While no keys pressed, draw images
	std::shared_ptr<HypnoQuartz::IAnimatorCallback> animCallback(std::make_shared<TextAnim>(gadget, text)) ;

	// do not go faster than about 30 frames per second, 
	// or the device may lock up, so....
	HypnoQuartz::Animator anim ;
	anim.setFrameRate(500) ;
	anim.start(animCallback) ;

	std::cout << "Hit enter to quit....\n";
	int cc = getchar() ;
	(void)cc;
	std::cout << "Stopping....\n";


	anim.stop() ;

	// 5. Logout
	gadget->doLogout();

}

// show the usage for the command line parameters
void ShowUsage(const std::string & programName)
{
	std::cerr << "Usage: " << programName << " <serial device> <text>\n";
	std::cerr << " Where <serial device> is the serial device with the gadget attached,\n";
	std::cerr << " and the text to display.\n";
	std::cerr << "Example: " << programName << " /dev/ttyACM0 \"hello world!\"\n";
} // ShowUsage

// The program starts executing here (obviously...)
int main(int argc, char ** argv)
{
	if (3 != argc)
	{ // not enough command line parameters
		ShowUsage(argv[0]);
		exit(-1);
	}
	
	// get parameters
	std::string port(argv[1]);
	std::string text(argv[2]);

	// finally - run the demo!
	try {
		RunDemo(port, text);
	} catch (...) {
		std::cerr << "Unable to run demo" << std::endl ;
	}

	return 0;
}

