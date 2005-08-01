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

You should have received a copy of the GNU General Public License
along with koraX's utils in file gpl.txt; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

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

See header file for more information

*/

// C++ standard (ISO/IEC 14882:2003)
#include <cstdio>	// perror
#include <sstream>	// ostringstream
#include <iomanip>	// manipulators (setw(), setfill(), precision())

// koraX's utils
#include "util_sys.h"

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using ::std::string;

//-------------------------------------------------------------------------------

namespace util {

// create instance of CSys
CSys &sys = CSys::getInstance();

//-------------------------------------------------------------------------------

// constructors execution takes at least :
// 200ms under Windows
// 400ms under Linux
// we need some time to compute CPU speed. 
// SUS (POSIX) timer is somewhat less precise so more time is needed
CSys::CSys ()
:cpuFrequency_(0),overhead_(0)
{
	computeCpuFrequency();
}

CSys::~CSys ()
{
}

//-------------------------------------------------------------------------------

CSys& CSys::getInstance ()
{
	static CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

	static CSys lSys;
	return lSys;
}

// function will detect CPU speed and getCycles overhead
void CSys::computeCpuFrequency (const char speed)
{
	SCOPE_LOCK (criticalSection_);

	cpuFrequency_ = DetectCpuSpeed (speed);
	overhead_ = getCycles();
	overhead_ = getCycles() - overhead_;
}

// return date in YYYY-MM-DD (International Standard ISO 8601)
string CSys::getDate (::std::time_t cas)
{
	if (cas == 0)
		cas = ::std::time (0);
	::std::tm *lDate = ::std::localtime (&cas);
	::std::ostringstream a; 
	a 	<< lDate->tm_year + 1900 << "-" 
		<< ::std::setw (2) << ::std::setfill ('0') << lDate->tm_mon + 1 << "-" 
		<< ::std::setw (2) << ::std::setfill ('0') << lDate->tm_mday;
	return a.str();
}

// return time in hh:mm:ss (International Standard ISO 8601)
string CSys::getTime (::std::time_t cas)
{
	if (cas == 0)
		cas = ::std::time (0);
	::std::tm *lDate = ::std::localtime (&cas);
	::std::ostringstream a;
	a	<< ::std::setw (2) << ::std::setfill ('0') << lDate->tm_hour << ":" 
		<< ::std::setw (2) << ::std::setfill ('0') << lDate->tm_min  << ":" 
		<< ::std::setw (2) << ::std::setfill ('0') << lDate->tm_sec;
	return a.str();
}

// for old compilers
// #define rdtsc __asm __emit 0fh __asm __emit 031h
// #define cpuid __asm __emit 0fh __asm __emit 0a2h

// this function returns number of CPU cycles since reset
uint64 CSys::getCycles ()
{
	// we do not check if rdtsc instructions is avaiable
	// because this function is as fast as possible, yeah

	// there is no need to "synchronize" CPU with cpuid instruction
	// because this function is for measuring long periods, o_O

	// using something else than local variable could be faster
	// (like class member or passing it as paramter by reference), but 
	// my asm skills are not enough for this task :)

	// With local variable, overhead is 30 instructions (WXP, AMD 2GHz, MSVC 2003 .NET)

	uint64 cycles;

#if ( defined __i386 && defined __unix__ ) || ( defined _WIN32 && defined __GNUG__ )
	__asm__ __volatile__ (
		"rdtsc\n\t"
	: "=A" (cycles)
	: );
#endif

#if defined _WIN32 && !defined __GNUG__
	__asm { 
		push eax
		push edx
		rdtsc 
		mov dword ptr [cycles],eax 
		mov dword ptr [cycles + 4],edx 
		pop edx
		pop eax
	}
#endif

	return cycles;
}

//-- private functions ----------------------------------------------------------

// don't look further, here be dragons

// this function will detect CPU speed, if possible.
// It will detect CPU speed twice and take lower value.
// This is because high precision timers are not so high
uint64 CSys::DetectCpuSpeed (const char speed)
{
	// l33t haXz0r skillz0r

	uint64 frequency = 0;

#if defined __i386 && defined __unix__ // UNIX
	// see if we have cpuid instruction
	int isCpuidPresent;
	__asm__ __volatile__ (
#if defined PIC			
		"push %%ebx\n\t"
#endif
		"pushfl\n\t"
		"pushfl\n\t"
		"popl %%eax\n\t"
		"movl %%eax,%%ebx\n\t"
		"xorl $0x200000,%%eax\n\t"
		"pushl %%eax\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %%eax\n\t"
		"xorl %%ebx,%%eax\n\t"
		"je NO_CPUID%=\n\t"
		"movl $1,%0 \n\t"
		"jmp DONE_CPUID%=\n\t"
		"NO_CPUID%=:\n\t"
		"movl $0,%0 \n\t"
		"DONE_CPUID%=:"
		"popfl\n\t"
#if defined PIC
		"pop %%ebx"
		: "=g" (isCpuidPresent): : "%eax");
#else
		: "=g" (isCpuidPresent): : "%eax", "%ebx");
#endif
	if (isCpuidPresent == 0)
		return 0;

