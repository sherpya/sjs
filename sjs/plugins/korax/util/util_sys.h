/* 

Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com

This file is part of koraX's utils.

koraX's utils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

koraX's utils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
In addition, as a special exception, the author gives permission to
link the code of this program with the Half-Life Game Engine ("HL
Engine") and Modified Game Libraries ("MODs") developed by Valve,
L.L.C ("Valve"). You must obey the GNU General Public License in all
respects for all of the code used other than the HL Engine and MODs
from Valve. If you modify this file, you may extend this exception
to your version of the file, but you are not obligated to do so. If
you do not wish to do so, delete this exception statement from your
version.

Read documentation in /doc folder for help, reference, and examples

Name       : CSys class
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : singleton class
Threadsafe : yes
Depends on : koraX's synchronization classes, miscellaneous
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_SYS
#define H_KORAX_UTIL_SYS

// C++ standard (ISO/IEC 14882:2003)
#include <ctime>			// time(), localtime(), time_t, tm, nanosleep()
#include <string>			// string

// Platform dependent
#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
#	include <windows.h>		// GetCurrentProcessId(), GetCurrentThreadId()
//#	include <process.h>		// _getpid()
// why not _getpid() ? There is no _get___() replacement for GetCurrentThreadId() 
// and GetCurrentProcessId() is subjectively faster
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <pthread.h>		// pthread_self() 
#	include <sys/types.h>	// pthread_t, pid_t, useconds_t
#	include <unistd.h>		// getpid(), usleep(), sleep()
#endif

// koraX's utils
#include "util_misc.h"
#include "util_sync.h"


//-------------------------------------------------------------------------------

namespace util {

#ifdef _WIN32
#	define KORAX_UTIL_SYS_DEFSPEED	10	// will do 100ms waiting in DetectCPUSpeed
#else
#	define KORAX_UTIL_SYS_DEFSPEED	5	// will do 200ms waiting in DetectCPUSpeed
#endif

	//-------------------------------------------------------------------------------

	class CSys
	{
	public: // Interface
			uint64			operator()					();							// alias for getCycles()
	static	CSys&			getInstance					();							// get CSys object

			uint64			getCpuFrequency				();							// CPU frequency in Hz
			uint64			getOverhead					();							// getCycles overhead
	static	uint64			getCycles					();							// CPU cycles since restart/power up
	static	pid_t			getProcessId				();							// ID of current process
	static	pthread_t		getThreadId					();							// ID of current thread
	static	::std::string	getDate						(::std::time_t cas = 0);	// YYYY-MM-DD (International Standard ISO 8601)
	static	::std::string	getTime						(::std::time_t cas = 0);	// hh:mm:ss   (International Standard ISO 8601)

	static	::std::string	getCpuUnits					();							// "Hz"
	static	::std::string	getOverheadUnits			();							// "cycles"
	static	::std::string	getCyclesUnits				();							// "cycles"
	static	::std::string	getDateFormat				();							// "YYYY-MM-DD"
	static	::std::string	getTimeFormat				();							// "hh:mm:ss"

			void			computeCpuFrequency			(const char speed = KORAX_UTIL_SYS_DEFSPEED);
			// good for long  intervals (> 100)
			uint64			computeSeconds				(const uint64 cycles);		// convert cycles to seconds
			uint64			computeMiliSeconds			(const uint64 cycles);		// convert cycles to miliseconds
			// good for small intervals (< 100)
			double			computeSecondsDouble		(const uint64 cycles);		// convert cycles to seconds
			double			computeMiliSecondsDouble	(const uint64 cycles);		// convert cycles to miliseconds

	private: // Implementation
		CSys (const CSys&);				// not supported
		CSys& operator= (const CSys&);	// not supported
		 CSys();
		~CSys();

		uint64				DetectCpuSpeed				(const char speed);

		uint64				cpuFrequency_;
		uint64				overhead_;
		CCriticalSection	criticalSection_;
	};

	//-------------------------------------------------------------------------------

	extern CSys &sys; // the only instance of CSys

	//-- inline functions -----------------------------------------------------------

	inline uint64 CSys::operator() ()
	{
		return getCycles();
	}

	inline uint64 CSys::getCpuFrequency ()
	{
		return cpuFrequency_;
	}

	inline ::std::string CSys::getCpuUnits ()
	{
		return "Hz";
	}

	inline uint64 CSys::getOverhead ()
	{
		return overhead_;
	}

	inline ::std::string CSys::getOverheadUnits ()
	{
		return "cycles";
	}

	inline ::std::string CSys::getCyclesUnits ()
	{
		return "cycles";
	}

	inline pid_t CSys::getProcessId ()
	{
#ifdef _WIN32
		return ::GetCurrentProcessId(); // faster than _getpid()
//		return ::_getpid();
#elif defined __linux__
		return ::getpid();
#else
		return 0;
#endif
	}

	inline pthread_t CSys::getThreadId ()
	{
#ifdef _WIN32
		return ::GetCurrentThreadId();
#elif defined __linux__
		return ::pthread_self();
#else
		return 0;
#endif
	}

	inline ::std::string CSys::getDateFormat ()
	{
		return "YYYY-MM-DD";
	}

	inline ::std::string CSys::getTimeFormat ()
	{
		return "hh:mm:ss";
	}

	inline uint64 CSys::computeSeconds (const uint64 cycles)
	{
		return (cpuFrequency_ == 0) ? 0 : (cycles / cpuFrequency_);
	}

	inline uint64 CSys::computeMiliSeconds (const uint64 cycles)
	{
		return (cpuFrequency_ == 0) ? 0 : ((cycles * 1000ul) / cpuFrequency_);
	}

	inline double CSys::computeSecondsDouble (const uint64 cycles)
	{
		if (cpuFrequency_ == 0)
			return 0;
		return  (cycles / cpuFrequency_) + (static_cast<double>(cycles % cpuFrequency_) / cpuFrequency_);
	}

	inline double CSys::computeMiliSecondsDouble (const uint64 cycles)
	{
		if (cpuFrequency_ == 0)
			return 0;
		return  ((cycles * 1000ul) / cpuFrequency_) + (static_cast<double>((cycles * 1000ul) % cpuFrequency_) / cpuFrequency_);
	}

} // namespace util

#endif // H_KORAX_UTIL_SYS
