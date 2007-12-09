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

Name       : CVector class
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : class
Threadsafe : no
Depends on : misc utilities
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_VECTOR
#define H_KORAX_UTIL_VECTOR

// C++ standard (ISO/IEC 14882:2003)
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <string>

// koraX's utils
#include "util_misc.h"

// support for HL engine
//#define SDK_VECTOR_SUPPORT 1

#ifdef SDK_VECTOR_SUPPORT
namespace engine {
	namespace SDK {
#		include "../engine/sdk/extdll.h"
	}
}
#endif

//-------------------------------------------------------------------------------

namespace util {

	//-------------------------------------------------------------------------------

	class CBaseVector // use derived classes instead
	{
	public: // Interface
		 CBaseVector();
		~CBaseVector();
		CBaseVector (float x, float y, float z);
		CBaseVector (const CBaseVector& rv);

		bool			isEqual	(const CBaseVector& rv, unsigned int precision = 8400) const;
		::std::string	Log		(unsigned char precision = 2) const;

#ifdef SDK_VECTOR_SUPPORT
		CBaseVector	(const ::engine::SDK::Vector& rv);
		operator ::engine::SDK::Vector () const;
		operator float *();
		operator const float *() const;
#endif

	protected: // Implementation
		float x_, y_, z_;
	};

	//-- inline functions -----------------------------------------------------------

	inline CBaseVector::CBaseVector ()
	:x_(0),y_(0),z_(0)
	{
	}

	inline CBaseVector::~CBaseVector ()
	{
	}

	inline CBaseVector::CBaseVector (float x, float y, float z)
	:x_(x),y_(y),z_(z)
	{
	}

	inline CBaseVector::CBaseVector (const CBaseVector& rv)
	:x_(rv.x_),y_(rv.y_),z_(rv.z_)
	{
	}

	// output "(1.23,4.56,7.89)"
	inline ::std::string CBaseVector::Log (unsigned char precision) const
	{
		::std::ostringstream a;
		a << ::std::setprecision (precision) <<::std::fixed << "(" << x_ << "," << y_ << "," << z_ << ")";
		return a.str();
	}

	inline bool CBaseVector::isEqual (const CBaseVector& rv, unsigned int precision) const
	{
		return ::util::isEqual (x_, rv.x_, precision) && ::util::isEqual (y_, rv.y_, precision) && ::util::isEqual (z_, rv.z_, precision);
	}

	//-------------------------------------------------------------------------------

#ifdef SDK_VECTOR_SUPPORT
	inline CBaseVector::CBaseVector (const ::engine::SDK::Vector& rv)
	:x_(rv.x),y_(rv.y),z_(rv.z)
	{
	}

	inline CBaseVector::operator ::engine::SDK::Vector () const
	{
		return ::engine::SDK::Vector (x_, y_, z_);
	}

	inline CBaseVector::operator float *()
	{
		return &x_;
	}

	inline CBaseVector::operator const float *() const
	{
		return &x_;
	}
#endif

	//-------------------------------------------------------------------------------
	class CAngle;

	class CVector : public CBaseVector
	{
	public: // Interface
		 CVector();
		~CVector();
		CVector (float x, float y, float z);
		CVector (const CVector& rv);
		CVector (const CBaseVector& rv);

#ifdef SDK_VECTOR_SUPPORT
		CVector	(const ::engine::SDK::Vector& rv);
#endif

		CVector&		operator=		(const CVector& rv);
		CVector&		operator+=		(const CVector& rv);
		CVector&		operator-=		(const CVector& rv);
		CVector			operator+		(const CVector& rv)			const;
		CVector			operator-		(const CVector& rv)			const;
		CVector			operator-		()							const;
		CVector			operator*		(float value)				const;
		CVector			operator/		(float value)				const;

		//-- get functions --------------------------------------------------------------
		float			getX			()							const;
		float			getY			()							const;
		float			getZ			()							const;

		//-- set functions --------------------------------------------------------------
		void			setX			(float value);
		void			setY			(float value);
		void			setZ			(float value);

		//-------------------------------------------------------------------------------
		float			Length			()							const;
		float			LengthEx		(char type = 2)				const;