	// see if we have rdtsc instruction
	int isRdtscPresent;
	__asm__ __volatile__ (
#if defined PIC
		"push %%ebx\n\t"
#endif
		"movl $1,%%eax\n\t"
		"cpuid\n\t"
		"testl $0x10,%%edx\n\t"
		"jz NO_RDTSC%=\n\t"
		"movl $1,%0\n\t"
		"jmp DONE_RDTSC%=\n\t"
		"NO_RDTSC%=:\n\t"
		"movl $0,%0\n\t"
		"DONE_RDTSC%=:\n\t"
#if defined PIC
		"pop %%ebx"
		: "=g" (isRdtscPresent): : "%eax", "%ecx", "%edx");
#else
		: "=g" (isRdtscPresent): : "%eax", "%ebx", "%ecx", "%edx");
#endif
	if (isRdtscPresent == 0)
		return 0;

	uint64		beginCycles;
	uint64		endCycles;
	uint64		overhead;
	uint64		tempFrequency;
	int			result;
	timespec	waitTime;

	waitTime.tv_sec  = 0;
	waitTime.tv_nsec = 1000000000ul / speed;
//	useconds_t uSeconds = 1000000ul / speed; // for usleep

	for (int i = 0; i < 2; ++i) {
		// warming up and overhead
		__asm__ __volatile__ (
#if defined PIC
			"push %%ebx\n\t"
#endif
			"xor %%eax,%%eax\n\t"
			"cpuid\n\t"
			"rdtsc\n\t"
			"xor %%eax,%%eax\n\t"
			"cpuid\n\t"
			"rdtsc\n\t"
			"xor %%eax,%%eax\n\t"
			"cpuid\n\t"
			"rdtsc\n\t"
#if defined PIC
			"pop %%ebx"
		: "=A" (overhead) : : "%ecx");
#else
		: "=A" (overhead) : : "%ebx", "%ecx");
#endif

		// starting
		__asm__ __volatile__ (
#if defined PIC
			"push %%ebx\n\t"
#endif
			"xor %%eax,%%eax\n\t"
			"cpuid\n\t"
			"rdtsc\n\t"
#if defined PIC
			"pop %%ebx"
		: "=A" (beginCycles) : : "%ecx");
#else
		: "=A" (beginCycles) : : "%ebx", "%ecx");
#endif

		// wait loop
		result = nanosleep (&waitTime, 0);
//		result = usleep (uSeconds);
//		result = sleep (10 / speed);

		// end
		__asm__ __volatile__ (
#if defined PIC
			"push %%ebx\n\t"
#endif
			"xor %%eax,%%eax\n\t"
			"cpuid\n\t"
			"rdtsc\n\t"
#if defined PIC
			"pop %%ebx"
		: "=A" (endCycles) : : "%ecx");
