/*
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

// header for some helper classes for the HypnoDemo
// Copyright Chris Lomont 2008
#ifndef HYPNODEMO_H
#define HYPNODEMO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Gadget.h"  // the gadget interface
#include "GadgetTTY.h"  // the gadget interface


/* We need two classes to talk with the gadget. The first one is a mutex lock
   for multithreaded applications. Since we are not multithreaded, this class is 
   empty.
   NOTE: Since we are not multithreaded, we need to call gadget.Update() quite 
   often to process bytes. If we were mutithreaded, we'd create a thread to do 
   this loop continuously for us.
*/
class DemoGadgetLock : public HypnoQuartz::GadgetTtyNoLock
{
};

/* The second class handles reading and writing bytes from the serial port.
*/
class DemoGadgetIO : public HypnoQuartz::GadgetTTY
{
}; // DemoGadgetIO

#endif // HYPNODEMO_H
// end - HypnoDemo.h
