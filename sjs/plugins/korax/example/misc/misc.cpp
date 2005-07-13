/* 
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using misc utilities
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <ctime>
#include <iostream>
#include <iomanip>
#include <limits>

using namespace util;
using ::std::clog;
using ::std::string;

int main (int /*argc*/, char* /*argv[]*/)
{
	// note that cout is redirected to file. You can change it in util_log.cpp

	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using misc utilities\n";

	// conversions from string to somethign else
	string testString = "3.1415";
	int testInt = fromString<int>(testString); // 3
	float testFloat = fromString<float>(testString); // 3.1415f

	// conversions to string
	testString = toString (testInt) + string (" ") + toString (testFloat);
	clog << testString << "\n";

	idum_ = static_cast<long>(::std::time (0)); // initialize random number generator
	// random numbers
	clog << "random float [0,1[ " << RandomFloat () << "\n";
	clog << "random long  [1,3] " << RandomLong (1, 3) << "\n";

	// degrees and radians conversions
	clog << "30 degrees is " << toRadians (30.0f) << " radians\n";
	clog << "3.1415 radians is " << toDegrees (3.1415f) << " degrees\n";

	// max and min
	clog << "max : " << ::util::max (1,2,3) << "\n";
	clog << "min : " << ::util::min (1,2,3) << "\n";

	// float equality and test for zero
	bool result = isEqual (testFloat, 3.1415f);
	clog << "equal : " << (result ? "YES" : "NO") << "\n";
	result = isZero (testFloat);
	clog << "is zero : " << (result ? "YES" : "NO") << "\n";

	clog << "Press return to quit... ";
	clog.flush();
	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');

	return 0;
}