#else
		: "=A" (endCycles) : : "%ebx", "%ecx");
#endif

		if (result != 0) {
		::std::perror ("CSys::DetectCpuSpeed() - Timer");
			return 0;
		}

		// compute frequency
		if (frequency == 0) {
			frequency = endCycles - beginCycles - beginCycles + overhead;
			frequency *= speed;
//			frequency /= 10; // for sleep only
		} else {
			tempFrequency = endCycles - beginCycles - beginCycles + overhead;
			tempFrequency *= speed;
//			tempFrequency /= 10; // for sleep only
			if(frequency > tempFrequency)
				frequency = tempFrequency;
		}
	}
#endif // UNIX

#if defined _WIN32 && !defined __GNUG__ // MSVC
	// see if we have cpuid instruction
	bool isCpuidPresent;
	__asm {
		pushad
		pushfd
		pop eax
		mov ebx, eax
		xor eax, 0x200000
		push eax
		popfd
		pushfd
		pop eax
		xor eax,ebx
		je NO_CPUID
		mov isCpuidPresent, 1 
		jmp DONE_CPUID
		NO_CPUID:
		mov isCpuidPresent, 0
		DONE_CPUID:
		popad
	}
	if (!isCpuidPresent)
		return 0;

	//see if we have rdtsc instruction
	bool isRdtscPresent;
	__asm {
		pushad
		mov  eax, 1
		cpuid
		test edx, 0x10	
		jz NO_RDTSC
		mov isRdtscPresent, 1
		jmp DONE_RDTSC
		NO_RDTSC:
		mov isRdtscPresent, 0
		DONE_RDTSC:
		popad
	}
	if (!isRdtscPresent)
		return 0;

	// use QueryPerformanceCounter in WIN32 environment
	LARGE_INTEGER qpcFrequency, beginTime, endTime, beginCycles, endCycles, overhead;
	uint64 tempFrequency;
	if (::QueryPerformanceFrequency (&qpcFrequency) == TRUE) {
		for (int i = 0; i < 2; ++i) {
			// set begining time
			::QueryPerformanceCounter (&beginTime);
			endTime.QuadPart = beginTime.QuadPart + qpcFrequency.QuadPart / speed;
			// warming up and measure
			__asm 
			{ 
				pushad
				xor eax, eax
				cpuid
				rdtsc 
				mov beginCycles.LowPart, eax 
				mov beginCycles.HighPart, edx 
				xor eax,eax
				cpuid
				rdtsc 
				mov beginCycles.LowPart, eax 
				mov beginCycles.HighPart, edx 
				xor eax, eax
				cpuid
				rdtsc 
				mov overhead.LowPart, eax 
				mov overhead.HighPart, edx 
				popad
				pushad
				xor eax, eax
				cpuid
				rdtsc 
				mov beginCycles.LowPart, eax 
				mov beginCycles.HighPart, edx 
				popad
			}
			// wait loop, freeeeeze everything, he he he
			do
				::QueryPerformanceCounter (&beginTime);
			while (beginTime.QuadPart < endTime.QuadPart);
			__asm 
			{ 
				pushad
				xor eax,eax
				cpuid
				rdtsc 
				mov endCycles.LowPart, eax 
				mov endCycles.HighPart, edx 
				popad
			}
			// compute frequency
			if (frequency == 0)
				frequency = (static_cast<uint64>(endCycles.QuadPart - beginCycles.QuadPart - beginCycles.QuadPart + overhead.QuadPart)) * speed;
			else {
				tempFrequency = (static_cast<uint64>(endCycles.QuadPart - beginCycles.QuadPart - beginCycles.QuadPart + overhead.QuadPart)) * speed;
				if(frequency > tempFrequency)
					frequency = tempFrequency;
			}
		}
	} else {
		// no high performance counter, ye olde pc
		return 0;
	}
