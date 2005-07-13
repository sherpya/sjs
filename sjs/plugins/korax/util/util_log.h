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

Name       : CLog class
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : singleton class
Threadsafe : yes
Depends on : CSys, koraX's synchronization classes, miscellaneous
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_LOG
#define H_KORAX_UTIL_LOG

// __FUNCTION__ macro is not C++ standard macro, but is it widely supported
#ifndef __FUNCTION__
#define __FUNCTION__ "N/A"
#endif

// C++ standard (ISO/IEC 14882:2003)
#include <iostream>		// clog, cerr, cout
#include <streambuf>	// streambuf
#include <fstream>		// ofstream
#include <string>		// string

// koraX's utils
#include "util_sync.h"

//-------------------------------------------------------------------------------

// WARNING : you should not use these 5 macros in static functions
//           use util::CLog::getInstance().Report() instead
//#define LOG(a,b)		::util::log.Report (a, __FILE__, __LINE__, __FUNCTION__, b);
#define LOG_INFO(a)		::util::log.Report (a, __FILE__, __LINE__, __FUNCTION__, ::util::CLog::eltInfo)
#define LOG_WARNING(a)	::util::log.Report (a, __FILE__, __LINE__, __FUNCTION__, ::util::CLog::eltWarning)
#define LOG_ERROR(a)	::util::log.Report (a, __FILE__, __LINE__, __FUNCTION__, ::util::CLog::eltError)
#define LOG_FUNCTION(a) ::util::CLogFunction __korax_log_temp_function(a, __FILE__, __LINE__, __FUNCTION__)

//-------------------------------------------------------------------------------

namespace util {

	class CLog
	{
	public: // Interface
		enum eLogType {
			eltInfo				= 0,
			eltWarning			= 1,
			eltError			= 2,
			eltFunctionEnter	= 3,
			eltFunctionExit		= 4
		};

	static	CLog&	getInstance();
			void	Report (const ::std::string &message, const ::std::string &file, unsigned long line, const ::std::string &function, eLogType type = eltInfo);

	private: // Implementation
		CLog (const CLog&);				// not supported
		CLog& operator= (const CLog&);	// not supported
		 CLog();
		~CLog();

		::std::ofstream log_;
		::std::ofstream cout_;
		::std::ofstream cerr_;

		// old standard stream buffers
		::std::streambuf *oldCout_;
		::std::streambuf *oldCerr_;

		// log entry ID, choose one
		unsigned int id_;	// 32bit or sometimes even 64bit :)
		//uint64 id_;		// 64bit

		unsigned long processId_;
		unsigned long threadId_;

		const ::std::string directory_;

		CCriticalSection criticalSection_;
	};

	//-------------------------------------------------------------------------------

	class CLogFunction
	{
	public: // Interface
		 CLogFunction (const ::std::string &message, const ::std::string &file, unsigned long line, const ::std::string &function);
		~CLogFunction ();

	private: // Implementation
		::std::string	message_;
		::std::string	file_;
		::std::string	function_;
		unsigned long	line_;
	};

	//-------------------------------------------------------------------------------

	extern CLog &log; // the only instance of CLog

	//-- inline functions -----------------------------------------------------------

	inline CLogFunction::CLogFunction (const ::std::string &message, const ::std::string &file, unsigned long line, const ::std::string &function)
	:message_(message),file_(file),line_(line),function_(function)
	{
		// WARNING : If you use CLogFunction in some static function, log may not be initialized yet
		//           In this case, replace log.Report() with safer but slower CLog::getInstance().Report()
		log.Report (message_, file_, line_, function_, CLog::eltFunctionEnter);
	}

	inline CLogFunction::~CLogFunction ()
	{
		// WARNING : If you use CLogFunction in some static function, log may not be initialized yet
		//           In this case, replace log.Report() with safer but slower CLog::getInstance().Report()
		log.Report (message_, file_, line_, function_, CLog::eltFunctionExit);
	}

} // namespace util

#endif // H_KORAX_UTIL_LOG
