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

Name       : CThread class
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : virtual class
Threadsafe : yes
Depends on : miscellaneous
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_THREAD
#define H_KORAX_UTIL_THREAD

// C++ standard (ISO/IEC 14882:2003)
#include <iostream>			// ostream, clog

// Platform dependent
#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
#	include <windows.h>		// GetCurrentProcessId(), GetCurrentThreadId()
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <sys/types.h>	// pthread_attr_t, pthread_t
#	include <pthread.h>		// pthread_self(), pthread_*()
#endif

// koraX's utils
#include "util_misc.h"
#include "util_sync.h"

//-------------------------------------------------------------------------------

namespace util {

	//-------------------------------------------------------------------------------

	class CThread
	{
	public: // Interface
		CThread();				// constructor
		virtual ~CThread();		// destructor
		CThread (const CThread&);
		virtual CThread& operator= (const CThread&);

		// TODO : set thread priority SetThreadPriority
		bool			Start		(void *data = 0);	// create and run new thread
		bool			WaitForEnd	() const;

		unsigned long	getId		() const;			// get Thread ID
		bool			getCreated	() const;
		bool			getStarted	() const;
		bool			getFinished	() const;

	protected: // Interface for Derived classes
		virtual void	Main		() = 0;	// main() of new thread
		virtual void	AtExit		();		// called just before thread will exit
		void			*data_;				// data for your thread

		friend void ThreadAtExit (CThread *thread);
#ifdef _WIN32
//		DWORD WINAPI ThreadProc(LPVOID pointer)
	    friend unsigned int WINAPI	ThreadProc (void *pointer);
#elif defined __unix__
		friend void*				ThreadProc (void *pointer);
#endif

	private: // Implementation
		// Independent creation support, see util_misc.h
		bool isIndependent_;
		bool startIndependent (void *data = 0);
		template<class T> friend void createIndependent (void *);

		pthread_t threadId_;
#ifdef _WIN32
		HANDLE handle_;
#endif
		bool isCreated_;
		bool isStarted_;
		bool isFinished_;

		CCriticalSection criticalSection_;
	};

	//-- inline functions  ----------------------------------------------------------

	inline unsigned long CThread::getId () const
	{
		return threadId_;
	}

	inline bool CThread::getCreated () const
	{
		return isCreated_;
	}

	inline bool CThread::getStarted () const
	{
		return isStarted_;
	}

	inline bool CThread::getFinished () const
	{
		return isFinished_;
	}

	inline bool CThread::startIndependent (void *data)
	{
		return Start (data);
	}

} // namespace util

#endif // H_KORAX_UTIL_THREAD
