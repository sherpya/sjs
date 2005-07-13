/* 
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CPar class
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
	clog << "Example on using CPar class\n";

	CPar par;

	// we will load example.xml
	// you can load xml also from any stream (e.g. keyboard)
	if (!par.loadXml ("example.xml")) {
		clog << "Example xml not found";
		clog.flush();
		std::cin.ignore (::std::numeric_limits<int>::max(), '\n');
		return 0;
	}
	
	// list xml
	par.List (clog);

	// query in xml
	clog << "First bot in xml has name \"" << par.getValue ("bot/name") << "\"\n";
	clog << "Bot Sam is version " << par.getValue ("bot/name='Sam'^meta:version") << "\n";
	clog << "non-dedicated server has IP address " << par.getValue ("server/type:dedicated='no'^address") << "\n";
	clog << "we will change Sauron's easy skill from " << par.getValue ("bot/name=Sauron^skill/easy") << " to 200\n";
	par.setValue<int> ("bot/name=Sauron^skill/easy", 2000);

	// you can do advanced query with & | operators
	// you can insert new and erase existing elements
	// you can move/swap/copy existing elements
	// see util_par.h for more functions or read help...

	clog << "Press return to quit... ";
	clog.flush();
	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');

	return 0;
}