#endif // MSVC

	
#if defined _WIN32 && defined __GNUG__ // MinGW
	// see if we have cpuid instruction
	int isCpuidPresent;
	__asm__ __volatile__ (
		"pushfl\n\t"
		"pushfl\n\t"
		"popl %%eax\n\t"
		"movl %%eax,%%ebx\n\t"
		"xorl $0x200000,%%eax\n\t"
		"pushl %%eax\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %%eax\n\t"
		"xorl %%ebx,%%eax\n\t"
		"je NO_CPUID%=\n\t"
		"movl $1,%0 \n\t"
		"jmp DONE_CPUID%=\n\t"
		"NO_CPUID%=:\n\t"
		"movl $0,%0 \n\t"
		"DONE_CPUID%=:"
		"popfl\n\t"
		: "=g" (isCpuidPresent): : "%eax", "%ebx"); 
	if (isCpuidPresent == 0)
		return 0;

	//see if we have rdtsc instruction
	int isRdtscPresent;
	__asm__ __volatile__ (
		"movl $1,%%eax\n\t"
		"cpuid\n\t"
		"testl $0x10,%%edx\n\t"
		"jz NO_RDTSC%=\n\t"
		"movl $1,%0\n\t"
		"jmp DONE_RDTSC%=\n\t"
		"NO_RDTSC%=:\n\t"
		"movl $0,%0\n\t"
		"DONE_RDTSC%=:\n\t"
		: "=g" (isRdtscPresent): : "%eax", "%ebx", "%ecx", "%edx");
	if (isRdtscPresent == 0)
		return 0;

	// use QueryPerformanceCounter in WIN32 environment
	LARGE_INTEGER qpcFrequency, beginTime, endTime, beginCycles, endCycles, overhead;
	uint64 tempFrequency;
	if (::QueryPerformanceFrequency (&qpcFrequency) == TRUE) {
		for(int i = 0; i < 2; ++i) {
			// set begining time
			::QueryPerformanceCounter (&beginTime);
			endTime.QuadPart = beginTime.QuadPart + qpcFrequency.QuadPart / speed;
			// warming up and measure
			__asm__ __volatile__ (
				"xor %%eax,%%eax\n\t"
				"cpuid\n\t"
				"rdtsc\n\t"
				"xor %%eax,%%eax\n\t"
				"cpuid\n\t"
				"rdtsc\n\t"
				"xor %%eax,%%eax\n\t"
				"cpuid\n\t"
				"rdtsc\n\t"
			: "=A" (overhead)
			:
			: "%ebx", "%ecx");
			// starting
			__asm__ __volatile__ (
				"xor %%eax,%%eax\n\t"
				"cpuid\n\t"
				"rdtsc\n\t"
			: "=A" (beginCycles)
			:
			: "%ebx", "%ecx");
			// wait loop, freeeeeze everything, he he he
			do
				::QueryPerformanceCounter (&beginTime);
			while (beginTime.QuadPart < endTime.QuadPart);
			// end
			__asm__ __volatile__ (
				"xor %%eax,%%eax\n\t"
				"cpuid\n\t"
				"rdtsc\n\t"
			: "=A" (endCycles)
			:
			: "%ebx", "%ecx");
			// compute frequency
			if (frequency == 0)
				frequency = (static_cast<uint64>(endCycles.QuadPart - beginCycles.QuadPart - beginCycles.QuadPart + overhead.QuadPart)) * speed;
			else {
				tempFrequency = (static_cast<uint64>(endCycles.QuadPart - beginCycles.QuadPart - beginCycles.QuadPart + overhead.QuadPart)) * speed;
				if (frequency > tempFrequency)
					frequency = tempFrequency;
			}
		}
	} else {
		// no high performance counter, ye olde pc
		return 0;
	}
#endif // MinGW

	return frequency;
}

} // namespace util