		bool			isZero			(float precision = 0.01f)	const; // every        parameter is zero
		bool			isZeroOr		(float precision = 0.01f)	const; // at least one parameter is zero

		CVector&		Normalize		();
		CVector&		Invert			();
		CVector&		toNegative		();
		CVector&		toPositive		();
		CVector&		CutNegative		();
		CVector&		CutPositive		();
		CVector&		CrossProduct	(const CVector &a, const CVector &b);
		float			DotProduct		(const CVector &b)			const;

		float			Angle			(const CVector &v)			const;
		CAngle			toAngles		()							const;
	};

	//-- inline functions -----------------------------------------------------------

	inline CVector::CVector ()
	:CBaseVector(0, 0, 0)
	{
	}

	inline CVector::~CVector ()
	{
	}

	inline CVector::CVector (float x, float y, float z)
	:CBaseVector(x, y, z)
	{
	}

	inline CVector::CVector (const CVector& rv)
	:CBaseVector(rv)
	{
	}

	inline CVector::CVector (const CBaseVector& rv)
	:CBaseVector(rv)
	{
	}

#ifdef SDK_VECTOR_SUPPORT
	inline CVector::CVector (const ::engine::SDK::Vector& rv)
	:CBaseVector(rv)
	{
	}
#endif

	inline CVector& CVector::operator= (const CVector& rv)
	{
		x_ = rv.x_;
		y_ = rv.y_;
		z_ = rv.z_;
		return *this;
	}

	inline CVector CVector::operator+ (const CVector& rv) const
	{
		return CVector(x_ + rv.x_, y_ + rv.y_, z_ + rv.z_);
	}

	inline CVector& CVector::operator+= (const CVector& rv)
	{
		x_ += rv.x_;
		y_ += rv.y_;
		z_ += rv.z_;
		return *this;
	}

	inline CVector CVector::operator- (const CVector& rv) const
	{
		return CVector(x_ - rv.x_, y_ - rv.y_, z_ - rv.z_);
	}

	inline CVector& CVector::operator-= (const CVector& rv)
	{
		x_ -= rv.x_;
		y_ -= rv.y_;
		z_ -= rv.z_;
		return *this;
	}

	inline CVector CVector::operator- () const
	{
		return CVector(-x_, -y_, -z_);
	}

	inline CVector CVector::operator* (float value) const
	{
		return CVector(x_ * value, y_ * value, z_ * value);
	}

	inline CVector CVector::operator/ (float value) const
	{
		return CVector(x_ / value, y_ / value, z_ / value);
	}

	//-------------------------------------------------------------------------------

	inline float CVector::getX () const
	{
		return x_;
	}

	inline float CVector::getY () const
	{
		return y_;
	}

	inline float CVector::getZ () const
	{
		return z_;
	}

	//-------------------------------------------------------------------------------

	inline void CVector::setX (float value)
	{
		x_ = value;
	}

	inline void CVector::setY (float value)
	{
		y_ = value;
	}

	inline void CVector::setZ (float value)
	{
		z_ = value;
	}

	//-------------------------------------------------------------------------------

	inline bool CVector::isZero (float value) const
	{
		return ::util::isZero(x_, value) && ::util::isZero(y_, value) && ::util::isZero(z_, value);
	}

	inline bool CVector::isZeroOr (float value) const
	{
		return ::util::isZero(x_, value) || ::util::isZero(y_, value) || ::util::isZero(z_, value);
	}

	inline float CVector::Length () const
	{
		return ::std::sqrt ((x_ * x_) + (y_ * y_) + (z_ * z_));
	}

	inline float CVector::LengthEx (char type) const
	{
		if (type == -1)
			return ::util::max (::std::fabs(x_), ::std::fabs(y_), ::std::fabs(z_));
		if (type == 1)
			return ::std::fabs(x_) + ::std::fabs(y_) + ::std::fabs(z_);
		if (type == 2)
			return ::std::sqrt ((x_ * x_) + (y_ * y_) + (z_ * z_));
		return ::std::pow (::std::pow (::std::fabs(x_), type) + ::std::pow (::std::fabs(y_), type) + ::std::pow (::std::fabs(z_), type), 1.0f / type);
	}

	inline CVector& CVector::Invert ()
	{
		x_ *= -1;
		y_ *= -1;
		z_ *= -1;
		return *this;
	}

