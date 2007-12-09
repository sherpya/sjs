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

Name       : Miscellaneous utils
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : macros, global functions and global variables
Threadsafe : some
Depends on : nothing
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_MISC
#define H_KORAX_UTIL_MISC

// C++ standard (ISO/IEC 14882:2003)
#include <cmath>			// abs
#include <string>			// string
#include <stdexcept>		// runtime_error
#include <sstream>			// istringstream, ostringstream
#include <fstream>			// ifstream

// Platform dependent
#ifdef _WIN32			// Windows API (Microsoft © Platform SDK)
#	include <windows.h>		// DWORD
#elif defined __unix__	// UNIX (The Single UNIX Specification Version 3)
#	include <stdint.h>		// ?int*_t
#endif

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#endif

//-------------------------------------------------------------------------------

namespace util {

	//-------------------------------------------------------------------------------

	// data types
#ifdef _WIN32	// Windows specific declarations
	typedef DWORD   pthread_t;			// used in CSys::getThreadId()
	typedef DWORD   pid_t;				// used in CSys::getProcessId()
	typedef __int8  int8;
//	typedef __int16 int16; // mingw has problem with it
	typedef short int16;
	typedef __int32 int32;
	typedef __int64 int64;
	typedef unsigned __int8  uint8;
//	typedef unsigned __int16 uint16; // mingw has problem with it
	typedef unsigned short   uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;
#elif defined __unix__	// UNIX specific declarations
	typedef int8_t   int8;
	typedef int16_t  int16;
	typedef int32_t  int32;
	typedef int64_t  int64;
	typedef uint8_t  uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
#endif

	//-------------------------------------------------------------------------------

	typedef void (*pointerToFunction) (void); // for alchemy with pointers to function

	//-------------------------------------------------------------------------------

	// class which wants to support Independent creation must declare
	// bool isIndependent_, startIndependent() function and must declare following
	// function as a friend : template<class T> friend void createIndependent (void *);
	// see util::CThread for example
	// example of calling this function : createIndependent<CMyClass>();
	// while in independent mode, object must destroy itself
	template<class T>
	inline void createIndependent (void *data = 0)
	{
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
		T &handle = *(new(_NORMAL_BLOCK, __FILE__, __LINE__) T());
#else
		T &handle = *(new T());
#endif
		handle.isIndependent_ = true;
		handle.startIndependent (data);
	}

	//-------------------------------------------------------------------------------

	// convert string to something else. I really do not know why this was not included in ISO C++
	// use this for simple types built-in intrinsic types, like int, double, ...
	template<class T>
	inline T fromStringEx (const ::std::string& s, bool failIfLeftoverChars = false, bool failIfNotFound = false)
	{
		T result;
		::std::istringstream i(s);
		char c;
		if (!(i >> result)) {
			// output type should accept assign 0
			// compiler will tell you if it does not
			result = 0;
			if (failIfNotFound)
				throw ::std::runtime_error (::std::string ("::fromStringEx() nothing to convert : ") + s);
		}
		if (failIfLeftoverChars && i.get (c))
			throw ::std::runtime_error (::std::string ("::fromStringEx() some characters not converted : ") + s);
		return result;
	}

	// fast version of fromStringEx
	template<class T>
	inline T fromString (const ::std::string& s)
	{
		T x;
		::std::istringstream i(s);
		if (!(i >> x))
			// output type should support 0 as a value
			// compiler will tell you if it does not
			return 0;
		return x;
	}

	// convert anything convertable to string :)
	template<class T>
	inline ::std::string toString (const T& x)
	{
		::std::ostringstream o;
		if (!(o << x))
			throw ::std::runtime_error ("::toString() cannot convert to string");
		return o.str();
	}

	//-------------------------------------------------------------------------------

	// simple check if file exists
	inline bool FileExists (const ::std::string &fileName)
	{
		::std::ifstream file (fileName.c_str());
		if (!file.is_open())
			return false;
		file.close();
		return true;
	}

	//-------------------------------------------------------------------------------

	// some conversion stuff
	inline uint16 FixedUnsigned16 (float value, float scale = 1.0f)
	{
		int output = static_cast<int>(value * scale);
		if (output < 0)
			output = 0;
		if (output > 0xFFFF)
			output = 0xFFFF;
		return static_cast<uint16>(output);
	}

