/* 
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CSys class
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <iostream>
#include <iomanip>
#include <limits>

using namespace util;
using ::std::clog;

int main (int /*argc*/, char* /*argv[]*/)
{
	// note that cout is redirected to file. You can change it in util_log.cpp

	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using CSys class\n";

	// CPU SPEED
	clog << "Your CPU speed is " << sys.getCpuFrequency() << sys.getCpuUnits() << "\n";
	clog << "Your CPU speed is " << sys.getCpuFrequency() / 1000000 << "M" << sys.getCpuUnits() << "\n";
	clog << ::std::fixed << ::std::setprecision (1);
	clog << "Your CPU speed is " << sys.getCpuFrequency() / 1000000000.0f << "G" << sys.getCpuUnits() << "\n";

	// Process and Thread id
	clog << "this process id is " << sys.getProcessId() << "\n";
	clog << "this thread  id is " << sys.getThreadId()  << "\n";

	// Time and Date
	clog << "current time is " << sys.getTime (::std::time (0)) << " (" << sys.getTimeFormat() << ")\n";
	clog << "current date is " << sys.getDate (::std::time (0)) << " (" << sys.getDateFormat() << ")\n";

	// Profiler
	int array [100000];
	uint64 cycles = sys.getCycles(); // start
	for (int i = 0; i < 100000; ++i) {
		array [i] = (i*i/(i+1)*(i+1)+i*i-i+i/(i+1)*(i+1))/(i*i/(i+1)*(i+1)+i*i-i+i/(i+1)*(i+1)*i+1);
	}
	cycles = sys.getCycles() - cycles; // stop
	clog << ::std::fixed << ::std::setprecision (5);
	clog << "that strange loop took " << sys.computeMiliSecondsDouble (cycles) << "ms\n";

	clog << "Press return to quit... ";
	clog.flush();

	// More profiler
	cycles = sys.getCycles();
	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');
	cycles = sys.getCycles() - cycles;
	clog << ::std::fixed << ::std::setprecision (1);
	clog << "program waited " << sys.computeSecondsDouble (cycles) << "s until you've pressed enter\n";

	return 0;
}
