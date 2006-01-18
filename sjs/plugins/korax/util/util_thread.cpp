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

See header file for more information

*/

// C++ standard (ISO/IEC 14882:2003)
#include <exception>		// exception
#include <stdexcept>		// runtime_error
#include <cstdio>			// perror()
#include <vector>			// vector

// Platform dependent
#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
#	include <process.h>		// _beginthreadex()
#	include <windows.h>		// Sleep()
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <unistd.h>		// usleep(), sleep()
#endif

// koraX's utils
#include "util_sync.h"
#include "util_sys.h"
#include "util_thread.h"

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//-------------------------------------------------------------------------------

namespace util {


	CThread::CThread ()
	:threadId_(0),isIndependent_(false),data_(0),isCreated_(false),isStarted_(false),isFinished_(false)
#ifdef _WIN32
	,handle_(INVALID_HANDLE_VALUE)
#endif
	{
//		printf("CThread::CThread()\n");
	}


	CThread::~CThread ()
	{
		if (!isIndependent_) {
//			printf("CThread::~CThread() - waiting for thread to finish\n");
#ifdef _WIN32
			if(isCreated_)
				WaitForSingleObject(handle_,INFINITE);
#elif defined __unix__
			if(isCreated_)
				pthread_join(threadId_, 0);
#endif
		}
//		printf("CThread::~CThread() - thread probably finished\n");
#ifdef _WIN32
		CloseHandle (handle_);
		handle_ = INVALID_HANDLE_VALUE;
#endif
//		printf("CThread::~CThread()\n");
	}


	// this is only for STL support
	CThread::CThread (const CThread& thread)
	:threadId_(0),isIndependent_(false),data_(0),isCreated_(false),isStarted_(false),isFinished_(false)
#ifdef _WIN32
	,handle_(INVALID_HANDLE_VALUE)
#endif
	{
		// do only if thread has not started
		if(thread.getCreated() || thread.getStarted() || thread.getFinished() || thread.getId()) {
			throw::std::runtime_error("CThread::CThread(const CThread&) - thread already started");
		}
	}

	// this is only for STL support
	// override it in derived class if you wish
	CThread& CThread::operator= (const CThread& thread)
	{
		// do only if thread has not started
		if(thread.getCreated() || thread.getStarted() || thread.getFinished() || thread.getId()) {
			throw::std::runtime_error("CThread::operator=() - thread already started");
		}
		return *this;
	}

	bool CThread::WaitForEnd () const
	{
		if(!(getCreated() && getStarted()))
			return false;
		if(getFinished())
			return true;
#ifdef _WIN32
		WaitForSingleObject (handle_,INFINITE);
#elif defined __unix__
		pthread_join (threadId_, 0);
#endif
		return true;
	}

	// override at will
	void CThread::AtExit ()
	{
//		printf("CThread::AtExit()\n");
	}

	void ThreadAtExit (::util::CThread *thread)
	{
	    try {
			thread->AtExit();
		} catch (::std::exception* e) {
	        delete e;
	    } catch (...) {
		}

		// delete thread
		if (thread->isIndependent_==true) {
//			printf("::ThreadAtExit() - independent thread, mazem\n");
			delete thread;
//			printf("::ThreadAtExit() - independent thread, zmazany\n");
		} else {
			thread->isFinished_ = true;
		}
	}

#ifdef _WIN32
//	DWORD WINAPI ThreadProc(LPVOID pointer)
	unsigned int WINAPI ThreadProc (void *pointer)
#elif defined __unix__
	void* ThreadProc (void *pointer)
#endif
	{
		CThread *thread = reinterpret_cast<CThread*>(pointer);
		thread->isStarted_ = true;
//		printf("::ThreadProc() - ja som novy thread a mam id %ld\n",thread->getId());
		try {
//			printf("::ThreadProc() - idem volat main\n");
			thread->Main();
//			printf("::ThreadProc() - main skoncil\n");
		} catch (...) {
			ThreadAtExit (thread);
			throw;
		}
//		printf("::ThreadProc() - idem volat konec\n");
		ThreadAtExit (thread);

//		printf("::ThreadProc()\n");
		return 0;
	}

	bool CThread::Start (void *data)
	{
		SCOPE_LOCK (criticalSection_);

		// can call this only once
		if (isFinished_ || isStarted_ || isCreated_)
			return false;

		data_ = data;
//		printf("CThread::Start() - startujem thread\n");
		// NOTE : error C3861: '_beginthreadex': identifier not found, even with argument-dependent lookup
		// To fix it, set Project - Properties - C/C++ - Code Generation - Runtime Library - Multi-threaded
//		if((handle_ = CreateThread (0, 0, ThreadProc, this, 0, &threadId_)) == NULL) {
#ifdef _WIN32
		if ((handle_ = reinterpret_cast<HANDLE>(_beginthreadex (0, 0, ThreadProc, this, 0, (unsigned int*)&threadId_))) == NULL) {
			::std::perror ("CThread::Start()");
			return false;
		}
#elif defined __unix__
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, isIndependent_ ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
		if (pthread_create (&threadId_, &attr, ThreadProc, this) != 0) {
//		::std::perror ("CThread::Start()");
            return false;
		}
        pthread_attr_destroy (&attr);
#endif
//		printf("CThread::Start()\n");
		isCreated_ = true;
		return true;
	}

} // namespace util