	inline int16 FixedSigned16 (float value, float scale = 1.0f)
	{
		int output = static_cast<int>(value * scale);
		if (output > 32767)
			output = 32767;
		if (output < -32768 )
			output = -32768;
		return static_cast<int16>(output);
	}

	//-------------------------------------------------------------------------------

	// random functions
	extern long idum_;

	// Numerical Recipes in C - The Art of Scientific Computing - Second Edition
	// http://www.ma.utexas.edu/documentation/nr/bookcpdf.html
	// Chapter 7-1, page 279, ran0() function
	inline float RandomFloat (long &idum = idum_)
	{
		const long IA	= 16807;
		const long IM	= 2147483647;
		const float AM	= static_cast<float>(1.0f / IM);
		const long IQ	= 127773;
		const long IR	= 2836;
		const long MASK = 123459876;

		long k;
		float ans;
		idum ^= MASK;
		k     = idum / IQ;
		idum  = IA * (idum - k * IQ) - IR * k;
		if (idum < 0)
			idum += IM;
		ans	  = AM * idum;
		idum ^= MASK;
		return ans;
	}

	inline long RandomLong (long start, long end, long &idum = idum_)
	{
		return start + static_cast<long>(RandomFloat (idum) * (end + 1 - start));
	}

	//-------------------------------------------------------------------------------

#ifdef RGB
#	undef RGB
#endif
	// simple RGB class
	class RGB {
	public:
		RGB(unsigned char red, unsigned char green, unsigned char blue) {red_ = red; green_ = green; blue_ = blue;}
		RGB(const RGB& rv) {red_ = rv.red_; green_ = rv.green_; blue_ = rv.blue_;}
		RGB& operator= (const RGB& rv) {red_ = rv.red_; green_ = rv.green_; blue_ = rv.blue_; return *this;}
		~RGB() {;}
		unsigned char red		() const	{return red_;}
		unsigned char green	() const	{return green_;}
		unsigned char blue	() const	{return blue_;}
		void red	(unsigned char a)	{red_ = a;}
		void green	(unsigned char a)	{green_ = a;}
		void blue	(unsigned char a)	{blue_ = a;}
	private:
		RGB();
		unsigned char red_, green_, blue_;
	};

	//-------------------------------------------------------------------------------

	// empty string
	extern const ::std::string& emptyString;	// use it instead of NULL pointer when returning strings
	extern const ::std::string& getEmptyString();	// use this if you need emptyString in static function

	//-------------------------------------------------------------------------------

	// math stuff
	extern const float pi;

	template<class T>
	inline T toRadians (T value)
	{
		return static_cast<T>(value * (pi / 180.0f));
	}

	template<class T>
	inline T toDegrees (T value)
	{
		return static_cast<T>(value * (180.0f / pi));
	}

	//-------------------------------------------------------------------------------

	// max
	#ifdef max
	#	undef max
	#endif
	#ifdef min
	#	undef min
	#endif

	template<class T>
	inline T max (T v1, T v2)
	{
		if (v1 > v2)
			return v1;
		return v2;
	}

	template<class T>
	inline T max (T v1, T v2, T v3)
	{
		if (v1 > v2)
			return max (v1, v3);
		return max (v2, v3);
	}

	template<class T>
	inline T min (T v1, T v2)
	{
		if (v1 < v2)
			return v1;
		return v2;
	}

	template<class T>
	inline T min (T v1, T v2, T v3)
	{
		if (v1 < v2)
			return min (v1, v3);
		return min (v2, v3);
	}

	//-------------------------------------------------------------------------------

	// float equality
	// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	inline bool isEqual (float A, float B, unsigned int maxUlps = 8400)
	{
		// 1.001 1.002 -> 8400  - 4 number precision
		// 1.01  1.02  -> 84000 - 3 numbers precision
//	    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
	    // Make aInt lexicographically ordered as a twos-complement int
	    if (*(int*)&A < 0)
			*(int*)&A = 0x80000000 - *(int*)&A;
		// Make bInt lexicographically ordered as a twos-complement int
		if (*(int*)&B < 0)
	        *(int*)&B = 0x80000000 - *(int*)&B;
	    if (static_cast<unsigned int>(::std::abs (*(int*)&A - *(int*)&B)) <= maxUlps)
			return true;
		return false;
	}

	// near zero
	inline bool isZero (float number, float precision = 0.01f) throw()
	{
		return ::std::fabs (number) < precision;
	}

} // namespace util

#endif // H_KORAX_UTIL_MISC