	inline CVector& CVector::toPositive ()
	{
		if (x_ < 0)
			x_ *= -1;
		if (y_ < 0)
			y_ *= -1;
		if (z_ < 0)
			z_ *= -1;
		return *this;
	}

	inline CVector& CVector::toNegative ()
	{
		if (x_ > 0)
			x_ *= -1;
		if (y_ > 0)
			y_ *= -1;
		if (z_ > 0)
			z_ *= -1;
		return *this;
	}

	inline CVector& CVector::CutNegative ()
	{
		if (x_ < 0)
			x_ = 0;
		if (y_ < 0)
			y_ = 0;
		if (z_ < 0)
			z_ = 0;
		return *this;
	}

	inline CVector& CVector::CutPositive ()
	{
		if (x_ > 0)
			x_ = 0;
		if (y_ > 0)
			y_ = 0;
		if (z_ > 0)
			z_ = 0;
		return *this;
	}

	inline float CVector::DotProduct (const CVector &b) const
	{
		return (x_ * b.x_) + (y_ * b.y_) + (z_ * b.z_);
	}

	inline CVector& CVector::CrossProduct (const CVector &a, const CVector &b)
	{
		x_ = (a.y_ * b.z_) - (a.z_ * b.y_);
		y_ = (a.z_ * b.x_) - (a.x_ * b.z_);
		z_ = (a.x_ * b.y_) - (a.y_ * b.x_);
		return *this;
	}

	//-------------------------------------------------------------------------------

	class CAngle : public CBaseVector
	{
	public: // Interface
		 CAngle();
		~CAngle();
		CAngle (float x, float y, float z);
		CAngle (const CAngle& rv);

#ifdef SDK_VECTOR_SUPPORT
		CAngle (const ::engine::SDK::Vector& rv);
#endif

		CAngle&			operator=		(const CAngle& rv);
		CAngle&			operator+=		(const CAngle& rv);
		CAngle&			operator-=		(const CAngle& rv);
		CAngle			operator+		(const CAngle& rv)	const;
		CAngle			operator-		(const CAngle& rv)	const;

		//-- get functions --------------------------------------------------------------
		float			getPitch		()					const;
		float			getYaw			()					const;
		float			getRoll			()					const;

		//-- set functions --------------------------------------------------------------
		void			setPitch		(float value);
		void			setYaw			(float value);
		void			setRoll			(float value);

		//-------------------------------------------------------------------------------
		CVector			toForward		()					const;
		CVector			toRight			()					const;
		CVector			toUp			()					const;

		CAngle&			Fix				(bool isBodyAngles = false);
		CAngle&			FixPitch		(bool isBodyAngles = false);
		CAngle&			FixRoll			();
		CAngle&			FixYaw			();

		static float	FixPitch		(float pitch, bool isBodyAngles = false);
		static float	FixRoll			(float roll);
		static float	FixYaw			(float yaw);
	};

	//-- inline functions -----------------------------------------------------------

	inline CAngle::CAngle ()
	:CBaseVector(0, 0, 0)
	{
	}

	inline CAngle::~CAngle ()
	{
	}

	inline CAngle::CAngle (float x, float y, float z)
	:CBaseVector(x ,y, z)
	{
	}

	inline CAngle::CAngle (const CAngle& rv)
	:CBaseVector(rv)
	{
	}

#ifdef SDK_VECTOR_SUPPORT
	inline CAngle::CAngle (const engine::SDK::Vector& rv)
	:CBaseVector(rv)
	{
	}
#endif

	inline CAngle& CAngle::operator= (const CAngle& rv)
	{
		x_ = rv.x_;
		y_ = rv.y_;
		z_ = rv.z_;
		return *this;
	}

	inline CAngle& CAngle::operator+= (const CAngle& rv)
	{
		x_ += rv.x_;
		y_ += rv.y_;
		z_ += rv.z_;
		return *this;
	}

	inline CAngle& CAngle::operator-= (const CAngle& rv)
	{
		x_ += rv.x_;
		y_ += rv.y_;
		z_ += rv.z_;
		return *this;
	}

