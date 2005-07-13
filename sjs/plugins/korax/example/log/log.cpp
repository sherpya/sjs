/* 
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CLog class
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>

using namespace util;
using ::std::clog;
using ::std::cout;
using ::std::endl;

bool SomeFunction (int value)
{
	// LOG value
	::std::ostringstream a;
	a << value;
	// this will log now and on function exit, so you'll know when function returned
	LOG_FUNCTION (a.str());

	// ...
	// ... function body
	// ...
	LOG_INFO ("Log from the inside of the function");
	// ...
	// ... function body
	// ...

	return true;
}

int main (int /*argc*/, char* /*argv[]*/)
{
	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using CLog class\n";

	// simplest logging

	cout << "Entry 1234, log is working as I expected" << endl;

	// these functions will log line number, function name and source file name
	LOG_INFO ("This is info entry");

	LOG_WARNING ("This is warning entry");

	LOG_ERROR ("This is error entry");

	// invoke function logging
	SomeFunction (-3);

	clog << "Look into cout.log, cerr.log and log.xml\n";
	clog << "Press return to quit... ";	
	clog.flush();

	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');

	return 0;
}
