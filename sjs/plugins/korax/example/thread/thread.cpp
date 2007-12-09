/*
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CThread class
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

// Platform dependent
#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
#	include <windows.h>		// Sleep()
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <unistd.h>		// getpid(), usleep(), sleep()
#endif

using namespace util;
using ::std::clog;
using ::std::string;

//--------------------------------------------------------------------

struct SampleData {
	CCriticalSection criticalSection_; // for syunchronization
	string	name_;
	int		age_;
	uint32	salary_;
	uint32	money_;
};

class SampleThread : public CThread
{
public:
	virtual void Main();
	virtual void AtExit();
private:
	static int count_;
	int local_;
};

int SampleThread::count_ = 0;

void SampleThread::Main()
{
	local_ = ++count_;
	SampleData &sampleData = *reinterpret_cast<SampleData *>(data_);
	while (sampleData.age_ < 100) {
		sampleData.criticalSection_.Lock();
		if (sampleData.age_ >= 100) {
			// age could change while we waited for releasing locked critical section
			sampleData.criticalSection_.UnLock();
			break;
		}
		clog << local_ << ". thread : " << sampleData.name_ << " is " << sampleData.age_ << " years old and has " << sampleData.money_ << "$" << ::std::endl;
#ifdef _WIN32
		::Sleep (1);
#elif defined __unix__
		usleep (1000);
#endif
		sampleData.money_ += sampleData.salary_;
		++sampleData.age_;
		sampleData.criticalSection_.UnLock();
#ifdef _WIN32
		::Sleep (1);
#elif defined __unix__
		usleep (1000);
#endif
	}
}

void SampleThread::AtExit()
{
	SampleData &sampleData = *reinterpret_cast<SampleData *>(data_);
	sampleData.criticalSection_.Lock();
	if (sampleData.age_ == 100)
	clog << local_ << ". thread : " << sampleData.name_ << " is dead and his wife has all his money" << ::std::endl;
	sampleData.criticalSection_.UnLock();
}

//--------------------------------------------------------------------

int main (int /*argc*/, char* /*argv[]*/)
{
	// note that cout is redirected to file. You can change it in util_log.cpp

	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using CThread class" << ::std::endl;


	SampleData data;
	data.name_		= "John Doe";
	data.age_		= 82;
	data.salary_	= 12000;
	data.money_		= 302500;

	// run normal Threads
	clog << "\nStarting normal thread..." << ::std::endl;
	SampleThread normalThread;
	normalThread.Start (&data);

	// thread is running

	// wait for end of the thread
	while (!normalThread.WaitForEnd()) {
#ifdef _WIN32
		::Sleep (10);
#elif defined __unix__
		usleep (10000);
#endif
	}

	//--------------------------------------------------------------------

	// run independent Threads
	clog << "\nNow we start 2 Independent thread and then wait for 1 second ..." << ::std::endl;
	data.name_		= "Poor Bill";
	data.age_		= 74;
	data.salary_	= 37;
	data.money_		= 1;
	createIndependent<SampleThread>(&data);
	createIndependent<SampleThread>(&data);

	// thread is running

	// independent threads are good if we do not want for them to end. However we must not finish program
	// before they ends. Independent threads deletes themself automatically

	// wait a bit
#ifdef _WIN32
	::Sleep (1000);
#elif defined __unix__
	sleep (1);
#endif

	clog << "\nWe cannot determine when independent thread ended.\nPress return to quit if you feel thread has ended... ";
	clog.flush();
	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');
	return 0;
}
