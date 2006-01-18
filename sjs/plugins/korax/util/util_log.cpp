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

NOTE : you should change directory constant

*/

// koraX's utils
#include "util_sys.h"
#include "util_log.h"

//#define KORAX_UTIL_BUILD_SUPPORT 1 // comment this if you don't use koraXs Build number logger
#define KORAX_UTIL_COUT 1 // comment this if you don't want to redirect cout

// build number
#ifdef KORAX_UTIL_BUILD_SUPPORT
	#include "../version/util_build.h"
#endif

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//-------------------------------------------------------------------------------

namespace util {

// create instance of CLog
CLog &log = CLog::getInstance();

//-------------------------------------------------------------------------------

CLog& CLog::getInstance ()
{
	static CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

	static CLog lLog;
	return lLog;
}

CLog::CLog ()
:id_(0),directory_("")
{
	// force create CSys object
	CSys &lSys = CSys::getInstance();

	// turn off synchronization, for better performance	
	::std::ios_base::sync_with_stdio (false);

	// save standard stream buffers
	oldCout_ = ::std::cout.rdbuf();
	oldCerr_ = ::std::cerr.rdbuf();

	// redirect cerr :
	bool resized = true;
	cerr_.open ((directory_ + "cerr.log").c_str());
	if (!cerr_)
		::std::cerr << "::util::CLog::CLog() - cannot create cerr.log" << ::std::endl;
	else {
		if (cerr_.rdbuf()->pubsetbuf (0, 0) == false)
			resized = false;
		::std::cerr.rdbuf (cerr_.rdbuf());
		if (resized == false)
			::std::cerr << "::util::CLog::CLog() - cannot resize file buffer for cerr" << ::std::endl;
	}

	// redirect cout :
	cout_.open ((directory_ + "cout.log").c_str());
	if (!cout_)
		::std::cerr << "::util::CLog::CLog() - cannot create cout.log" << ::std::endl;
	else {
		// set 0 for debug, 4096 for release
		if (cout_.rdbuf()->pubsetbuf (0, 4096) == false)
			::std::cerr << "::util::CLog::CLog() - cannot resize file buffer for cout" << ::std::endl;
#ifdef KORAX_UTIL_COUT
		::std::cout.rdbuf (cout_.rdbuf());
#endif
	}

	processId_ = lSys.getProcessId();
	threadId_  = lSys.getThreadId();

	// create log :
	log_.open ((directory_ + "log.xml").c_str());
	if (!log_)
		::std::cerr << "::util::CLog::CLog() - cannot create log.xml" << ::std::endl;
	else {
		// set 0 for debug, 4096 for release
		if (log_.rdbuf()->pubsetbuf (0, 4096) == false)
			::std::cerr << "::util::CLog::CLog() - cannot resize file buffer for log" << ::std::endl;
		log_ << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<log>\n"
			 << "\t<info>\n\t\t<cpu unit=\""					<< lSys.getCpuUnits().c_str() 
			 << "\">"											<< lSys.getCpuFrequency() 
			 << "</cpu>\n\t\t<overhead unit=\""					<< lSys.getOverheadUnits().c_str() 
			 << "\">"											<< lSys.getOverhead() 
			 << "</overhead>\n\t\t<date format=\""				<< lSys.getDateFormat().c_str() 
			 << "\">"											<< lSys.getDate().c_str() 
			 << "</date>\n\t\t<time format=\""					<< lSys.getTimeFormat().c_str() 
			 << "\">"											<< lSys.getTime().c_str() 
			 << "</time>\n\t\t<pid>"							<< processId_ 
			 << "</pid>\n\t\t<tid>"								<< threadId_ 
			 << "</tid>"
#ifdef H_KORAX_UTIL_BUILD
			 << "\n\t\t<build>\n\t\t\t"							<< KORAX_UTIL_BUILD_NUMBER 
			 << "\n\t\t\t<date format=\""						<< KORAX_UTIL_BUILD_DATE_FORMAT 
			 << "\">"											<< KORAX_UTIL_BUILD_DATE 
			 << "</date>\n\t\t\t<time format=\""				<< KORAX_UTIL_BUILD_TIME_FORMAT 
			 << "\">"											<< KORAX_UTIL_BUILD_TIME 
			 << "</time>\n\t\t</build>"
#endif
			 << "\n\t</info>\n\t<start>"	<< CSys::getCycles() 
			 << "</start>\n\t<entries>" 
			 << ::std::endl;
	}

//	::std::cout << "util::CLog::CLog()" << ::std::endl;
}


CLog::~CLog ()
{
	// restore original streams
	::std::cout.flush();
	::std::cout.rdbuf (oldCout_);
	::std::cerr.flush();
	::std::cerr.rdbuf (oldCerr_);

	// turn synchronization back on
	::std::ios_base::sync_with_stdio (true);

	// close files
	if (log_) {
		log_ << "\t</entries>\n\t<end  >" << CSys::getCycles() 
			 << "</end>\n</log>"
			 << ::std::endl;
		log_.close();
	}
	cout_.close();
	cerr_.close();

//	::std::cout << "util::CLog::~CLog()" << ::std::endl;
}

void CLog::Report (const ::std::string &message, const ::std::string &file, unsigned long line, const ::std::string &function, CLog::eLogType type)
{
	SCOPE_LOCK (criticalSection_);

	// if error or warning, send it also to cerr
	if (type == eltError) {
		::std::cerr << "ERROR in file \""	<< file.c_str() 
					<< "\", line "			<< line 
					<< ", function \""		<< function.c_str() 
					<< "\" : "				<< message.c_str() 
					<< ::std::endl;
	}

	if (type == eltWarning) {
		::std::cerr << "WARNING in file \""	<< file.c_str() 
					<< "\", line "			<< line 
					<< ", function \""		<< function.c_str() 
					<< "\" : "				<< message.c_str() 
					<< ::std::endl;
	}

	// write to log
	log_ << "\t\t<entry id=\"" << id_;
	unsigned int tempInt = CSys::getProcessId();
	if (tempInt != processId_)
		log_ << "\" pid=\"" << tempInt;
	tempInt = CSys::getThreadId();
	if (tempInt != threadId_)
		log_ << "\" tid=\"" << tempInt; 

	log_ << "\" cycles=\""		<< CSys::getCycles() 
//		 << "\" date=\""		<< CSys::getDate().c_str() // don't need, will compute from cycles
//		 << "\" time=\""		<< CSys::getTime().c_str() // don't need, will compute from cycles
		 << "\" file=\""		<< file.c_str() 
		 << "\" line=\""		<< line 
		 << "\" function=\""	<< function.c_str() 
		 << "\" type=\""		<< type 
		 << "\">"				<< message.c_str() 
		 << "</entry>\n"; 

	++id_;
}

} // namespace util
