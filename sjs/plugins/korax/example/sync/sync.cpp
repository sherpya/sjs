/*
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CSync class
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <iostream>
#include <iomanip>
#include <limits>

using namespace util;
using ::std::clog;


CCriticalSection globalCriticalSection;

bool SafeFunction1 (int /*value*/)
{
	// SCOPE_LOCK macro will lock whole function
	SCOPE_LOCK (globalCriticalSection);

	// ...
	// ... this is safe
	// ...

	return true;
}

bool SafeFunction2 (int /*value*/)
{
	static CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

	// ...
	// ... this is safe
	// ...

	return true;
}

bool NOTSafeFunction (int /*value*/)
{
	CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

	// ...
	// ... this is NOT safe
	// ...

	return true;
}

int main (int /*argc*/, char* /*argv[]*/)
{
	// note that cout is redirected to file. You can change it in util_log.cpp

	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using CSync class\n";

	// CriticalSection can make your code threadsafe
	// it is very fast mutex

	CCriticalSection criticalSection;
	for (int i = 0; i < 1000; ++i) {
		criticalSection.Lock ();
		// ...
		// ... this is safe
		// ...
		criticalSection.UnLock ();
	}

	SafeFunction1 (-3);

	SafeFunction2 (37);


	clog << "Press return to quit... ";
	clog.flush();

	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');

	return 0;
}