	inline CAngle CAngle::operator+ (const CAngle& rv) const
	{
		return CAngle(x_ + rv.x_, y_ + rv.y_, z_ + rv.z_);
	}

	inline CAngle CAngle::operator- (const CAngle& rv) const
	{
		return CAngle(x_ - rv.x_, y_ - rv.y_, z_ - rv.z_);
	}

	//-------------------------------------------------------------------------------

	inline float CAngle::getPitch () const
	{
		return x_;
	}

	inline float CAngle::getYaw () const
	{
		return y_;
	}

	inline float CAngle::getRoll () const
	{
		return z_;
	}

	//-------------------------------------------------------------------------------

	inline void CAngle::setPitch (float value)
	{
		x_ = value;
	}

	inline void CAngle::setYaw (float value)
	{
		y_ = value;
	}

	inline void CAngle::setRoll (float value)
	{
		z_ = value;
	}

	inline CAngle& CAngle::Fix (bool isBodyAngles)
	{
		FixPitch	(isBodyAngles);
		FixYaw		();
		FixRoll		();
		return *this;
	}

	inline CAngle& CAngle::FixPitch (bool isBodyAngles)
	{
		x_ = FixPitch (x_, isBodyAngles);
		return *this;
	}

	inline CAngle& CAngle::FixYaw ()
	{
		y_ = FixYaw (y_);
		return *this;
	}

	inline CAngle& CAngle::FixRoll ()
	{
		z_ = FixRoll (z_);
		return *this;
	}

	//-------------------------------------------------------------------------------

	class CPosition : public CBaseVector
	{
	public: // Interface
		 CPosition();
		~CPosition();
		CPosition (float x, float y, float z);
		CPosition (const CPosition& rv);

#ifdef SDK_VECTOR_SUPPORT
		CPosition (const ::engine::SDK::Vector& rv);
#endif

		CPosition&		operator=		(const CPosition& rv);
		CPosition&		operator+=		(const CVector& rv);
		CPosition		operator+		(const CVector& rv)		const;
		CVector			operator-		(const CPosition& rv)	const;

		//-- get functions --------------------------------------------------------------
		float			getX			()						const;
		float			getY			()						const;
		float			getZ			()						const;

		//-- set functions --------------------------------------------------------------
		void			setX			(float value);
		void			setY			(float value);
		void			setZ			(float value);
	};

	//-- inline functions -----------------------------------------------------------

	inline CPosition::CPosition ()
	:CBaseVector(0, 0, 0)
	{
	}

	inline CPosition::~CPosition ()
	{
	}

	inline CPosition::CPosition (float x, float y, float z)
	:CBaseVector(x, y, z)
	{
	}

	inline CPosition::CPosition (const CPosition& rv)
	:CBaseVector(rv)
	{
	}

#ifdef SDK_VECTOR_SUPPORT
	inline CPosition::CPosition (const ::engine::SDK::Vector& rv)
	:CBaseVector(rv)
	{
	}
#endif

	inline CPosition& CPosition::operator= (const CPosition& rv)
	{
		x_ = rv.x_;
		y_ = rv.y_;
		z_ = rv.z_;
		return *this;
	}

	inline CPosition CPosition::operator+ (const CVector& rv) const
	{
		return CPosition(x_ + rv.getX(), y_ + rv.getY(), z_ + rv.getZ());
	}

	inline CPosition& CPosition::operator+= (const CVector& rv)
	{
		x_ += rv.getX();
		y_ += rv.getY();
		z_ += rv.getZ();
		return *this;
	}

	inline CVector CPosition::operator- (const CPosition& rv) const
	{
		return CVector(x_ - rv.x_, y_ - rv.y_, z_ - rv.z_);
	}

	//-------------------------------------------------------------------------------

	inline float CPosition::getX () const
	{
		return x_;
	}

	inline float CPosition::getY () const
	{
		return y_;
	}

	inline float CPosition::getZ () const
	{
		return z_;
	}

	//-------------------------------------------------------------------------------

	inline void CPosition::setX (float value)
	{
		x_ = value;
	}

	inline void CPosition::setY (float value)
	{
		y_ = value;
	}

	inline void CPosition::setZ (float value)
	{
		z_ = value;
	}

} // namespace util

#endif // H_KORAX_UTIL_VECTOR
