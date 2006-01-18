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
License along with koraX's utils in file gpl.txt; if not, write to the
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

Name       : Synchronization classes
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : classes
Threadsafe : yes
Depends on : nothing
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_SYNC
#define H_KORAX_UTIL_SYNC

// C++ standard (ISO/IEC 14882:2003)
#include <cstdio>			// perror()
#include <stdexcept>		// logic_error, runtime_error

#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
/*
// uncomment following 4 lines, if you want to have TryLock in WIN32/64 environment.
// You must also include these 4 lines in every file which includes some
// Windows, and you must place it before these #include statements
// WARNING : your program won't run on Windows 98/95, only Windows NT4.0/2k3/XP and higher
#	ifdef _WIN32_WINNT
#		undef _WIN32_WINNT
#	endif
#	define _WIN32_WINNT 0x0400
//*/
// Platform dependent
#	include <windows.h>		// CRITICAL_SECTION, *CriticalSection()
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <pthread.h>		// PTHREAD_MUTEX_RECURSIVE, pthread_mutexattr_*(), pthread_mutex_*()
#	include <sys/types.h>	// pthread_mutex_t, pthread_mutexattr_t
#endif

// use this to make your function thread-safe
#define SCOPE_LOCK(a) util::CScopeLock __korax_sync_temp_lock(a)

//-------------------------------------------------------------------------------

namespace util {

	//-------------------------------------------------------------------------------

	class CCriticalSection
	{
	public: // Interface
		 CCriticalSection();	// constructor
		~CCriticalSection();	// destructor

		void Lock();			// Lock critical section
		bool TryLock();			// Same as Lock(), but won't wait if already locked
		void UnLock();			// Unlock critical section

	private: // Implementation
		CCriticalSection (const CCriticalSection&);				// not supported
		CCriticalSection& operator= (const CCriticalSection&);	// not supported

#ifdef _WIN32
		CRITICAL_SECTION handler_;
#elif defined __unix__
		pthread_mutex_t handler_;
		pthread_mutexattr_t type_;
#endif
	};

	class CScopeLock
	{
	public: // Interface
		CScopeLock (CCriticalSection &handler);		// constructor
		~CScopeLock();								// destructor

	private: // Implementation
		CScopeLock();								// not supported
		CScopeLock (const CScopeLock&);				// not supported
		CScopeLock& operator= (const CScopeLock&);	// not supported

		CCriticalSection &handler_;
	};


	//-- inline functions  ----------------------------------------------------------

	inline void CCriticalSection::Lock()
	{
#ifdef _WIN32
		::EnterCriticalSection (&handler_);
#elif defined __unix__
		if (::pthread_mutex_lock (&handler_) != 0) {
			::std::perror ("CCriticalSection::Lock()");
			throw ::std::runtime_error ("CCriticalSection::Lock()");
		}
#endif
	}

	inline bool CCriticalSection::TryLock()
	{
#ifdef _WIN32
		// If you want to have support for TryLock in WIN32 environment,
		// uncomment following line and comment throw and return statement
		// You must also uncomment lines at the begining of this file.
		// WARNING : read comments on the top of this file carefully
		/* 
		return (TryEnterCriticalSection(&handler_) != 0); 
		//*/
		throw ::std::logic_error ("CCriticalSection::TryLock() not supported");
		return false;
#elif defined __unix__
		// does not check for errors
		return (::pthread_mutex_trylock (&handler_) == 0);
#else
		return false;
#endif
	}

	inline void CCriticalSection::UnLock()
	{
#ifdef _WIN32
		::LeaveCriticalSection (&handler_);
#elif defined __unix__
		if (::pthread_mutex_unlock (&handler_) != 0) {
			::std::perror ("CCriticalSection::UnLock()");
			throw ::std::runtime_error ("CCriticalSection::UnLock()");
		}
#endif
	}


	inline CScopeLock::CScopeLock (CCriticalSection &handler)
	:handler_(handler)
	{
		handler_.Lock();
	}

	inline CScopeLock::~CScopeLock()
	{
		handler_.UnLock();
	}

} // namespace util

#endif // H_KORAX_UTIL_SYNC
